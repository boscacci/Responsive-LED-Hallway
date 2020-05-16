#pragma once

class MotionSensor
{
public: // Public members:
    MotionSensor(int position_, int pinAssignment_);
    int position;
    int pinAssignment;
};

MotionSensor::MotionSensor(int position_, int pinAssignment_)
{
    position = position_;
    pinAssignment = pinAssignment_;
    pinMode(pinAssignment_, INPUT_PULLUP);
}