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

#include "spi_master.h"

#include "timer.h"

#if defined(__AVR_AT90USB162__) || defined(__AVR_ATmega16U2__) || defined(__AVR_ATmega32U2__) || defined(__AVR_ATmega16U4__) || defined(__AVR_ATmega32U4__) || defined(__AVR_AT90USB646__) || defined(__AVR_AT90USB647__) || defined(__AVR_AT90USB1286__) || defined(__AVR_AT90USB1287__)
#    define SPI_SCK_PIN B1
#    define SPI_MOSI_PIN B2
#    define SPI_MISO_PIN B3
#elif defined(__AVR_ATmega32A__)
#    define SPI_SCK_PIN B7
#    define SPI_MOSI_PIN B5
#    define SPI_MISO_PIN B6
#elif defined(__AVR_ATmega328P__) || defined(__AVR_ATmega328__)
#    define SPI_SCK_PIN B5
#    define SPI_MOSI_PIN B3
#    define SPI_MISO_PIN B4
#endif

#ifndef SPI_TIMEOUT
#    define SPI_TIMEOUT 100
#endif

static pin_t   currentSlavePin    = NO_PIN;
static uint8_t currentSlaveConfig = 0;
static bool    currentSlave2X     = false;

void spi_init(void) {
	//taken from arduino SPI.cpp . to reset fork from source.
	uint8_t sreg = SREG;
	//noInterrupts();
	
    writePinHigh(SPI_SS_PIN);
	setPinOutput(SPI_SS_PIN);
	
	SPCR = (_BV(SPE) | _BV(MSTR));
	
    setPinOutput(SPI_SCK_PIN);
    setPinOutput(SPI_MOSI_PIN);
    //setPinInput(SPI_MISO_PIN);
	SREG = sreg;
}

bool spi_start(long int clock, pin_t slavePin, bool lsbFirst, uint8_t mode, uint16_t divisor) {
    if (currentSlavePin != NO_PIN || slavePin == NO_PIN) {
        return false;
    }
	/* Clock settings are defined as follows. Note that this shows SPI2X
    // inverted, so the bits form increasing numbers. Also note that
    // fosc/64 appears twice
    // SPR1 SPR0 ~SPI2X Freq
    //   0    0     0   fosc/2
    //   0    0     1   fosc/4
    //   0    1     0   fosc/8
    //   0    1     1   fosc/16
    //   1    0     0   fosc/32
    //   1    0     1   fosc/64
    //   1    1     0   fosc/64
    //   1    1     1   fosc/128

    // We find the fastest clock that is less than or equal to the
    // given clock rate. The clock divider that results in clock_setting
    // is 2 ^^ (clock_div + 1). If nothing is slow enough, we'll use the
    // slowest (128 == 2 ^^ 7, so clock_div = 6).
	*/
    
	/*
    // When the clock is known at compiletime, use this if-then-else
    // cascade, which the compiler knows how to completely optimize
    // away. When clock is not known, use a loop instead, which generates
    // shorter code.
	*/
	uint8_t clockDiv;
    if (__builtin_constant_p(clock)) {
      if (clock >= F_CPU / 2) {
        clockDiv = 0;
      } else if (clock >= F_CPU / 4) {
        clockDiv = 1;
      } else if (clock >= F_CPU / 8) {
        clockDiv = 2;
      } else if (clock >= F_CPU / 16) {
        clockDiv = 3;
      } else if (clock >= F_CPU / 32) {
        clockDiv = 4;
      } else if (clock >= F_CPU / 64) {
        clockDiv = 5;
      } else {
        clockDiv = 6;
      }
    } else {
      uint32_t clockSetting = F_CPU / 2;
      clockDiv = 0;
      while (clockDiv < 6 && clock < clockSetting) {
        clockSetting /= 2;
        clockDiv++;
      }
    }

    // Compensate for the duplicate fosc/64
    if (clockDiv == 6)
    clockDiv = 7;

    // Invert the SPI2X bit
    clockDiv ^= 0x1;


    SPCR = _BV(SPE) | _BV(MSTR) | ((lsbFirst == true) ? _BV(DORD) : 0) |
      (mode & SPI_MODE_MASK) | ((clockDiv >> 1) & SPI_CLOCK_MASK);
    SPSR = clockDiv & SPI_2XCLOCK_MASK;
	
	currentSlavePin = slavePin;
    setPinOutput(currentSlavePin);
    writePinLow(currentSlavePin);

    return true;
  }
    
    


spi_status_t spi_write(uint8_t data) {
    /*SPDR = data;
    uint16_t timeout_timer = timer_read();
    while (!(SPSR & _BV(SPIF))) {
        if ((timer_read() - timeout_timer) >= SPI_TIMEOUT) {
            return SPI_STATUS_TIMEOUT;
        }
    }
    return SPDR;
	*/
	SPDR = data;
	while (!(SPSR & (1 << SPIF)));
	return SPDR;
}

spi_status_t spi_read() {
    /*SPDR = 0x00; // Dummy
    uint16_t timeout_timer = timer_read();
    while (!(SPSR & _BV(SPIF))) {
        if ((timer_read() - timeout_timer) >= SPI_TIMEOUT) {
            return SPI_STATUS_TIMEOUT;
        }
    }
	return SPDR;*/
	SPDR = 0x00;
	while (!(SPSR & (1 << SPIF)));
	return SPDR;
}

spi_status_t spi_transmit(const uint8_t *data, uint16_t length) {
    spi_status_t status;

    for (uint16_t i = 0; i < length; i++) {
        status = spi_write(data[i]);

        if (status < 0) {
            return status;
        }
    }

    return SPI_STATUS_SUCCESS;
}

spi_status_t spi_receive(uint8_t *data, uint16_t length) {
    spi_status_t status;

    for (uint16_t i = 0; i < length; i++) {
        status = spi_read();

        //if (status >= 0) {
            data[i] = status;
        //} else {
        //    return status;
        //}
    }

    return SPI_STATUS_SUCCESS;
}

void spi_stop(void) {
    if (currentSlavePin != NO_PIN) {
        setPinOutput(currentSlavePin);
        writePinHigh(currentSlavePin);
        currentSlavePin = NO_PIN;
        SPSR &= ~(_BV(SPI2X));
        SPCR &= ~(currentSlaveConfig);
        currentSlaveConfig = 0;
        currentSlave2X     = false;
    }
}
