// Controls the LEDs in our entrance hallway
#include <Adafruit_NeoPixel.h>

#define NUMPIXELS 1266    // Total number of LEDs in the strips
#define STRIP_PIN 50      // LED Strip data
#define BRIGHTNESS 120    // out of 255
const int PHOTO_PIN = A8; // Where to plug in photoresistor

int stay_on_dur = 120; // N seconds to keep lights on after PIR trip
int jump_size = 10;    // How many LEDs light up at a time

// Init LED strip object from NeoPixel library
Adafruit_NeoPixel pixels(NUMPIXELS, STRIP_PIN, NEO_GRB + NEO_KHZ800);

// 2 color modes so far
uint32_t warm_white;
uint32_t daylight;

// Preferences relating to PIR sensors:
// {Input pin, PIR state, Strip position, stay-on duration}
int all_pir_inputs[4][4] = {
    {12, 0, 1150, stay_on_dur}, // Ryan/Erin
    {42, 0, 415, stay_on_dur},  // FrontDoor?
    {5, 0, 600, stay_on_dur},   // Kitchen?
    {9, 0, 965, stay_on_dur}    // Bathroom
};

// Where to plug in general on/off switch
const int switch_pin = 30;
int switch_state = 0;

// Where to plug in mode-cycle button
const int button_pin = A0;
const int button_thresh = 950; // Button sensitivity
int button_mode = 0;

////////////////////////////////////////////////////////////

void setup()
{
  // Establish pin modes
  pinMode(all_pir_inputs[0][0], INPUT_PULLUP);
  pinMode(all_pir_inputs[1][0], INPUT_PULLUP);
  pinMode(all_pir_inputs[2][0], INPUT_PULLUP);
  pinMode(all_pir_inputs[3][0], INPUT_PULLUP);
  pinMode(switch_pin, INPUT);
  pinMode(button_pin, INPUT);
  pinMode(PHOTO_PIN, INPUT);

  // Set BRIGHTNESS (max = 255)
  pixels.setBrightness(BRIGHTNESS);

  // INITIALIZE NeoPixel pixels object (REQUIRED)
  pixels.begin();

  // Turn OFF all pixels first
  pixels.show();
  Serial.begin(9600);
}

////////////////////////////////////////////////////////////

float sense_brightness()
// Reads the ambient room brightness from the photoresistor
{
  float sensorValue = analogRead(PHOTO_PIN);
  // Serial.println();
  // Serial.print("Light sensor: ");
  // Serial.println(sensorValue);
  float AMB = map(sensorValue, 0, 60, 1, 100);
  if (AMB > 100)
  {
    AMB = 100;
  }
  return AMB;
}

////////////////////////////////////////////////////////////

void loop()
{
  // Start anew
  Serial.print("\n");
  Serial.print(millis() % 60000);
  pixels.clear();
  pixels.show();

  // Read ambient room brightness
  float AMB = sense_brightness();

  // Adjust diff colors differently based on ambient brightness
  float RED = map(AMB, 1, 100, 0, 245);
  float GREEN = map(AMB, 1, 100, 0, 108);
  float BLUE = map(AMB, 1, 80, 0, 19);
  float mult = (AMB / 100) + .01;
  warm_white = pixels.Color(10 + RED, 2 + GREEN, 1 + BLUE);
  daylight = pixels.Color(170 * mult, 170 * mult, 170 * mult);

  // Read from button
  uint32_t color_from_mode = read_button();
  Serial.print(" Button mode: ");
  Serial.print(button_mode);
  Serial.print(" | ");

  // Read from switch
  switch_state = read_switch();
  int switch_was_flipped_off = 1; // Default is switch flipped off
  if (switch_state == HIGH)
  {
    // But maybe color mode changes while switch is on
    switch_was_flipped_off = flip_the_switch(color_from_mode);
  }

  if (switch_was_flipped_off == 0)
  // If we're emerging from switched-on state, start loop over again
  {
    pixels.clear();
    pixels.show();
    delay(200);
    return 0;
  }

  // Check each PIR sensor:
  for (int i = 0; i < 4; i++)
  {
    int pir_pin = all_pir_inputs[i][0];
    int pir_state = digitalRead(pir_pin);
    int pir_position = all_pir_inputs[i][2];
    int pir_duration = all_pir_inputs[i][3];
    if (pir_state == HIGH)
    {
      // Motion sensor tripped! Fire the LED's!!
      light_up_and_monitor(pir_pin, pir_position, color_from_mode, pir_duration);
    }
  }
}

