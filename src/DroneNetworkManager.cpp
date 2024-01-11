#include "DroneNetworkManager.hpp"

#include <Arduino.h>

void dummySpeedCallback(float frontBias, float rightBias, float clockWiseBias) {
    Serial.println("Received SpeedPacket but no Callback set.");
}

void dummyArmCallback(float height, float grip) {
    Serial.println("Received ArmPacket but no Callback set.");
}

void DroneNetworkManager::sendEcho()
{
    this->udp.beginPacket(udp.remoteIP(), udp.remotePort());
    this->udp.write(1);
    this->udp.endPacket();
}

void DroneNetworkManager::sendBatteryState()
{
    this->udp.beginPacket(this->owner, this->ownerPort);
    this->udp.write(100);
    this->udp.endPacket();
}

void DroneNetworkManager::handleSpeedPacket(byte *packet)
{
    bool same = true;
    for (byte i = 0; i < 4; i++) {
        if (this->lastSpeedPacket[i] != packet[i]) {
            this->lastSpeedPacket[i] = packet[i];
            same = false;
        }
    }
    if (same) {
        return;
    }
    float frontbias = packet[1] / 255.f * ((packet[0] & 1) ? -1.f : 1.f);
    float rightbias = packet[2] / 255.f * ((packet[0] & 2) ? -1.f : 1.f);
    float clockwisebias = packet[3] / 255.f * ((packet[0] & 4) ? -1.f : 1.f);
    this->speedCallback(frontbias, rightbias, clockwisebias);
}

void DroneNetworkManager::handleArmPacket(byte *packet)
{
    bool same = true;
    for (byte i = 0; i < 2; i++) {
        if (this->lastArmPacket[i] != packet[i]) {
            this->lastArmPacket[i] = packet[i];
            same = false;
        }
    }
    if (same) {
        return;
    }
    float armAngle = packet[0] / 255.f;
    float grip = packet[1] / 255.f;
    this->armCallback(armAngle, grip);
}

void DroneNetworkManager::cutOwner()
{
    if (this->owned) {
        this->owned == false;
        Serial.println("Owner broke protocol. Open to new Connections.");
        this->armCallback(0.f, 0.f);
        this->speedCallback(0.f, 0.f, 0.f);
    }
}

void DroneNetworkManager::setNewOwner() {
    if (!this->owned) {
        this->owned = true;
        this->owner = this->udp.remoteIP();
        this->ownerPort = this->udp.remotePort();
        this->lastPacketTime = millis();
    }
}

void DroneNetworkManager::init(char *ssid, char *password)
{
    Serial.println("Starting Network Manager.");
    WiFi.begin(ssid, password);
    memset(this->buffer, 0, DRONEBUFFERSIZE);
    memset(this->lastArmPacket, 0, 2);
    memset(this->lastSpeedPacket, 0, 4);
    this->owned = false;
    this->lastBatteryPacket = 0;
    this->lastPacketTime = 0;
    this->speedCallback = &dummySpeedCallback;
    this->armCallback = &dummyArmCallback;
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print('.');
        delay(500);
    }
    Serial.print("Connected! IP address: ");
    Serial.println(WiFi.localIP());
    Serial.printf("Waiting for drone control on port %d\n", DRONEPORT);
    this->udp.begin(DRONEPORT);
}

void DroneNetworkManager::setSpeedCallback(void (*f)(float, float, float)) {
    this->speedCallback = f;
}

void DroneNetworkManager::setArmCallback(void (*f)(float, float)) {
    this->armCallback = f;
}

void DroneNetworkManager::tick() {
    if (this->owned && millis() - this->lastBatteryPacket > BATTERYINTERVAL) {
        sendBatteryState();
        this->lastBatteryPacket = millis();
    }
    int packetSize = this->udp.parsePacket();
    if (packetSize && packetSize < DRONEBUFFERSIZE) {
        if (this->owned) {
            if (this->udp.remoteIP() != this->owner) {
                return;
            }
            this->lastPacketTime = millis();
        }
        this->udp.read(this->buffer, DRONEBUFFERSIZE);
        switch (this->buffer[0])
        {
        case 0:
            sendEcho();
            break;
        case 2:
            if (packetSize != 5) {
                cutOwner();
                break;
            }
            setNewOwner();
            handleSpeedPacket(&(this->buffer[1]));
            break;
        case 3:
            if (packetSize != 3) {
                cutOwner();
                break;
            }
            setNewOwner();
            handleArmPacket(&(this->buffer[1]));
            break;
        default:
            cutOwner();
            break;
        }
    } else {
        if (this->owned && millis() - this->lastPacketTime > OWNERTIMEOUT) {
            cutOwner();
        }
    }

}