#if 1
/*
* Name: Michael Ly
* Date: September 26, 2021
* Description: Uses an ISR to put the board in one of three modes.
*              The device boots up into mode 1: the off state (no LEDs lit).
*              On the depression of a switch on the dev board, the first LED
*              color should light blue and be steady.
*              On the depression of a switch a second time, the LED color should
*              flash with an on time of 50ms and total cycle time of 1 second.
*              The LED should remain flashing until the switch is depressed
*              and the system returns to the off state of the state transitions.
* Target: Espressif ESP32 dev board, TI MSP432401R
*/

/* Libraries */
#include "main.h"
#include <Arduino.h>
#include <stdint.h>

#if 1
    #define ESP32        (1)
#else
    #define MSP432401R   (1)
#endif


/* Setup */
void setup() {
    Serial.begin(115200) ;
    pinMode(LED2_B, OUTPUT) ;
    pinMode(LED1_B, OUTPUT) ;
    pinMode(LED1_G, OUTPUT) ;
    pinMode(LED1_R, OUTPUT) ;
    pinMode(BUTTON1, INPUT_PULLUP) ;

    attachInterrupt(digitalPinToInterrupt(BUTTON1), ISR_buttonPressed, FALLING) ;

    LED_init() ;                                                                            // Set all LEDs to OFF position
    buttonCount             = 0 ;
    previousMillis_Btn      = 0 ;
    previousMillis_Blink1   = 0 ;
    previousMillis_Blink2   = 0 ;   
}

