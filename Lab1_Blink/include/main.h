#ifndef MAIN_H_
#define MAIN_H_

#include <Arduino.h>

gpio_num_t  LED_B        = GPIO_NUM_18 ;         // Blue LED Delays
uint16_t    DELAY_ON_B   = 2000 ;
uint16_t    DELAY_OFF_B  = 4000 ;

gpio_num_t  LED_R        = GPIO_NUM_16 ;         // Red LED Delays
uint16_t    DELAY_ON_R   = 3000 ;
uint16_t    DELAY_OFF_R  = 2000 ;

typedef enum {                                  // Enumerated type
  LED_ON  = LOW ,                               // LEDs are active LOW
  LED_OFF = HIGH
} LED_State_t ;

LED_State_t blueLED_state ;                     // Declare variables of type LED_State_t
LED_State_t redLED_state ;

/* Function prototypes */
void blinkBlueLED(void) ;
void blinkRedLED(void) ;

#endif /* MAIN_H_ */