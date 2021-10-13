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
 *      STATE 0 (OFF)                 = 63.1 mA
 *      State 1 (Steady Blue LED)     = 63.8 mA
 *      State 2 (Blinking Red LED)    = 63.2 mA
 *      State 3 (Light Sleep Mode)    = 2.12 mA
 *      State 4 (Deep Sleep Mode)     = 12uA to 13uA
 */
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
 *  POWER CONSUMPTION: 
 *      STATE 0 (OFF)                 = ( 63.1 mA ) * ( 4.99V ) = 305   mW
 *      State 1 (Steady Blue LED)     = ( 63.8 mA ) * ( 4.99V ) = 318   mW
 *      State 2 (Blinking Red LED)    = ( 63.2 mA ) * ( 4.99V ) = 315   mW
 *      State 3 (Light Sleep Mode)    = ( 2.12 mA ) * ( 4.99V ) = 10.6  mW
 *      State 4 (Deep Sleep Mode)     = ( 12.0 uA ) * ( 4.99V ) = 59.88 uW
 */
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


/* LIBRARIES */
#include "main.h"
#include <Arduino.h>
#include <stdint.h>

/* TARGET SELECTION */
#if 1
    #define ESP32        (1)
#else
    #define MSP432401R   (1)
#endif

/* SETUP */
void setup() {
    Serial.begin(115200) ;
    pinMode(LED2_B, OUTPUT) ;
    pinMode(LED1_B, OUTPUT) ;
    pinMode(LED1_G, OUTPUT) ;
    pinMode(LED1_R, OUTPUT) ;
    pinMode(BUTTON1, INPUT_PULLUP) ;

    attachInterrupt(digitalPinToInterrupt(BUTTON1), ISR_buttonPressed, RISING) ;

    LED_init() ;                                                                            // Set all LEDs to OFF position
    // buttonCount             = 0 ;                                                        // Uninitialized static variables are set to zero in C
    previousMillis_Btn      = 0 ;
    previousMillis_Blink1   = 0 ;
    previousMillis_Blink2   = 0 ;   
}

/* MAIN */
void loop() {
    currentMillis = millis() ;                                                              // Get current time

    if ( !(buttonCount % 7) ) {                                                             // Reset LEDs to OFF state
        LED_init() ;
        buttonCount = 0 ;
    }
    else if ( !(buttonCount % 5) ) {                                                        // Go to deep sleep mode
        change_to_state4() ;
    }
    else if ( !(buttonCount % 3) ) {                                                        // Go to light sleep mode
        change_to_state3() ;
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
#if 0
void debounceButton(void) {                                                                 // This function detects a button press
    currentMillis = millis() ;                                                              // and then debounces the button with the
    if ( (currentMillis - previousMillis_Btn) >= (BUTTON_DEBOUNCE) ) {                      // millis nonblocking method. It then
        previousMillis_Btn += BUTTON_DEBOUNCE ;                                             // increments the button counter.
        buttonCount++ ;
        
        Serial.printf("Button has been pressed %u times\n", buttonCount);
    }
}
#endif
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void change_to_state1(void) {                                                               // On every (7n+1) button press, turn the blue
    blueLED1_state  = LED_ON ;                                                              // LED to an ON steady state
    redLED_state    = LED_OFF ;
    
    digitalWrite(LED1_B, blueLED1_state) ;
    digitalWrite(LED1_R, redLED_state) ;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void change_to_state2(uint32_t currentMillis) {                                             // On every (7n+2) button press, have the blue LED
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

void change_to_state3() {                                                                   // Put the device in light sleep mode.
    LED_init() ;
    Serial.println("Enabling light sleep mode...") ;
    esp_sleep_enable_ext0_wakeup(BUTTON1, BUTTON_ON) ;                                      // Configures light sleep wakeup sources (GPIO)
                                                                                            // then puts the ESP32 into light sleep mode.
    esp_light_sleep_start() ;                                                               // Prints wakeup reason when woken up.

    wakeup_reason = esp_sleep_get_wakeup_cause() ;
    switch(wakeup_reason)
    {
        case ESP_SLEEP_WAKEUP_EXT0      : Serial.println("Wakeup caused by external signal using RTC_IO") ;             break ;
        case ESP_SLEEP_WAKEUP_EXT1      : Serial.println("Wakeup caused by external signal using RTC_CNTL") ;           break ;
        case ESP_SLEEP_WAKEUP_TIMER     : Serial.println("Wakeup caused by timer") ;                                    break ;
        case ESP_SLEEP_WAKEUP_TOUCHPAD  : Serial.println("Wakeup caused by touchpad") ;                                 break ;
        case ESP_SLEEP_WAKEUP_ULP       : Serial.println("Wakeup caused by ULP program") ;                              break ;
        default                         : Serial.printf("Wakeup was not caused by deep sleep: %d\n",wakeup_reason) ;    break ;
    }
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void change_to_state4() {                                                                   // Put the device in deep sleep mode.
    esp_sleep_enable_ext0_wakeup(BUTTON1, BUTTON_ON) ;                                      // Configures deep sleep wakeup sources (GPIO)
                                                                                            // then puts the ESP32 into deep sleep mode.
    esp_deep_sleep_start() ;                                                                // Prints wakeup reason when woken up.
    Serial.println("Enabling deep sleep mode...") ;
    wakeup_reason = esp_sleep_get_wakeup_cause() ;
    switch(wakeup_reason)
    {
        case ESP_SLEEP_WAKEUP_EXT0      : Serial.println("Wakeup caused by external signal using RTC_IO") ;             break ;
        case ESP_SLEEP_WAKEUP_EXT1      : Serial.println("Wakeup caused by external signal using RTC_CNTL") ;           break ;
        case ESP_SLEEP_WAKEUP_TIMER     : Serial.println("Wakeup caused by timer") ;                                    break ;
        case ESP_SLEEP_WAKEUP_TOUCHPAD  : Serial.println("Wakeup caused by touchpad") ;                                 break ;
        case ESP_SLEEP_WAKEUP_ULP       : Serial.println("Wakeup caused by ULP program") ;                              break ;
        default                         : Serial.printf("Wakeup was not caused by deep sleep: %d\n", wakeup_reason) ;   break ;
    }
}

/* Button ISR Handler */
#ifndef MSP432401R
    void IRAM_ATTR ISR_buttonPressed(void) {                                                // This function detects a button press
        #if 1
        if ( (currentMillis - previousMillis_Btn) >= (BUTTON_DEBOUNCE) ) {                  // and then debounces the button with the
            previousMillis_Btn += BUTTON_DEBOUNCE ;                                         // millis nonblocking method. It then
            buttonCount++ ;                                                                 // increments the button counter.
            
            Serial.printf("Button has been pressed %u times\n", buttonCount);
        }
        #else
        debounceButton() ;
        #endif
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
        // debounceButton() ;
    }
#endif