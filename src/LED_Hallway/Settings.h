#pragma once
#include <Adafruit_NeoPixel.h>

Adafruit_NeoPixel strip(1, 1, NEO_GRB + NEO_KHZ800);

// Default Color Mode
extern String currentColorMode = "OFF";

// Required Inputs
extern int NUMPIXELS = 1266; // Total number of LEDs in the circuit

// User Options
extern const int MAX_BRIGHTNESS = 100; // 0 to 255
extern const long STAY_ON_SECS = 150;  // N seconds to keep lights on after PIR trip
extern const int JUMP_SIZE = 6;        // How many LEDs light up at a time

// Pin Assignments
extern const int STRIP_PIN = 50;   // Data output to LED strip
extern const int PHOTO_PIN = A8;   // Photoresistor GPIO pin; senses brightness
extern const int IR_PIN = 38;      // IR Remote input
extern const int SPEAKER_PIN = 32; // Piezo speaker out
extern const int SPEAKER_GND = 30; // Piezo speaker ground

// Color Modes
extern const uint32_t TUNG_0 = strip.Color(28, 14, 3);
extern const uint32_t TUNG_1 = strip.Color(215, 95, 15);
extern const uint32_t DAY_0 = strip.Color(50, 50, 60);
extern const uint32_t DAY_1 = strip.Color(167, 215, 198);
extern const int NIGHT_LIGHT_THRESHOLD = 20;