/* Main */
void loop() {
    currentMillis = millis() ;                                                              // Get current time

    if ( !(buttonCount % 3) ) {                                                             // Reset LEDs to OFF state
        LED_init() ;
        buttonCount = 0 ;
    }
    else if ( !(buttonCount % 2) ) {                                                        // Flash red LED ON for 50ms with 1s cycle
        change_to_state2(currentMillis) ;
    }
    else {
        change_to_state1() ;                                                                // Turn blue LED ON and keep at steady state
    }

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* Function Definitions */

void LED_init(void) {                                                                       // This function turns all LEDs OFF
    blueLED2_state  = LED_OFF ;
    blueLED1_state  = LED_OFF ;
    greenLED_state  = LED_OFF ;
    redLED_state    = LED_OFF ;

    digitalWrite(LED2_B, blueLED2_state) ;
    digitalWrite(LED1_B, blueLED1_state) ;
    digitalWrite(LED1_G, greenLED_state) ;
    digitalWrite(LED1_R, redLED_state) ;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void change_to_state1(void) {                                                               // on every (3n+1) button press, turn the blue
    blueLED1_state  = LED_ON ;                                                              // LED to an ON steady state
    redLED_state    = LED_OFF ;
    
    digitalWrite(LED1_B, blueLED1_state) ;
    digitalWrite(LED1_R, redLED_state) ;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void change_to_state2(uint32_t currentMillis) {                                             // On every (3n+2) button press, have the blue LED
    blueLED1_state  = LED_OFF ;                                                             // in the OFF position, but flash on the red LED
    digitalWrite(LED1_B, blueLED1_state) ;                                                  // for 50ms with total cycle time of 1s.

    if ( (currentMillis - previousMillis_Blink1) >= BLINK1_DELAY ) {                        // Turn red LED ON for 50ms
        redLED_state = LED_OFF ;
        digitalWrite(LED1_R, redLED_state) ;
        previousMillis_Blink1 += BLINK1_DELAY ;
    }
    if ( (currentMillis - previousMillis_Blink2) >= BLINK2_DELAY ) {                        // Turn red LED OFF for 950ms
        redLED_state = LED_ON ;
        digitalWrite(LED1_R, redLED_state) ;
        previousMillis_Blink2 += BLINK2_DELAY ;
    }
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* Button ISR Handler */
#ifndef MSP432401R
    void IRAM_ATTR ISR_buttonPressed(void) {                                                // This function detects a button press
        if ( (currentMillis - previousMillis_Btn) >= (BUTTON_DEBOUNCE) ) {                  // and then debounces the button with the
            previousMillis_Btn += BUTTON_DEBOUNCE ;                                         // millis nonblocking method. It then
            buttonCount++ ;                                                                 // increments the button counter.
            
            Serial.printf("Button has been pressed %u times\n", buttonCount);
        }
    }
#else
    void ISR_buttonPressed(void) {
        if ( (currentMillis - previousMillis_Btn) >= (BUTTON_DEBOUNCE) ) {
            previousMillis_Btn += BUTTON_DEBOUNCE ;
            buttonCount++ ;
            
            Serial.print("Button has been pressed ");
            Serial.print(buttonCount) ;
            Serial.println(" times") ;
        }
    }
#endif






#else
/*
 * Name: Michael Ly
 * Date: September 26, 2021
 * Description: Uses an ISR to put the board in one of three modes.
 *              The device boots up into mode 1: the off state (no LEDs lit).
 *              On the depression of a switch on the dev board, the first LED
 *              color should light blue and be steady.
 *              On the depression of a switch a second time, the LED color should
 *              flash with an on time of 50ms and total cycle time of 1 second.
 *              The LED should remain flashing until the switch is depressed
 *              and the system returns to the off state of the state transitions.
 * Target: Espressif ESP32 dev board
 */

/* Libraries */
#include "main.h"
#include <Arduino.h>
#include <stdint.h>

/* Setup */
void setup() {
    Serial.begin(115200) ;
    pinMode(LED2_B, OUTPUT) ;
    pinMode(LED1_B, OUTPUT) ;
    pinMode(LED1_G,  OUTPUT) ;
    pinMode(LED1_R,  OUTPUT) ;
    pinMode(BUTTON1, INPUT_PULLUP) ;

    attachInterrupt(digitalPinToInterrupt(BUTTON1), ISR_buttonPressed, FALLING) ;

    LED_init() ;                                                                            // Set all LEDs to OFF position
    buttonCount             = 0 ;                                                               // Accumulator for button presses
    previousMillis_Btn      = 0 ;
    previousMillis_Blink1   = 0 ;
    previousMillis_Blink2   = 0 ;   
}

void loop() {
    currentMillis = millis() ;

    if ( !(buttonCount % 3) ) {                                                             // If the button is pressed 3 times, reset
        LED_init() ;                                                                        // all LEDs to OFF mode
        buttonCount = 0 ;
    }
    else if ( !(buttonCount % 2) ) {
        blueLED1_state  = LED_OFF ;                                                         // Turn the blue LED OFF and the red LED ON
        digitalWrite(LED1_B, blueLED1_state) ;

        if ( (currentMillis - previousMillis_Blink1) >= BLINK1_DELAY ) {
            redLED_state = LED_OFF ;
            digitalWrite(LED1_R, redLED_state) ;
            previousMillis_Blink1 += BLINK1_DELAY ;
        }
        if ( (currentMillis - previousMillis_Blink2) >= BLINK2_DELAY ) {
            redLED_state = LED_ON ;
            digitalWrite(LED1_R, redLED_state) ;
            previousMillis_Blink2 += BLINK2_DELAY ;
        }

    }
    else {                                                                                  // If the button is pressed just once,
        blueLED1_state  = LED_ON ;                                                          // turn on the blue LED and hold the color
        redLED_state    = LED_OFF ;
        
        digitalWrite(LED1_B, blueLED1_state) ;
        digitalWrite(LED1_R,  redLED_state) ;
    }
}

void LED_init(void) {                                                                       // This function turns all LEDs OFF
    blueLED2_state  = LED_OFF ;                                                             // Set the states
    blueLED1_state  = LED_OFF ;
    greenLED_state  = LED_OFF ;
    redLED_state    = LED_OFF ;

    digitalWrite(LED2_B, blueLED2_state) ;                                                  // Write the the pins
    digitalWrite(LED1_B, blueLED1_state) ;
    digitalWrite(LED1_G,  greenLED_state) ;
    digitalWrite(LED1_R,  redLED_state) ;
}

/* Button ISR Handler */
void IRAM_ATTR ISR_buttonPressed(void) {                                                    // This function detects a button press
    if ( (currentMillis - previousMillis_Btn) >= (BUTTON_DEBOUNCE) ) {                         // and then debounces the button with the
        previousMillis_Btn += BUTTON_DEBOUNCE ;                                                // millis nonblocking method. It then
        buttonCount += 1;                                                                   // increments the button counter.
        Serial.printf("Button has been pressed %u times\n", buttonCount);
    }
}

#endif