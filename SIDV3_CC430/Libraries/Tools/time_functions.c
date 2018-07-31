/*
 * time_functions.c
 *
 *  Created on: 16 juin 2015
 *      Author: propriétaire
 */

#include "time_functions.h"

static uint32_t timeTicks = 0u;

#define MS_PER_TICK		1000U

void resetTimeTick(void) {
	timeTicks = 0u;
}

uint32_t getTimeTick(void) {
	return timeTicks;
}

uint32_t getTimeInMs(void) {
	return (timeTicks * MS_PER_TICK);
}

void setTimeout(uint32_t * t, uint32_t delay) {
	*t = timeTicks + (delay/MS_PER_TICK);
}

void wait(uint32_t delay) {
	uint32_t timeout_ = timeTicks + (delay/MS_PER_TICK);
	while(timeTicks < timeout_) {

	}
}

void wait_us(uint32_t delayus)

{

        uint32_t cycles_ = 0u;

        for(cycles_ = 0u; cycles_ < delayus; cycles_++)

        {

                __delay_cycles(20000000u / 1000000u); /* 1us */

        }

}


uint8_t hasTimeout(const uint32_t * t) {

	///P2OUT ^= BIT3;
	return (uint8_t)(timeTicks >= *t);

}

void timetickIncrement(void)
{
	timeTicks++;
}
/* Add Timer ISR Here That increment timeTicks at MS_PER_TICK */


void main_delay_us(uint32_t delay_us)
{
    while(delay_us >= 500)
    {
        main_hal_delay_cycles_500_us();
        delay_us -= 500;
    }

    while(delay_us >= 100)
    {
        main_hal_delay_cycles_100_us();
        delay_us -= 100;
    }

    while(delay_us >= 50)
    {
        main_hal_delay_cycles_50_us();
        delay_us -= 50;
    }

    while(delay_us != 0)
    {
        main_hal_delay_cycles_10_us();
        if(delay_us >= 10) { delay_us -= 10; }
        else               { delay_us  = 0;  }
    }
}




