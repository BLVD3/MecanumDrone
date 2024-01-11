#include "WheelController.hpp"
#include <cstdlib>

void WheelController::init(pin_size_t flPin1, pin_size_t flPin2, pin_size_t frPin1, pin_size_t frPin2, pin_size_t blPin1, pin_size_t blPin2, pin_size_t brPin1, pin_size_t brPin2) {
    this->wheels[0].pPin = flPin1;
    this->wheels[0].nPin = flPin2;
    this->wheels[1].pPin = frPin1;
    this->wheels[1].nPin = frPin2;
    this->wheels[2].pPin = blPin1;
    this->wheels[2].nPin = blPin2;
    this->wheels[3].nPin = brPin2;
    this->wheels[3].pPin = brPin1;
    pinMode(flPin1, OUTPUT);
    pinMode(flPin2, OUTPUT);
    pinMode(frPin1, OUTPUT);
    pinMode(frPin2, OUTPUT);
    pinMode(blPin1, OUTPUT);
    pinMode(blPin2, OUTPUT);
    pinMode(brPin2, OUTPUT);
    pinMode(brPin1, OUTPUT);
    setSpeed(0.f, 0.f, 0.f);
}

void WheelController::setSpeed(float frontBias, float rightBias, float clockwiseBias, float factor) {
    float max = 0.f;
    for (byte i = 0; i < 4; i++) {
        speeds[i] = frontBias;
        speeds[i] += rightBias * ((i & 1) ^ ((i >> 1) & 1) ? -1.f : 1.f);
        speeds[i] += clockwiseBias * (i & 1 ? -1.f : 1.f);
        if (max < abs(speeds[i])) {
            max = abs(speeds[i]);
        }
    }
    if (max > 1.f) {
        for (byte i = 0; i < 4; i++)
        {
            speeds[i] /= max;
        }
    }
    for (byte i = 0; i < 4; i++) {
        speeds[i] * factor;
    }
    apply();
}

void WheelController::apply()
{
    for (byte i = 0; i < 4; i++) {
        if (speeds[i] == 0.f) {
            digitalWrite(wheels[i].pPin, HIGH);
            digitalWrite(wheels[i].nPin, HIGH);
        } else if (speeds[i] > 0) {
            digitalWrite(wheels[i].nPin, LOW);
            analogWrite(wheels[i].pPin, abs(speeds[i]) * 255);
        } else {
            digitalWrite(wheels[i].pPin, LOW);
            analogWrite(wheels[i].nPin, abs(speeds[i]) * 255);
        }
    }
}
