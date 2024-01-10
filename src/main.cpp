#include <Arduino.h>

#include "WheelController.hpp"

WheelController wheelController;

void setup() {
    wheelController.init(8, 9, 10, 11, 12, 13, 14, 15);
}

void loop() {
    
}