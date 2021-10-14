/*
 * Name: Michael Ly
 * Date: September 17, 2021
 * Description: Turns LED color blue ON for 2 seconds
 * and OFF for 4 seconds. Then turns LED color red ON
 * for 3 seconds and OFF for 2 seconds
 * Target: ESP32 Dev Module
 */

#include "main.h"
#include <Arduino.h>

void setup() {                                  // Configure LEDs as output
  pinMode(LED_B, OUTPUT) ;
  pinMode(LED_R, OUTPUT) ;
}

/* MAIN */ 
void loop() {
    blinkBlueLED() ;                            // Turn blue LED on and off
    blinkRedLED() ;                             // Turn red LED on and off
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* Function Definitions */

void blinkBlueLED(void) {                       // This function turns the blue LED ON
  blueLED_state = LED_ON ;                      // for 2 seconds and then OFF for 4
  digitalWrite(LED_B, LED_ON) ;                 // seconds.
  delay(DELAY_ON_B) ;
  blueLED_state = LED_OFF ;
  digitalWrite(LED_B, blueLED_state) ;
  delay(DELAY_OFF_B) ;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void blinkRedLED(void) {                        // This function turns the red LED ON
  redLED_state = LED_ON ;                       // for 3 seconds and OFF for 2 seconds
  digitalWrite(LED_R, redLED_state) ;
  delay(DELAY_ON_R) ;
  redLED_state = LED_OFF ;
  digitalWrite(LED_R, redLED_state) ;
  delay(DELAY_OFF_R) ;
}