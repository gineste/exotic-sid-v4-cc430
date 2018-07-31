/*
 * time_functions.h
 *
 *  Created on: 16 juin 2015
 *      Author: propriétaire
 */

#ifndef TMS570_HAL_TIME_FUNCTIONS_H_
#define TMS570_HAL_TIME_FUNCTIONS_H_


#include <stdint.h>
#include <msp430.h>
//#include <cc430f5135.h>

#define main_hal_delay_cycles_500_us() __delay_cycles(500  * (20000000u / 1000000u))
#define main_hal_delay_cycles_100_us() __delay_cycles(100  * (20000000u / 1000000u))
#define main_hal_delay_cycles_50_us()  __delay_cycles( 50  * (20000000u / 1000000u))
#define main_hal_delay_cycles_10_us()  __delay_cycles( 10  * (20000000u / 1000000u))
/* ....................................................................................... */
/* \public ............................................................................... */
/* \brief delay the execution for a given number of µs.................................... */
/* \details the CPU does not go to sleep; it keeps on being active during that period..... */
/* \param[in] delay_us the delay, in microseconds......................................... */
/* ....................................................................................... */


void resetTimeTick(void) ;
uint32_t getTimeTick(void);
uint32_t getTimeInMs(void);
void setTimeout(uint32_t * t, uint32_t delay);
void wait(uint32_t delay);
void wait_us(uint32_t delayus);
uint8_t hasTimeout(const uint32_t * t);
void timetickIncrement(void);
void main_delay_us(uint32_t delay_us);


#endif /* TMS570_HAL_TIME_FUNCTIONS_H_ */
