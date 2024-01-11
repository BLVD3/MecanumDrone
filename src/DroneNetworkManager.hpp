#ifndef _H_DRONENETWORKMANAGER
#define _H_DRONENETWORKMANAGER

#include <WiFi.h>
#include <WiFiUdp.h>

#define DRONEPORT 25560
#define DRONEBUFFERSIZE 16
#define OWNERTIMEOUT 500
#define BATTERYINTERVAL 500

/// @brief Manager for WifiPackets of the Mecanum Drone
class DroneNetworkManager
{
private:
    WiFiUDP udp;
    byte buffer[512];
    bool owned;
    IPAddress owner;
    uint16_t ownerPort;
    long lastPacketTime;

    long lastBatteryPacket;

    void (*speedCallback)(float, float, float);
    byte lastSpeedPacket[4];

    void (*armCallback)(float, float);
    byte lastArmPacket[2];

    void sendEcho();
    void sendBatteryState();
    void handleSpeedPacket(byte *packet);
    void handleArmPacket(byte *packet);
    void cutOwner();
    void setNewOwner();
public:
    /// @brief Initializes the NetworkManager
    /// @param ssid 
    /// @param password 
    void init(char* ssid, char* password);

    /// @brief Sets the function that will be called after a Speed Packet arrived.
    /// @param speedCallback function will be called with frontbias, rightbias and clockwisebias if changed
    void setSpeedCallback(void (*speedCallback)(float, float, float));
    /// @brief Sets the function that will be called after a Arm Packet arrived.
    /// @param armCallback function will be called with the vertical position and claw position.
    void setArmCallback(void (*armCallback)(float, float));
    void tick();
};



#endif