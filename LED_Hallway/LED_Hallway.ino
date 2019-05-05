#include <Adafruit_NeoPixel.h>
#define NUMPIXELS 450
#define STRIP_PIN    3
Adafruit_NeoPixel pixels(NUMPIXELS, STRIP_PIN, NEO_GRB + NEO_KHZ800);

int led_pin = 13;                // choose the pin for the LED

int rob_pir_pin = 9;               // choose the input pin (for PIR sensor)
int rob_pir_state = LOW;             // we start, assuming no motion detected

int front_pir_pin = 6;               // choose the input pin (for PIR sensor)
int front_pir_state = LOW;             // we start, assuming no motion detected

void setup() {
  pinMode(led_pin, OUTPUT);      // declare LED as output
  pinMode(rob_pir_pin, INPUT);     // declare sensor as input
  pinMode(front_pir_pin, INPUT);     // declare sensor as input
  pixels.begin();           // INITIALIZE NeoPixel pixels object (REQUIRED)
  pixels.show();            // Turn OFF all pixels ASAP
  pixels.setBrightness(30); // Set BRIGHTNESS to about 1/5 (max = 255)
  Serial.begin(9600);
}
 
void loop(){
  Serial.println("New Loopski");
  
  rob_pir_state = digitalRead(rob_pir_pin);
  front_pir_state = digitalRead(front_pir_pin);
  
  Serial.println("All quiet on the western front");

  if (rob_pir_state == HIGH){
    Serial.println("ROB IS MOVING");
    light_up(450);
    delay(3000);
    pixels.clear(); pixels.show();
  }

  if (front_pir_state == HIGH){
    Serial.println("INTRUDER ALERT INTRUDER ALERT");
    light_up(200);
    delay(3000);
    pixels.clear(); pixels.show();
  }
  
}


void light_up(int start_pos){
    for (int i = 0; i <= NUMPIXELS; i+=3) {
      pixels.setPixelColor(start_pos + i, pixels.Color(220, 110, 25));
      pixels.setPixelColor(start_pos + i+1, pixels.Color(220, 110, 50));
      pixels.setPixelColor(start_pos + i+2, pixels.Color(220, 110, 50));
      pixels.setPixelColor(start_pos - i-1, pixels.Color(220, 110, 50));
      pixels.setPixelColor(start_pos - i-2, pixels.Color(220, 110, 50));
      pixels.setPixelColor(start_pos - i-3, pixels.Color(220, 110, 50));
      pixels.show();
    }
}
