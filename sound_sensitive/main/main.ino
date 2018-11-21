#include "userDef.h"
#include <Adafruit_NeoPixel.h>
#include <Microduino_ColorLED.h>    //Import the library for the ColorLED.
#include <stdint.h>
#include <Servo.h>

// Touch sensor variables
unsigned long debounce_time_touch_pin = 0;
uint8_t current_selection = 0;
const uint8_t CURRENT_SELECTION_COUNT = 7;

// LED Matrix variables
Adafruit_NeoPixel matrix = Adafruit_NeoPixel(NUMPIXELS, LEDMATRIX_PIN, NEO_GRB + NEO_KHZ800);
float brightness = 1;
int primary_color_pos = rand() % 3;  // Used to set primary color in array of len 3 where 0 = R, 1 = G, 2 = B (RGB)
int primary_color = rand() % 55 + 200;
int secondary_colors = rand() % 81;
int color_sets [2][3];
int iter_delay = 10; // Number of loops to show lights after noise has stopped
int color_aggregate_mic = 0; // Used to specify when to change colors

// Servo variables
Servo myservo;  // create servo object to control a servo
int servo_pos = 0;    // variable to store the servo position
bool do_increment = true;


void setup() {
  Serial.begin(9600);
  // put your setup code here, to run once:
  pinMode(MIC_PIN, INPUT);
  // pinMode(TOUCH_PIN, INPUT);
  // myservo.attach(SERVO_PIN);  // attaches the servo on pin SDA to the servo object

  matrix.begin();
  matrix.show();
}


void set_colors(uint16_t mic_value) {
  if (mic_value > 0) {
    int adjusted_mic_value = int(mic_value/100) + 1;
    brightness = map(mic_value, 0, MAX_THRESHOLD, 0, 100);
    color_aggregate_mic = color_aggregate_mic + adjusted_mic_value;

    if (color_aggregate_mic > 400) {
      color_aggregate_mic = 0;
      primary_color_pos = rand() % 3;
      secondary_colors = rand() % 81;
      primary_color = rand() % 55 + 200;
    }
  }

  for (uint16_t color_set = 0; color_set < 2; ++color_set) {
    for (uint16_t rgb_pos = 0; rgb_pos < 2; ++rgb_pos) {
      color_sets[color_set][rgb_pos] = secondary_colors * brightness * (primary_color/secondary_colors);
    }
    color_sets[color_set][primary_color_pos] = primary_color * brightness;
  }

  //Set, but not show, the color and brightness of all the LEDs in the LED Matrix
  for (uint16_t j = 0; j < NUMPIXELS; ++j) {
    int selected_colors = rand() % 2;
    matrix.setPixelColor(j,
      matrix.Color(color_sets[selected_colors][0],
                   color_sets[selected_colors][1],
                   color_sets[selected_colors][2]));
  }

  //Actually execute and show the set values on the LEDs
  matrix.show();
}


void set_servo(uint16_t mic_value) {
  int pos_change = 0;

  if (mic_value > 0) {
    pos_change = int(mic_value/100) + 1;
    if (do_increment) {
      if ((servo_pos + pos_change) >= 179) {
        servo_pos = 179;
        do_increment = false;
      } else {
        servo_pos = servo_pos + pos_change;
      }
    } else {
      if ((servo_pos - pos_change) <= 1) {
        servo_pos = 1;
        do_increment = true;
      } else {
        servo_pos = servo_pos - pos_change;
      }
    }
    myservo.write(servo_pos);              // tell servo to go to position in variable 'pos'
  }
}


void set_digitalRead() {
  //Check if touch button is pressed.
  if ( !digitalRead(TOUCH_PIN )
       &&
       ((millis() - debounce_time_touch_pin ) > DEBOUNCE_TIME_MILLISECONDS)
     ) {
    debounce_time_touch_pin = millis();

    //Change the current selection color by incrementing the value
    if (++current_selection >= CURRENT_SELECTION_COUNT) {
      current_selection = 0;
    }
  }
}


void loop() {
  uint16_t mic_value = analogRead(MIC_PIN);

  if (mic_value < AMBIENT_THRESHOLD) {
    mic_value = 0;
  }
  else if (mic_value > MAX_THRESHOLD) {
    mic_value = MAX_THRESHOLD;
  }

  // set_servo(mic_value);
  set_colors(mic_value);

  delay(50);
}


