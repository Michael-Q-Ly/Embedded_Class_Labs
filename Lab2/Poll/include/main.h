#ifndef MAIN_H_
#define MAIN_H_

#include <Arduino.h>

#if !defined(ESP32) && !defined(MSP432401R)
    #warning "No macros defined."
#endif

/* TARGET SELECTION */
#if !defined(MSP432401R)
    /* ESP32 PINS */
    uint8_t const LED2_B        = 19 ;     
    uint8_t const LED1_B        = 18 ;
    uint8_t const LED1_G        = 17 ;
    uint8_t const LED1_R        = 16 ;
    uint8_t const BUTTON1       = 0 ;

    /* Enumerations and Structuress */
    typedef enum {                                                                              // LEDs are active LOW for ESP32
        LED_ON  = LOW ,
        LED_OFF = HIGH
    } LED_State_t ;
    typedef enum {                                                                              // Switches are active LOW
        BUTTON_ON  = LOW,
        BUTTON_OFF = HIGH    
    } Button_State_t ;
#else
    /* MSP432 PINS */
    uint8_t const LED2_B        = 78 ;                                                          // LED2_B is actually yellow on MSP432
    uint8_t const LED1_B        = 77 ;
    uint8_t const LED1_G        = 76 ;
    uint8_t const LED1_R        = 75 ;
    uint8_t const BUTTON1       = 73 ;

    /* Enumerations and Structuress */
    typedef enum {                                                                              // LEDs are active HIGH for MSP432
        LED_ON  = HIGH ,
        LED_OFF = LOW
    } LED_State_t ;
    typedef enum {                                                                              // Switches are active LOW
        BUTTON_ON  = LOW,
        BUTTON_OFF = HIGH    
    } Button_State_t ;
#endif

/* Constants */
uint8_t  const BUTTON_DEBOUNCE  = 150 ;
uint32_t const BLINK1_DELAY     = 50 ;
uint32_t const BLINK2_DELAY     = 950;

/* State Variables */
static LED_State_t      blueLED2_state ;                                                        // LED states
static LED_State_t      blueLED1_state ;
static LED_State_t      greenLED_state ;
static LED_State_t      redLED_state ;
static Button_State_t   button_state_old ;
static Button_State_t   button_state ;

static uint8_t          buttonCount ;                                                           // Buttons
uint8_t                 buttonPress ;

uint32_t                currentMillis ;                                                         // Timing
uint32_t                previousMillis_Btn ;
uint32_t                previousMillis_Blink1 ;
uint32_t                previousMillis_Blink2 ;

/* Function Prototypes */
void LED_init(void) ;
void debounceButton(uint32_t currentMillis) ;
void change_to_state1(void) ;
void change_to_state2(uint32_t currentMillis) ;
void show_button_state_and_count(uint8_t buttonPress, uint8_t buttonCount) ;

#endif /* MAIN_H_ */