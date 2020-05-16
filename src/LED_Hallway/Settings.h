#pragma once
#include <Adafruit_NeoPixel.h>

Adafruit_NeoPixel strip(1, 1, NEO_GRB + NEO_KHZ800);

// Required Inputs
extern int NUMPIXELS = 1266; // Total number of LEDs in the circuit

// User Options
extern const int MAX_BRIGHTNESS = 120; // 0 to 255
extern const long STAY_ON_SECS = 150;  // N seconds to keep lights on after PIR trip
extern const int JUMP_SIZE = 10;       // How many LEDs light up at a time

// Pin Assignments
extern const int STRIP_PIN = 50; // Data output to LED strip
extern const int PHOTO_PIN = A8; // Photoresistor GPIO pin; senses brightness

// Color Modes
extern const uint32_t TUNGSTEN = strip.Color(150, 20, 20);
extern const uint32_t DAYLIGHT_0 = strip.Color(50, 50, 60);
extern const uint32_t DAYLIGHT_1 = strip.Color(145, 145, 155);
