/*  Copyright 2020
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

#include <stdbool.h>

#include "gpio.h"

typedef int16_t spi_status_t;

#ifndef LSBFIRST
#define LSBFIRST 0
#endif
#ifndef MSBFIRST
#define MSBFIRST 1
#endif

#define SPI_CLOCK_DIV4 0x00
#define SPI_CLOCK_DIV16 0x01
#define SPI_CLOCK_DIV64 0x02
#define SPI_CLOCK_DIV128 0x03
#define SPI_CLOCK_DIV2 0x04
#define SPI_CLOCK_DIV8 0x05
#define SPI_CLOCK_DIV32 0x06

#define SPI_MODE0 0x00
#define SPI_MODE1 0x04
#define SPI_MODE2 0x08
#define SPI_MODE3 0x0C

// Hardware SS pin is defined in the header so that user code can refer to it
#if defined(__AVR_AT90USB162__) || defined(__AVR_ATmega16U2__) || defined(__AVR_ATmega32U2__) || defined(__AVR_ATmega16U4__) || defined(__AVR_ATmega32U4__) || defined(__AVR_AT90USB646__) || defined(__AVR_AT90USB647__) || defined(__AVR_AT90USB1286__) || defined(__AVR_AT90USB1287__)
#    define SPI_SS_PIN B0
#elif defined(__AVR_ATmega32A__)
#    define SPI_SS_PIN B4
#elif defined(__AVR_ATmega328P__) || defined(__AVR_ATmega328__)
#    define SPI_SS_PIN B2
#endif

#define SPI_STATUS_SUCCESS (0)
#define SPI_STATUS_ERROR (-1)
#define SPI_STATUS_TIMEOUT (-2)

#define SPI_TIMEOUT_IMMEDIATE (0)
#define SPI_TIMEOUT_INFINITE (0xFFFF)

#define SPI_MODE_MASK 0x0C  // CPOL = bit 3, CPHA = bit 2 on SPCR
#define SPI_CLOCK_MASK 0x03  // SPR1 = bit 1, SPR0 = bit 0 on SPCR
#define SPI_2XCLOCK_MASK 0x01  // SPI2X = bit 0 on SPSR


#ifdef __cplusplus
extern "C" {
#endif
void spi_init(void);

bool spi_start(long int clock, pin_t slavePin, bool lsbFirst, uint8_t mode, uint16_t divisor);

spi_status_t spi_write(uint8_t data);

spi_status_t spi_read(void);

spi_status_t spi_transmit(const uint8_t *data, uint16_t length);

spi_status_t spi_receive(uint8_t *data, uint16_t length);
/*
inline static void spi_transfer(void *buf, uint16_t count) {
	if (count == 0) return;
	uint8_t *p = (uint8_t *)buf;
	SPDR = *p;
	while (--count > 0) {
	  uint8_t out = *(p + 1);
	  while (!(SPSR & _BV(SPIF))) ;
	  uint8_t in = SPDR;
	  SPDR = out;
	  *p++ = in;
	}
	while (!(SPSR & _BV(SPIF))) ;
	*p = SPDR;
}
*/
void spi_stop(void);
#ifdef __cplusplus
}
#endif