////////////////////////////////////////////////////////////

void light_up_and_monitor(int pir_pin,
                          int pixel_position,
                          uint32_t color_from_mode,
                          float duration)
{
  // Wrapper function to light up LEDs on motion sensor trip and
  // continue monitoring button/switch state while waiting
  Serial.println("\n\n");
  Serial.print(pir_pin);
  Serial.print(" IS MOVING\n\n");

  // Do the actual lighting up
  light_up_from(pixel_position, color_from_mode);

  // Save the time of PIR activation
  long trip_event = millis();

  // This one will get updated in while loop
  float now = millis();

  // Save current color val before loop, in case it changes
  uint32_t current_color = color_from_mode;

  Serial.println("\n\nTrip Event:");
  Serial.print(trip_event);
  Serial.println("\n\nDuration*1k:");
  Serial.print(duration * 1000);

  while ((now - trip_event) < (duration * 1000))
  {
    Serial.println("\n\nNow: ");
    Serial.print(now);
    Serial.println("\n\n");
    if (current_color != read_button())
    {
      // Break loop if the button is pushed
      return;
    }
    if (read_switch() == HIGH)
    {
      // Break loop if switch is...switched
      return;
    }
    // Save current time and keep monitoring actuators
    now = millis();
  }

  pixels.clear();
  pixels.show();
  Serial.print(" Pause for light reset...\n\n");
  // Give the photoresistor a sec to settle down
  delay(2000);
}

////////////////////////////////////////////////////////////

int flip_the_switch(uint32_t color_from_mode)
{
  // Turn on the lights.
  for (int i = 0; i <= NUMPIXELS; i++)
  {
    pixels.setPixelColor(i, color_from_mode);
  }
  pixels.show();

  Serial.println("WHOA THERE COWBOY! DON'T FORGET TO FLIP THOSE SUCKERS BACK OFF");

  // Save current color val before loop
  uint32_t current_color = color_from_mode;

  // Keep monitoring both actuators until something changes
  while (switch_state == HIGH)
  {
    switch_state = read_switch();
    color_from_mode = read_button();
    if (current_color != color_from_mode)
    {
      // Break main loop and start over if color has changed
      return 0;
    }
  }

  pixels.clear();
  pixels.show();
  // If color has not changed, continue thru rest of main loop
  return 1;
}

////////////////////////////////////////////////////////////

uint32_t read_button()
{
  // Cycles through color modes
  int average = 0;
  int readings = 10;

  for (int i = 0; i < readings; i++)
  {
    average += digitalRead(button_pin);
  }
  // This takes the mean of n readings in case it gets weird
  average = (average / readings);

  if (average == HIGH)
  {
    button_mode += 1;
    button_mode = button_mode % 2; // There are only 2 modes right now
    delay(200);
  }

  switch (button_mode)
  {
  case 0: // "Tungsten"
    return (warm_white);
    break;
  case 1: // 5600k daylight
    return (daylight);
    break;
  }
}

////////////////////////////////////////////////////////////

int read_switch()
{
  // Averages n readings to get a good switch reading
  int average = 0;
  int readings = 10;

  for (int i = 0; i < readings; i++)
  {
    average += digitalRead(switch_pin);
  }
  average = average / readings;
  Serial.print("Switch average: ");
  Serial.print(average);
  return average;
}

////////////////////////////////////////////////////////////

void light_up_from(int start_pos, uint32_t color_from_mode)
{
  // Do some mod addition to light up the whole LED circumference in order
  for (int i = 0; i <= NUMPIXELS; i = i + jump_size)
  {
    for (int j = 0 - jump_size; j <= jump_size; j++)
    {
      pixels.setPixelColor((start_pos + i) % NUMPIXELS + j, color_from_mode);
      pixels.setPixelColor(start_pos - i + j, color_from_mode);
    };
    pixels.show();
  }
  return 1;
}
