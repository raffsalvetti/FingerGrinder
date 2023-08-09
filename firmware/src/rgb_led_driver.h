/*
 * rgb_led_driver.h
 *
 *  Created on: 27 de jun de 2020
 *      Author: raffaello
 */

#ifndef RGB_LED_DRIVER_H_
#define RGB_LED_DRIVER_H_

#include <avr/io.h>

/*
 * I got this code from
 * https://blog.podkalicki.com/attiny13-controlling-led-rgb-fancy-light-effects/
 * */

#ifndef LED_RED
#define    LED_RED			0
#endif
#ifndef LED_GREEN
#define    LED_GREEN		1
#endif
#ifndef LED_BLUE
#define    LED_BLUE			2
#endif

#ifndef MAX_STEP
#define    MAX_STEP			(32)
#endif
#ifndef STEP
#define    STEP				(1)
#endif

#define    REDtoYELLOW		(0)
#define    YELLOWtoGREEN	(1)
#define    GREENtoCYAN		(2)
#define    CYANtoBLUE		(3)
#define    BLUEtoVIOLET		(4)
#define    VIOLETtoRED		(5)

void rgb_led_init();
uint8_t rgb_led_state();

#endif /* RGB_LED_DRIVER_H_ */
