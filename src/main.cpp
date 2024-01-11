#include <Arduino.h>

#include "WheelController.hpp"
#include "DroneNetworkManager.hpp"

WheelController wheelController;
DroneNetworkManager wifi;

void setup() {
    Serial.begin(115200);
    wifi.init("NVSpot", "nwex1463");
}

void loop() {
    wifi.tick();
}