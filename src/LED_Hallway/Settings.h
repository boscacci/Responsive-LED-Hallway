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

// @ max brite 100:
// for TUNG_1:
// (200, 75, 12): Way more green pls
// (200, 170, 12): Way too green
// (200, 100, 12): Too yellow looking, add blue and red
// (210, 100, 22): Needs more green
// (210, 110, 22): More green!
// (210, 115, 22): MORE
// (210, 120, 22): Feels green...add red? Close.
// (215, 120, 15): Too yellow, sub green
// (215, 115, 15): Down more on green
// (215, 110, 15): Kelvin is good, sub tiny green
// (215, 108, 15): More down green
// (215, 106, 15): reading 2705k, -.38 tint. Add 1 green
// (215, 107, 15): Wrong way, sub 2 green!
// (215, 105, 15): Down one more green...
// (215, 104, 15): Down one more pls.....
// (215, 103, 15): That was huge swing. Go back up one.
// (215, 103, 15): WOOF to camera. Try taking out lots of green for fun
// (215, 94, 15): Weird iPhone image processing anomaly noticed.
// (215, 99, 15): Woof for the eye. Ryan tweaking hue bulbs.
// (215, 95, 15):

// @ max brite 100:
// for DAY:
// (220, 190, 200): Too magenta
// (220, 210, 200): Need even more green, and blue
// (190, 210, 200): Good direction, even less red pls
// (170, 210, 200): Went too far on warmth, still some tint
// (175, 215, 200): More more green pls
// (175, 225, 200): Less red, more blue
// (165, 225, 210): Too blue. no more tint
// (165, 225, 200): Split diff on blue
// (165, 225, 205): Very close. Inch up blue
// (165, 225, 207): Down one point of blue
// (165, 225, 206): Down 2 green?
// (165, 223, 206): Up on red?
// (167, 223, 206): Down 2 blue, down 2 green
// (167, 218, 204): Down 4 green, 2 on blue
// (167, 214, 202): Go up 2 on green
// (167, 216, 202): Down 1 blue
// (167, 216, 201): Why did it get warmer? Down more blue!
// (167, 216, 199): Down 1 green?
// (167, 215, 199): Down 1 blue?
// (167, 215, 198): Down 1 more blue!
// (167, 215, 197): Whoa that was a huge color swing...!