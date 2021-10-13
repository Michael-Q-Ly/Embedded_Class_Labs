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
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
 * CURRENT READINGS:
 * WITH no delay()
 *    State 1 (Steady Blue LED):
 *    OFF                          = 63.2 mA
 *    State 1 (Steady Blue LED)    = 63.4 mA
 *    State 2 (Blinking Red LED)   = 63.3 mA
 */
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* Libraries */
#include "main.h"
#include <Arduino.h>
#include <stdint.h>

// #define PRINT_TO_CONSOLE     (1)

/* TARGET SELECTION */
#if 1
    #define ESP32        (1)
#else
    #define MSP432401R   (1)
#endif

/* Target Initialization */
void setup() {
    Serial.begin(115200) ;
    pinMode(LED2_B, OUTPUT) ;
    pinMode(LED1_B, OUTPUT) ;
    pinMode(LED1_G, OUTPUT) ;
    pinMode(LED1_R, OUTPUT) ;
    pinMode(BUTTON1, INPUT_PULLUP) ;

    LED_init() ;                                                                            // Set all LEDs to OFF position
    button_state_old        = BUTTON_OFF ;                                                  // Default state of button is OFF
    buttonCount             = 0 ;                                                           // Accumulator for button presses
    previousMillis_Btn      = 0 ;
    previousMillis_Blink1   = 0 ;
    previousMillis_Blink2   = 0 ;
    
}

/* Main */
void loop() {
    button_state    = (Button_State_t)( digitalRead(BUTTON1) ) ;                            // Read button state and type cast read method
    buttonPress     = (button_state_old == BUTTON_OFF) && (button_state == BUTTON_ON) ;     // Flag that detects a button press
    
    currentMillis = millis() ;                                                              // Get the current time
    
    if (buttonPress) {
        debounceButton(currentMillis) ;                                                     // Debounce the button press
        
        #ifdef PRINT_TO_CONSOLE
            show_button_state_and_count(button_state, buttonCount) ;                        // Print button state and count to console
        #endif
    }

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

    button_state_old = button_state ;                                                       // Keep track of last button state
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

void debounceButton(uint32_t currentMillis) {                                               // Debounces button presses
    if( (currentMillis - previousMillis_Btn) >= BUTTON_DEBOUNCE) {
        buttonCount++ ;
        previousMillis_Btn += BUTTON_DEBOUNCE ;
    }
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void change_to_state1(void) {                                                               // On every (3n+1) button press, turn the blue
    blueLED1_state  = LED_ON ;                                                              // LED to an ON steady state
    redLED_state    = LED_OFF ;

    digitalWrite(LED1_B, blueLED1_state) ;
    digitalWrite(LED1_R, redLED_state) ;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void change_to_state2(uint32_t currentMillis) {                                             // On every (3n+2) button press, have the blue LED
    blueLED1_state  = LED_OFF ;                                                             // in the OFF position, but flash on the red LED
    digitalWrite(LED1_B, blueLED1_state) ;                                                  // for 50ms with total cycle time of 1s.

    if ( (currentMillis - previousMillis_Blink1) >= BLINK1_DELAY ) {                        // ON for 50ms
        redLED_state = LED_OFF ;
        digitalWrite(LED1_R, redLED_state) ;
        previousMillis_Blink1 += BLINK1_DELAY ;
    }
    if ( (currentMillis - previousMillis_Blink2) >= BLINK2_DELAY ) {                        // OFF for 950ms
        redLED_state = LED_ON ;
        digitalWrite(LED1_R, redLED_state) ;
        previousMillis_Blink2 += BLINK2_DELAY ;
    }
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void show_button_state_and_count(uint8_t button_state, uint8_t buttonCount) {               // Show the current button state and button count
    #if !defined(MSP432401R)
        /* ESP32 */
        Serial.printf("Button State: %u\n", button_state) ;
        Serial.printf("Button is pressed %u times\n", buttonCount) ;
    #else
        /* MSP432401R */
        Serial.print("Button State: ") ;
        Serial.println(button_state) ;
        Serial.print("Button is pressed ") ;
        Serial.print(buttonCount) ;
        Serial.println(" times") ;
    #endif
}
