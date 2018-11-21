#pragma once

//#define DEBUG                   //Uncomment to debug

#define LEDMATRIX_PIN   A0        //Defines the pin that connects the LED Matrix

#define NUMPIXELS       6         //Defines the number of pixels on the LED Matrix

#define MIC_PIN         A2        //Defines the pin that connects to the Microphone

#define TOUCH_PIN       6         //Defines the pin that connects the Touch Button

#define SERVO_PIN       SDA         //Defines the pin that connects the Servo

const uint16_t AMBIENT_THRESHOLD = 40;    //Determines the noise loudness values to ignore and sets anything under it to 0

const uint16_t MAX_THRESHOLD = 800;       //Sets the max loudness value. Used for better LED output scaling.

const unsigned long DEBOUNCE_TIME_MILLISECONDS = 200;   //Sets the time to hold down the touch button to register as a "press"

const bool RUN_SERVO = false;
const bool RUN_LED = true;
const bool RUN_TOUCH_SENSOR = false;
