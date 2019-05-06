#include <Adafruit_NeoPixel.h>
#define NUMPIXELS 450
#define STRIP_PIN    3
Adafruit_NeoPixel pixels(NUMPIXELS, STRIP_PIN, NEO_GRB + NEO_KHZ800);

int led_pin = 13;                // choose the pin for the LED

int rob_pir_pin = 9;               // choose the input pin (for PIR sensor)
int rob_pir_state = LOW;             // we start, assuming no motion detected
int rob_position = 0;
int rob_duration = 5;

int front_pir_pin = 6;               // choose the input pin (for PIR sensor)
int front_pir_state = LOW;             // we start, assuming no motion detected
int front_position = 225;
int front_duration = 2;

const int switch_pin = 7;
const int switch_thresh = 1000;    
int switch_state = LOW;

const int button_pin = A0;
const int button_thresh = 950;    
int button_mode = 0;

uint32_t warm_white = pixels.Color(255, 100, 30);
uint32_t daylight = pixels.Color(200, 200, 200);

////////////////////////////////////////////////////////////

void setup() {
  pinMode(led_pin, OUTPUT);      
  
  pinMode(rob_pir_pin, INPUT);    
  pinMode(front_pir_pin, INPUT);  
  pinMode(switch_pin, INPUT);     
  pinMode(button_pin, INPUT);    
   
  pixels.begin();           // INITIALIZE NeoPixel pixels object (REQUIRED)
  pixels.show();            // Turn OFF all pixels ASAP
  pixels.setBrightness(15); // Set BRIGHTNESS to about 1/5 (max = 255)
  Serial.begin(9600);
}

////////////////////////////////////////////////////////////
 
void loop(){
  // Start anew
  Serial.print("\n");
  Serial.print(millis() % 60000); // Meaningless
  pixels.clear(); pixels.show();

  // Read from button
  uint32_t color_from_mode = read_button();
  Serial.print(" Button mode: ");
  Serial.print(button_mode);
  Serial.print(" | ");

  // Read from switch
  switch_state = read_switch();
  if (switch_state == HIGH){
    flip_the_switch(color_from_mode);
  }

  // Check motion sensor 0
  rob_pir_state = digitalRead(rob_pir_pin);
  if (rob_pir_state == HIGH){
    light_up_and_monitor(rob_pir_pin, rob_position, color_from_mode, rob_duration);
  }

  // Check motion sensor 1
  front_pir_state = digitalRead(front_pir_pin);
  if (front_pir_state == HIGH){
    light_up_and_monitor(front_pir_pin, front_position, color_from_mode, front_duration);
  }
}

////////////////////////////////////////////////////////////

void light_up_and_monitor(int pir_pin, 
                          int pixel_position, 
                          uint32_t color_from_mode,
                          int duration){
  Serial.println("\n\n"); Serial.print(pir_pin); Serial.print(" IS MOVING\n\n");
  light_up_from(pixel_position, color_from_mode);
  int trip_event = millis();
  uint32_t current_color = color_from_mode; // Save current color val before loop
  int now = millis();
  while ((now - trip_event) < (duration * 1000)){
    color_from_mode = read_button();
    if (current_color != color_from_mode){
      return;
      }
    now = millis();
  }
}

////////////////////////////////////////////////////////////

void flip_the_switch(uint32_t color_from_mode){
  
  for (int i = 0; i <= NUMPIXELS; i++) {
    pixels.setPixelColor(i, color_from_mode);
  }
  pixels.show();
  
  Serial.println("\n\nWHOA THERE COWBOY! \nDON'T FORGET TO FLIP THOSE SUCKERS BACK OFF\n");

  uint32_t current_color = color_from_mode; // Save current color val before loop
  while(switch_state == HIGH){
    color_from_mode = read_button();
    if (current_color != color_from_mode){
      return;
    }
    Serial.print("\n");
    switch_state = read_switch();
  }
  pixels.clear(); pixels.show();
//  delay(2000);
  return;
}

////////////////////////////////////////////////////////////

uint32_t read_button() {
  
  int average = 0;
  int readings = 10;
  
  for (int i=0; i < readings; i++) {
    average += digitalRead(button_pin);
  }
  average = (average/readings);
  Serial.print(" | Button average: ");
  Serial.print(average);
  Serial.print(" | ");
  
  if (average == HIGH) {
    button_mode += 1;
    button_mode = button_mode % 2;
    delay(200);
  }

  switch (button_mode) {
    case 0:    // Normal
      return(warm_white);
      break;
    case 1:    // your hand is close to the sensor
      return(daylight);
      break;
  }
}

////////////////////////////////////////////////////////////

int read_switch() {
  
  int average = 0;
  int readings = 10;
  
  for (int i=0; i < readings; i++) {
    average += digitalRead(switch_pin);
  }
  average = average/readings;
  Serial.print("Switch average: ");
  Serial.print(average);
  return average;
}

////////////////////////////////////////////////////////////

void light_up_from(int start_pos, uint32_t color_from_mode){
    for (int i = 0; i <= NUMPIXELS; i+=3) {
      pixels.setPixelColor(start_pos + i, color_from_mode);
      pixels.setPixelColor(start_pos + i+1, color_from_mode);
      pixels.setPixelColor(start_pos + i+2, color_from_mode);
      pixels.setPixelColor(start_pos - i-1, color_from_mode);
      pixels.setPixelColor(start_pos - i-2, color_from_mode);
      pixels.setPixelColor(start_pos - i-3, color_from_mode);
      pixels.show();
    }
}
