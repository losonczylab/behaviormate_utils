#ifndef __DUE_TONE_H__
#define __DUE_TONE_H__

//If we are on the Uno, skip all of this.
#ifndef __AVR_ATmega328P__

#include <Arduino.h>

static int toneReportPin = -1;

/*
 Tone generator
 v1  use timer, and toggle any digital pin in ISR
   funky duration from arduino version
   TODO use FindMckDivisor?
   timer selected will preclude using associated pins for PWM etc.
    could also do timer/pwm hardware toggle where caller controls duration
*/


// timers TC0 TC1 TC2   channels 0-2 ids 0-2  3-5  6-8     AB 0 1
// use TC1 channel 0 

#define TONE_TIMER TC1
#define TONE_CHNL 0
#define TONE_IRQ TC3_IRQn

// TIMER_CLOCK4   84MHz/128 with 16 bit counter give 10 Hz to 656KHz
//  piano 27Hz to 4KHz

static uint8_t TCChanEnabled = 0;
static boolean pin_state = false ;
static Tc *chTC = TONE_TIMER;
static uint32_t chNo = TONE_CHNL;

volatile static int32_t toggle_count;
static uint32_t tone_pin;

// frequency (in hertz) and duration (in milliseconds).

void tone(uint32_t, uint32_t, int32_t);
void tone(uint32_t, uint32_t);

void noTone(uint32_t);

// timer ISR  TC1 ch 0
void TC3_Handler(void);

#endif
#endif
