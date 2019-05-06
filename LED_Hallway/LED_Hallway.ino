#include <Adafruit_NeoPixel.h>
#define NUMPIXELS 450
#define STRIP_PIN    3
Adafruit_NeoPixel pixels(NUMPIXELS, STRIP_PIN, NEO_GRB + NEO_KHZ800);

int led_pin = 13;                // choose the pin for the LED

int rob_pir_pin = 9;               // choose the input pin (for PIR sensor)
int rob_pir_state = LOW;             // we start, assuming no motion detected
int rob_position = 450;

int front_pir_pin = 6;               // choose the input pin (for PIR sensor)
int front_pir_state = LOW;             // we start, assuming no motion detected

const int switch_pin = 7;
const int switch_thresh = 865;    
int switch_state = LOW;

const int button_pin = A0;
const int button_thresh = 950;    
int button_mode = 0;

uint32_t warm_white = pixels.Color(220, 110, 50);
uint32_t daylight = pixels.Color(80, 100, 100);

////////////////////////////////////////////////////////////

void setup() {
  pinMode(led_pin, OUTPUT);      
  
  pinMode(rob_pir_pin, INPUT);    
  pinMode(front_pir_pin, INPUT);  
  pinMode(switch_pin, INPUT);     
  pinMode(button_pin, INPUT);    
   
  pixels.begin();           // INITIALIZE NeoPixel pixels object (REQUIRED)
  pixels.show();            // Turn OFF all pixels ASAP
  pixels.setBrightness(30); // Set BRIGHTNESS to about 1/5 (max = 255)
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
  read_switch();
  if (switch_state == HIGH){
    flip_the_switch(color_from_mode);
  }

  // Check motion sensors
  rob_pir_state = digitalRead(rob_pir_pin);
  if (rob_pir_state == HIGH){
    Serial.println("\n\nROB IS MOVING\n\n");
    light_up_from(rob_position, color_from_mode);
    delay(3000);
  }

  // Check motion sensors
  front_pir_state = digitalRead(front_pir_pin);
  if (front_pir_state == HIGH){
    Serial.println("INTRUDER ALERT INTRUDER ALERT");
    light_up_from(200, color_from_mode);
    delay(3000);
  }
}

////////////////////////////////////////////////////////////

uint32_t read_button() {
  
  int average = 0;
  int readings = 10;
  
  for (int i=0; i < readings; i++) {
    average += analogRead(button_pin);
  }
  average = (average/readings);
  Serial.print(" | Button average: ");
  Serial.print(average);
  Serial.print(" | ");
  
  if (average > button_thresh) {
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
  int readings = 15;
  
  for (int i=0; i < readings; i++) {
    average += digitalRead(switch_pin);
  }
  average = average/readings;
  Serial.print("Switch average: ");
  Serial.print(average);
  if (average < switch_thresh) {
    switch_state = LOW;
      return 0;
  }
  else {
    switch_state = HIGH;
    return 1;
  }
  
}

////////////////////////////////////////////////////////////

void flip_the_switch(int color_from_mode){
  
  for (int i = 0; i <= NUMPIXELS; i++) {
    pixels.setPixelColor(i, color_from_mode);
  }
  pixels.show();
  Serial.println("\nWHOA THERE COWBOY \nDON'T FORGET TO FLIP THOSE SUCKERS BACK OFF\n");
    
  // Keep lights on until switch is off
    
  int current_mode = button_mode;
  while(switch_state == HIGH){
    read_switch();
  }
  pixels.clear(); pixels.show();
  delay(2000);
  return;
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
