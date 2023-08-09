/*
 * simple_spi.h
 *
 *  Created on: 2 de jun de 2020
 *      Author: raffaello
 */

#ifndef SIMPLE_SPI_H_
#define SIMPLE_SPI_H_

#include <avr/io.h>
#include <util/atomic.h>

#define SPI_MODE0 0
#define SPI_MODE1 1

//port configuration
#ifndef SPI_PORT
#define SPI_PORT PORTB
#endif
#ifndef SPI_DDR_PORT
#define SPI_DDR_PORT DDRB
#endif
#ifndef USCK_PIN
#define USCK_PIN 2
#endif
#ifndef DO_PIN
#define DO_PIN 1
#endif
#ifndef DI_PIN
#define DI_PIN 0
#endif

void simple_spi_init();
void simple_spi_set_data_mode(uint8_t data_mode);
uint8_t simple_spi_send(uint8_t data);
void simple_spi_end();

#endif /* SIMPLE_SPI_H_ */
