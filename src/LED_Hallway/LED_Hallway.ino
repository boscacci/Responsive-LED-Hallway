// Controls the LED's in our entrance hallway

#include <Adafruit_NeoPixel.h>
// #include <Array.h>
#define NUMPIXELS 1500
#define STRIP_PIN 50

int stay_on_dur = 150;
int jump_size = 10;
int BRIGHTNESS = 50; // Out of 255

// Init LED strip object?
Adafruit_NeoPixel pixels(NUMPIXELS, STRIP_PIN, NEO_GRB + NEO_KHZ800);

// Define pins being used for data io:

// choose the pin for the debug blinky LED
int led_pin = 13;

int all_pir_inputs[4][4] = {
  // Input pin, PIR state, Strip position, hold duration
  {12,0,1300,stay_on_dur}, // Ryan/Erin
  {42,0,430,stay_on_dur}, // FrontDoor?
  {5,0,770,stay_on_dur}, // Kitchen?
  {9, 0, 1110,stay_on_dur} // Bathroom
};

// Where to plug in general on/off switch
const int switch_pin = 30;
int switch_state = 0;

// Where to plug in mode cycle button
const int button_pin = A0;
const int button_thresh = 950; // Sensitivity
int button_mode = 0;

// Setting LED color modes
uint32_t warm_white = pixels.Color(255, 110, 20);
uint32_t daylight   = pixels.Color(170, 170, 170);

////////////////////////////////////////////////////////////

void setup() {
  pinMode(led_pin, OUTPUT);      
  
  pinMode(all_pir_inputs[0][0], INPUT);    
  pinMode(all_pir_inputs[1][0], INPUT);  
  pinMode(all_pir_inputs[2][0], INPUT);     
  pinMode(all_pir_inputs[3][0], INPUT);
  pinMode(switch_pin, INPUT);
  pinMode(button_pin, INPUT);
   
  pixels.begin();           // INITIALIZE NeoPixel pixels object (REQUIRED)
  pixels.show();            // Turn OFF all pixels ASAP
  pixels.setBrightness(BRIGHTNESS); // Set BRIGHTNESS (max = 255)
  Serial.begin(9600);
}

////////////////////////////////////////////////////////////
 
void loop(){
  // Start anew
  Serial.print("\n");
  Serial.print(millis() % 60000);
  pixels.clear(); 
  pixels.show();

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

  // Check each PIR sensor:
  for (int i = 0; i < 4; i++) {
    int pir_pin =               all_pir_inputs[i][0];
    int pir_state = digitalRead(pir_pin);
    int pir_position =          all_pir_inputs[i][2];
    int pir_duration =          all_pir_inputs[i][3];
    if (pir_state == HIGH){
      light_up_and_monitor(pir_pin, pir_position, color_from_mode, pir_duration);
    }
  }
}

////////////////////////////////////////////////////////////

void light_up_and_monitor(int pir_pin, 
                          int pixel_position, 
                          uint32_t color_from_mode,
                          float duration){
  Serial.println("\n\n"); Serial.print(pir_pin); Serial.print(" IS MOVING\n\n");
  light_up_from(pixel_position, color_from_mode);
  long trip_event = millis();
  uint32_t current_color = color_from_mode; // Save current color val before loop
  float now = millis();
  Serial.println("\n\nTrip Event:"); Serial.print(trip_event);
  Serial.println("\n\nDuration*1k:"); Serial.print(duration*1000);
  while ((now - trip_event) < (duration * 1000)){
    Serial.println("\n\nNow: "); Serial.print(now);
    Serial.println("\n\n");
    color_from_mode = read_button();
    if (current_color != color_from_mode){
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
    for (int i = 0; i <= NUMPIXELS; i = i+jump_size) {
      for (int j = 0-jump_size; j <= jump_size; j++){
        pixels.setPixelColor((start_pos + i) % NUMPIXELS + j, color_from_mode);
        pixels.setPixelColor(start_pos - i + j, color_from_mode);
      };      
      pixels.show();
    }
    return 1;
}
