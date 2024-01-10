#ifndef _H_WHEELCONTROLLER
#define _H_WHEELCONTROLLER

#include <Arduino.h>


/// @brief Controller Class for a 4WD Mecanum Drone that uses PWM Controlled driverchips like the DRV8212.
class WheelController {
private:
    typedef struct {
        pin_size_t pPin;
        pin_size_t nPin;
    } Wheel;
    Wheel wheels[4];
    float speeds[4];
    void apply();
public:
    
    /// @brief Initializes the 4WD Mecanum Wheel Controller.
    /// @param flPin1 Front Left Pin, that will be pulled HIGH when driving forward
    /// @param flPin2 Front Left Pin, that will be pulled LOW when driving forward
    /// @param frPin1 Front Right Pin, that will be pulled HIGH when driving forward
    /// @param frPin2 Front Right Pin, that will be pulled LOW when driving forward
    /// @param blPin1 Back Left Pin, that will be pulled HIGH when driving forward
    /// @param blPin2 Back Left Pin, that will be pulled LOW when driving forward
    /// @param brPin1 Back Right Pin, that will be pulled HIGH when driving forward
    /// @param brPin2 Back Right Pin, that will be pulled LOW when driving forward
    void init(pin_size_t flPin1, pin_size_t flPin2, pin_size_t frPin1, pin_size_t frPin2, 
        pin_size_t blPin1, pin_size_t blPin2, pin_size_t brPin1, pin_size_t brPin2);

    /// @brief Sets the speed of the drone.
    /// @param frontBias Describes how fast the Drone wants to move forward
    /// @param rightBias Describes how fast the Drone wants to move right
    /// @param clockwiseBias Describes how fast the Drone wants to turn clockwise
    /// @param factor Resulting Speed gets Multiplied by this factor. Values higher than 1 result in undefiened behaviour
    void setSpeed(float frontBias, float rightBias, float clockwiseBias, float factor = 1.0f);
};

#endif