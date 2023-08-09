/*
 * rgb_led_driver.c
 *
 *  Created on: 27 de jun de 2020
 *      Author: raffaello
 */

#include "rgb_led_driver.h"

typedef struct {
	uint16_t red;
	uint16_t green;
	uint16_t blue;
} rgb_led_t;

static uint8_t rgb_led_current_state = 0;
static rgb_led_t rgb_led;
static uint16_t rgb_led_i;
static uint8_t led_rgb_color;

void rgb_led_init() {
	rgb_led.red = MAX_STEP;
	rgb_led.green = 0;
	rgb_led.blue = 0;
	led_rgb_color = 0x00;
}

static void rainbow() {
    switch (rgb_led_current_state) {
		case REDtoYELLOW: rgb_led.green += STEP; break;
		case YELLOWtoGREEN: rgb_led.red -= STEP; break;
		case GREENtoCYAN: rgb_led.blue += STEP; break;
		case CYANtoBLUE: rgb_led.green -= STEP; break;
		case BLUEtoVIOLET: rgb_led.red += STEP; break;
		case VIOLETtoRED: rgb_led.blue -= STEP; break;
		default: break;
    }

    if (rgb_led.red >= MAX_STEP || rgb_led.green >= MAX_STEP || rgb_led.blue >= MAX_STEP || rgb_led.red <= 0 || rgb_led.green <= 0 || rgb_led.blue <= 0) {
    	rgb_led_current_state = (rgb_led_current_state + 1) % 6; // Finished fading a color so move on to the next
    }
}

uint8_t rgb_led_state() {
	if (rgb_led_i < rgb_led.red) {
		led_rgb_color &= ~(1 << LED_RED);
	} else {
		led_rgb_color |= 1 << LED_RED;
	}

	if (rgb_led_i < rgb_led.green) {
		led_rgb_color &= ~(1 << LED_GREEN);
	} else {
		led_rgb_color |= 1 << LED_GREEN;
	}

	if (rgb_led_i < rgb_led.blue) {
		led_rgb_color &= ~(1 << LED_BLUE);
	} else {
		led_rgb_color |= 1 << LED_BLUE;
	}

	if (rgb_led_i >= MAX_STEP) {
		rainbow();
		rgb_led_i = 0;
	}
	rgb_led_i++;
	return led_rgb_color;
}
