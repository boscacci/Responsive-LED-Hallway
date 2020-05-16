// Controls the LEDs in our entrance hallway
#include <Adafruit_NeoPixel.h>
#include "MotionSensor.h"
#include "Settings.h"

// Default Color Mode
String currentColorMode = "OFF";

// Init Motion Sensor Objects
MotionSensor RyanErinSensor(1150, 12);
MotionSensor FrontDoorSensor(415, 42);
MotionSensor KitchenSensor(600, 5);
MotionSensor BathroomSensor(965, 9);

// List of all motion sensors
MotionSensor allMotionSensors[] = {
    RyanErinSensor,
    FrontDoorSensor,
    KitchenSensor,
    BathroomSensor,
};

// Init LED strip object from NeoPixel library
Adafruit_NeoPixel pixels(NUMPIXELS, STRIP_PIN, NEO_GRB + NEO_KHZ800);

void setup()
{
  // More input pin settings
  pinMode(PHOTO_PIN, INPUT);

  // Set brightness cap
  pixels.setBrightness(MAX_BRIGHTNESS);

  pixels.begin();
  pixels.clear();
  pixels.show();

  // Start Serial Monitor
  Serial.begin(9600);
}

void loop()
{

  String currentColorMode = readRemoteIR(currentColorMode);

  if (currentColorMode == "OFF")
  {
    Serial.println("Color mode is OFF.");
    pixels.clear();
    pixels.show();
    return;
  }

  if (currentColorMode == "RAINBOW")
  {
    Serial.println("Rainbow mode lezgooo.");
    return;
  }

  if (currentColorMode == "TUNG_ON")
  {
    Serial.println("TUNG ON!");
    pixels.fill(TUNGSTEN, 0, NUMPIXELS);
    pixels.show();
    return;
  }

  if (currentColorMode == "DAY_ON")
  {
    pixels.fill(DAYLIGHT_1, 0, NUMPIXELS);
    pixels.show();
    delay(100);
    return;
  }

  Serial.print("Monitoring Motion Sensors. CurrentColorMode: ");
  Serial.println(currentColorMode);

  // Check each PIR sensor:
  for (int i = 0; i < sizeof(allMotionSensors) / sizeof(allMotionSensors[0]); i++)
  {
    int pir_state = digitalRead((allMotionSensors[i]).pinAssignment);
    if (pir_state == HIGH)
    {
      // Motion sensor tripped! Fire the LED's!!
      light_up_and_monitor(allMotionSensors[i], currentColorMode);
    }
  }
}

String readRemoteIR(String currentColorMode)
{
  // if (no signal from remote){
  //   return currentColorMode;
  // }

  // !TODO This should actually read from IR remote:
  // Right now it just returns an arbitrary value
  String newColorMode = "TUNG_MOTION";

  return newColorMode;
}

void light_up_and_monitor(MotionSensor thisPir, String thisColor)
{
  Serial.print("PIR #");
  Serial.print(thisPir.pinAssignment);
  Serial.print(" detected motion!\n\n");

  int ambienceLevel = readRoomAmbience();

  // Do the actual lighting up
  if (thisColor == "TUNG_MOTION")
  {
    light_up_from(thisPir.position, TUNGSTEN);
  }

  if (thisColor == "DAY_MOTION")
  {
    if (ambienceLevel < 33)
    {
      Serial.print("Brightness level: ");
      Serial.println("< 33");
      light_up_from(thisPir.position, DAYLIGHT_0);
    }
    else
    {
      Serial.print("Brightness level: ");
      Serial.println(">= 33");
      light_up_from(thisPir.position, DAYLIGHT_1);
    }
  }

  // Save time at which LEDs are fully lit
  long trip_event = millis();

  // This one will get updated in upcoming while loop
  long now = millis();

  while ((now - trip_event) < (STAY_ON_SECS * 1000))
  {
    Serial.println();
    Serial.print("Now: ");
    Serial.println(now);

    // Break loop if the button is pushed
    if (thisColor != readRemoteIR(thisColor))
    {
      return;
    }

    // Save current time and keep monitoring remote
    now = millis();
  }

  // Stay-on dur has expired, turn off!
  pixels.clear();
  pixels.show();
  Serial.println("Wait a tic for photoresistor to read...");
  delay(1500);
}

void light_up_from(int start_pos, uint32_t color_from_mode)
{
  // Do some mod addition to light up the whole LED circumference in order
  for (int i = 0; i <= NUMPIXELS; i = i + JUMP_SIZE)
  {
    for (int j = 0 - JUMP_SIZE; j <= JUMP_SIZE; j++)
    {
      pixels.setPixelColor((start_pos + i) % NUMPIXELS + j, color_from_mode);
      pixels.setPixelColor(start_pos - i + j, color_from_mode);
    };
    pixels.show();
  }
  return 1;
}

float readRoomAmbience()
// Reads ambient room brightness from photoresistor
{
  float sensorValue = analogRead(PHOTO_PIN);
  // Serial.println();
  // Serial.print("Light sensor: ");
  // Serial.println(sensorValue);
  float ambienceLevel = map(sensorValue, 0, 60, 1, 100);
  if (ambienceLevel > 100)
  {
    ambienceLevel = 100;
  }
  return ambienceLevel;
}