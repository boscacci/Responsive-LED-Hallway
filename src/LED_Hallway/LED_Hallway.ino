// 3rd Party Libs
// #include <Adafruit_NeoPixel.h>
#include <IRremote.h>

// Custom headers
#include "MotionSensor.h"
#include "Settings.h"

IRrecv irrecv(IR_PIN); // Make pin SIG for IR diode
decode_results results;

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

  pinMode(SPEAKER_PIN, OUTPUT);
  pinMode(SPEAKER_GND, OUTPUT);
  digitalWrite(SPEAKER_PIN, LOW);
  digitalWrite(SPEAKER_GND, LOW);

  // For IR remote sensor
  irrecv.enableIRIn();

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
  delay(500);

  currentColorMode = readRemoteIR(currentColorMode);

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
    taste_the_rainbow();
    return;
  }

  int ambienceLevel = readRoomAmbience();

  if (currentColorMode == "TUNG_ON")
  {
    if (ambienceLevel < 33)
    {
      Serial.println("Tung on dim!");
      pixels.fill(TUNG_0, 0, NUMPIXELS);
      pixels.show();
      while (currentColorMode == "TUNG_ON")
      {
        currentColorMode = readRemoteIR("TUNG_ON");
        delay(500);
      }
      pixels.clear();
      pixels.show();
      delay(200);
      return;
    }
    else
    {
      Serial.println("Tung on bright!");
      pixels.fill(TUNG_1, 0, NUMPIXELS);
      pixels.show();
      while (currentColorMode == "TUNG_ON")
      {
        currentColorMode = readRemoteIR("TUNG_ON");
        delay(500);
      }
      pixels.clear();
      pixels.show();
      delay(200);
      return;
    }
  }

  if (currentColorMode == "DAY_ON")
  {
    if (ambienceLevel < 33)
    {
      Serial.println("5600k on dim!");
      pixels.fill(DAY_0, 0, NUMPIXELS);
      pixels.show();
      while (currentColorMode == "DAY_ON")
      {
        currentColorMode = readRemoteIR("DAY_ON");
        delay(500);
      }
      pixels.clear();
      pixels.show();
      delay(200);
      return;
    }
    else
    {
      Serial.println("5600k on bright!");
      pixels.fill(DAY_1, 0, NUMPIXELS);
      pixels.show();
      while (currentColorMode == "DAY_ON")
      {
        currentColorMode = readRemoteIR("DAY_ON");
        delay(500);
      }
      pixels.clear();
      pixels.show();
      delay(200);
      return;
    }
  }

  pixels.clear();
  pixels.show();

  Serial.print("Monitoring Motion Sensors. CurrentColorMode: ");
  Serial.println(currentColorMode);

  // Check each PIR sensor:
  for (int i = 0; i < sizeof(allMotionSensors) / sizeof(allMotionSensors[0]); i++)
  {
    int pir_state = digitalRead((allMotionSensors[i]).pinAssignment);
    if (pir_state == HIGH)
    {
      // Check again for safety:
      if (digitalRead((allMotionSensors[i]).pinAssignment))
      {
        // Motion sensor tripped! Fire the LED's!!
        light_up_and_monitor(allMotionSensors[i], currentColorMode);
      }
    }
  }
}

String hexDecoder(String hexToDecode)
{
  if (hexToDecode == "e318261b") // 1 button
  {
    Serial.println("Motion-sensing tungsten activated.");
    return "TUNG_MOTION";
  }
  if (hexToDecode == "511dbb") // 2 button
  {
    Serial.println("Motion-sensing 5600k activated.");
    return "DAY_MOTION";
  }
  if (hexToDecode == "52a3d41f") // 4 button
  {
    Serial.println("Always-on tungsten activated.");
    return "TUNG_ON";
  }
  if (hexToDecode == "d7e84b1b") // 5 button
  {
    Serial.println("Always-on 5600k activated.");
    return "DAY_ON";
  }
  if (hexToDecode == "f076c13b") // 7 button
  {
    Serial.println("Trippy mode activated.");
    return "RAINBOW";
  }
  if (hexToDecode == "97483bfb") // zero button
  {
    Serial.println("Always-off mode activated.");
    return "OFF";
  }
  Serial.print("hexDecoder received invalid value: ");
  Serial.println(hexToDecode);
  return "ERROR";
}

void taste_the_rainbow()
{
  for (long firstPixelHue = 0; firstPixelHue < 5 * 65536; firstPixelHue += 256)
  {
    for (int i = 0; i < pixels.numPixels(); i++)
    {
      int pixelHue = firstPixelHue + (i * 65536L / pixels.numPixels());
      pixels.setPixelColor(i, pixels.gamma32(pixels.ColorHSV(pixelHue)));
    }
    pixels.show();
    currentColorMode = readRemoteIR(currentColorMode);
    if (currentColorMode != "RAINBOW")
    {
      pixels.clear();
      pixels.show();
      return;
    }
    delay(10);
  }
}

String readRemoteIR(String currentColorMode)
{
  String resultString = "";

  // If we get a signal,
  if (irrecv.decode(&results))
  {
    // Save it for decoding
    resultString = String(results.value, HEX);
    Serial.print("Result string from readRemoteIR: ");
    Serial.println(resultString);
    irrecv.resume();
  }
  else
  {
    // If we don't get a signal, return old mode
    return currentColorMode;
  }

  // If we got a signal, decode it:
  String newColorMode = hexDecoder(resultString);

  // If it doesn't decode well,
  if (newColorMode == "ERROR")
  {
    // Give back the input value
    return currentColorMode;
  }
  // Otherwise, send updated color mode
  tone(SPEAKER_PIN, 1000, 100);
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
    if (ambienceLevel < 33)
    {
      Serial.print("Brightness level: ");
      Serial.println("< 33");
      light_up_from(thisPir.position, TUNG_0);
    }
    else
    {
      Serial.print("Brightness level: ");
      Serial.println(">= 33");
      light_up_from(thisPir.position, TUNG_1);
    }
  }

  if (thisColor == "DAY_MOTION")
  {
    if (ambienceLevel < 33)
    {
      Serial.print("Brightness level: ");
      Serial.println("< 33");
      light_up_from(thisPir.position, DAY_0);
    }
    else
    {
      Serial.print("Brightness level: ");
      Serial.println(">= 33");
      light_up_from(thisPir.position, DAY_1);
    }
  }

  // Save time at which LEDs are fully lit
  long trip_event = millis();

  // This one will get updated in upcoming while loop
  long now = millis();

  while ((now - trip_event) < (STAY_ON_SECS * 1000))
  {
    Serial.println();
    Serial.print("Seconds remaining: ");
    Serial.println(((STAY_ON_SECS * 1000) - (now - trip_event)) / 1000);

    // Break loop if the button is pushed
    currentColorMode = readRemoteIR(thisColor);
    if (thisColor != currentColorMode)
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