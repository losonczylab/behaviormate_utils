#include "dueTone.h"

// If we are on the Uno, skip all of this.
#ifndef __AVR_ATmega328P__

/*
 Tone generator
 v1  use timer, and toggle any digital pin in ISR
   funky duration from arduino version
   TODO use FindMckDivisor?
   timer selected will preclude using associated pins for PWM etc.
    could also do timer/pwm hardware toggle where caller controls duration
*/

// frequency (in hertz) and duration (in milliseconds).

void tone(uint32_t ulPin, uint32_t frequency, int32_t duration)
{
		const uint32_t rc = VARIANT_MCK / 256 / frequency; 
		tone_pin = ulPin;
		toggle_count = 0;  // strange  wipe out previous duration
		if (duration > 0 ) toggle_count = 2 * frequency * duration / 1000;
		 else toggle_count = -1;

		if (!TCChanEnabled) {
 			pmc_set_writeprotect(false);
			pmc_enable_periph_clk((uint32_t)TONE_IRQ);
			TC_Configure(chTC, chNo,
				TC_CMR_TCCLKS_TIMER_CLOCK4 |
				TC_CMR_WAVE |         // Waveform mode
				TC_CMR_WAVSEL_UP_RC ); // Counter running up and reset when equals to RC
	
			chTC->TC_CHANNEL[chNo].TC_IER=TC_IER_CPCS;  // RC compare interrupt
			chTC->TC_CHANNEL[chNo].TC_IDR=~TC_IER_CPCS;
			 NVIC_EnableIRQ(TONE_IRQ);
                         TCChanEnabled = 1;
		}
		TC_Stop(chTC, chNo);
                TC_SetRC(chTC, chNo, rc);    // set frequency
		TC_Start(chTC, chNo);
}

void tone(uint32_t ulPin, uint32_t frequency)
{
  tone(ulPin, frequency, -1);
}

void noTone(uint32_t ulPin)
{
	TC_Stop(chTC, chNo);  // stop timer
	digitalWrite(ulPin, LOW);  // no signal on pin
}

// timer ISR  TC1 ch 0
void TC3_Handler ( void ) {
	TC_GetStatus(TC1, 0);
	if (toggle_count != 0){
		// toggle pin  TODO  better
		digitalWrite(tone_pin,pin_state= !pin_state);
		if (toggle_count > 0) toggle_count--;
	} else {
		noTone(tone_pin);
	}
}
#endif
