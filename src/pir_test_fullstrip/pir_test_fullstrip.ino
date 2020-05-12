#include <Adafruit_NeoPixel.h>

#define KITCHEN 5
#define BATHROOM 9
#define RYANERIN 12
#define FRONTDOOR 42

Adafruit_NeoPixel strip(1266, 50, NEO_GRB + NEO_KHZ800);
uint32_t magenta = strip.Color(50, 0, 50);
uint32_t darkness = strip.Color(0, 0, 0);

void setup()
{
  pinMode(KITCHEN, INPUT_PULLUP);
  pinMode(BATHROOM, INPUT_PULLUP);
  pinMode(RYANERIN, INPUT_PULLUP);
  pinMode(FRONTDOOR, INPUT_PULLUP);
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  Serial.begin(9600);
}

void loop()
{
  // BATHROOM //
  if (digitalRead(BATHROOM))
  {
    strip.fill(magenta, 925, 100);
    strip.show();
    Serial.print("BATHROOM: 1");
  }
  else
  {
    strip.fill(darkness, 925, 100);
    strip.show();
    Serial.print("BATHROOM: 0");
  }

  // FRONTDOOR //
  if (digitalRead(FRONTDOOR))
  {
    strip.fill(magenta, 300, 100);
    strip.show();
    Serial.print(" Frontdoor: 1");
  }
  else
  {
    strip.fill(darkness, 300, 100);
    strip.show();
    Serial.print(" Frontdoor: 0");
  }

  // RYANERIN
  if (digitalRead(RYANERIN))
  {
    strip.fill(magenta, 1100, 100);
    strip.show();
    Serial.println(" Ryanerin: 1");
  }
  else
  {
    strip.fill(darkness, 1100, 100);
    strip.show();
    Serial.println(" Ryanerin: 0");
  }
}