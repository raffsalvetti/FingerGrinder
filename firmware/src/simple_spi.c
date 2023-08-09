/*
 * simple_spi.c
 *
 *  Created on: 2 de jun de 2020
 *      Author: raffaello
 */


#include "simple_spi.h"

void simple_spi_init() {
    USICR &= ~(1 << USISIE | 1 << USIOIE | 1 << USIWM1);
    USICR |= (1 << USIWM0 | 1 << USICS1 | 1 << USICLK);
    SPI_DDR_PORT |= (1 << USCK_PIN | 1 << DO_PIN);
    SPI_DDR_PORT &= ~(1 << DI_PIN);
}

void simple_spi_set_data_mode(uint8_t data_mode) {
    if (data_mode == SPI_MODE1) {
        USICR |= (1 << USICS0); // clock via Timer/Counter0 Compare Match
    } else{
        USICR &= ~(1 << USICS0); // clock manual (default)
    }
}

uint8_t simple_spi_send(uint8_t data) {
    USIDR = data;
    USISR = (1 << USIOIF);
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        while ( !(USISR & (1 << USIOIF)) ) USICR |= (1 << USITC);
    }
    return USIDR;
}

void simple_spi_end() {
	USICR &= ~(1 << USIWM1 | 1 << USIWM0);
}
