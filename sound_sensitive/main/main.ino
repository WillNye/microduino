#include "userDef.h"
#include <Microduino_ColorLED.h>    //Import the library for the ColorLED.
#include <stdint.h>
#include <Servo.h>

unsigned long debounce_time_touch_pin = 0;

ColorLED matrix = ColorLED(NUMPIXELS, LEDMATRIX_PIN, NEO_GRB + NEO_KHZ800);

uint8_t current_selection = 0;
const uint8_t CURRENT_SELECTION_COUNT = 7;

uint8_t color_matrix[CURRENT_SELECTION_COUNT][3]  = {
  {1, 0, 0},
  {0, 1, 0},
  {0, 0, 1},
  {0, 1, 1},
  {1, 0, 1},
  {1, 1, 0},
  {1, 1, 1}
};

Servo myservo;  // create servo object to control a servo
int servo_pos = 0;    // variable to store the servo position
bool do_increment = true;


void setup() {
  Serial.begin(9600);
  // put your setup code here, to run once:
  pinMode(MIC_PIN, INPUT);
  pinMode(TOUCH_PIN, INPUT);
  myservo.attach(SERVO_PIN);  // attaches the servo on pin SDA to the servo object

  matrix.begin();
  matrix.show();
}

void loop() {
  // put your main code here, to run repeatedly:

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

  //Read in the microphone value
  uint16_t mic_value = analogRead(MIC_PIN);
  int pos_change = 0;

  //Check if the microphone value is under the ambient threshold, set mic value to 0 to ignore if it is under the threshold
  if (mic_value < AMBIENT_THRESHOLD) {
    mic_value = 0;
  }
  //Check if the microphone value is over the max loudness threshold
  //If it is, set the microphone value to that threshold, this sets the max value for the microphone value
  if (mic_value > MAX_THRESHOLD) {
    mic_value = MAX_THRESHOLD;
  }
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

  //Determine the brightness of the leds by using the map function
  uint8_t brightness = map(mic_value, 0, MAX_THRESHOLD, 0, 255);

  //Set, but not show, the color and brightness of all the LEDs in the LED Matrix
  for (uint16_t j = 0; j < NUMPIXELS; ++j) {
    matrix.setPixelColor(j, matrix.Color(color_matrix[current_selection][0] * brightness, color_matrix[current_selection][1] * brightness, color_matrix[current_selection][2] * brightness));
  }

  //Actually execute and show the set values on the LEDs
  matrix.show();

  delay(50);
}
