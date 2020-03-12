// Controls the LED's in our entrance hallway

#include <Adafruit_NeoPixel.h>
// #include <Array.h>
#define NUMPIXELS 1500
#define STRIP_PIN 50

int stay_on_dur = 150;
int jump_size = 10;

// Init LED strip object?
Adafruit_NeoPixel pixels(NUMPIXELS, STRIP_PIN, NEO_GRB + NEO_KHZ800);
uint32_t warm_white = pixels.Color(255, 110, 20);
uint32_t daylight = pixels.Color(170, 170, 170);

int all_pir_inputs[4][4] = {
    // Input pin, PIR state, Strip position, hold duration
    {12, 0, 1300, stay_on_dur}, // Ryan/Erin
    {42, 0, 430, stay_on_dur},  // FrontDoor?
    {5, 0, 770, stay_on_dur},   // Kitchen?
    {9, 0, 1110, stay_on_dur}   // Bathroom
};

// Where to plug in photoresistor
const int PHOTO_PIN = A5;

// Where to plug in general on/off switch
const int switch_pin = 30;
int switch_state = 0;

// Where to plug in mode cycle button
const int button_pin = A0;
const int button_thresh = 950; // Sensitivity
int button_mode = 0;

////////////////////////////////////////////////////////////

void setup()
{
  pinMode(all_pir_inputs[0][0], INPUT);
  pinMode(all_pir_inputs[1][0], INPUT);
  pinMode(all_pir_inputs[2][0], INPUT);
  pinMode(all_pir_inputs[3][0], INPUT);
  pinMode(switch_pin, INPUT);
  pinMode(button_pin, INPUT);
  pinMode(PHOTO_PIN, INPUT);
  pixels.setBrightness(240); // Set BRIGHTNESS (max = 255)
  pixels.begin();            // INITIALIZE NeoPixel pixels object (REQUIRED)
  pixels.show();             // Turn OFF all pixels ASAP
  Serial.begin(9600);
}

////////////////////////////////////////////////////////////

float sense_brightness()
{
  float sensorValue = analogRead(PHOTO_PIN);
  float brightness = map(sensorValue, 0, 60, 2, 100);
  return brightness / 100;
}

////////////////////////////////////////////////////////////

void loop()
{
  // Start anew
  Serial.print("\n");
  Serial.print(millis() % 60000);
  pixels.clear();
  pixels.show();

  // Setting LED color modes
  float AMB = sense_brightness();
  Serial.print("\n\n");
  Serial.print(AMB);
  Serial.print("\n\n");
  warm_white = pixels.Color(255 * AMB, 110 * AMB, 20 * AMB);
  daylight = pixels.Color(170 * AMB, 170 * AMB, 170 * AMB);

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
  {
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
  Serial.println("\n\n");
  Serial.print(pir_pin);
  Serial.print(" IS MOVING\n\n");
  light_up_from(pixel_position, color_from_mode);
  long trip_event = millis();
  uint32_t current_color = color_from_mode; // Save current color val before loop
  float now = millis();
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
      return;
    }
    if (read_switch() == HIGH)
    {
      return;
    }
    now = millis();
  }
  pixels.clear();
  pixels.show();
  Serial.print(" Pause for light reset...\n\n");
  delay(2000);
}

////////////////////////////////////////////////////////////

int flip_the_switch(uint32_t color_from_mode)
{

  for (int i = 0; i <= NUMPIXELS; i++)
  {
    pixels.setPixelColor(i, color_from_mode);
  }
  pixels.show();

  Serial.println("\n\nWHOA THERE COWBOY! \nDON'T FORGET TO FLIP THOSE SUCKERS BACK OFF\n");

  uint32_t current_color = color_from_mode; // Save current color val before loop
  while (switch_state == HIGH)
  {
    color_from_mode = read_button();
    if (current_color != color_from_mode)
    {
      return 0;
    }
    Serial.print("\n");
    switch_state = read_switch();
    Serial.print("\n\n");
    Serial.print(analogRead(PHOTO_PIN));
    Serial.print("\n\n");
  }
  pixels.clear();
  pixels.show();
  return 1;
}

////////////////////////////////////////////////////////////

uint32_t read_button()
{

  int average = 0;
  int readings = 10;

  for (int i = 0; i < readings; i++)
  {
    average += digitalRead(button_pin);
  }
  average = (average / readings);

  if (average == HIGH)
  {
    button_mode += 1;
    button_mode = button_mode % 2;
    delay(200);
  }

  switch (button_mode)
  {
  case 0: // Normal
    return (warm_white);
    break;
  case 1: // your hand is close to the sensor
    return (daylight);
    break;
  }
}

////////////////////////////////////////////////////////////

int read_switch()
{

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
