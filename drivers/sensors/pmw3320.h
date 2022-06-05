/* Copyright 2020 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
 * Copyright 2019 Sunjun Kim
 * Copyright 2020 Ploopy Corporation
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include <stdint.h>

#ifndef PMW3320_CPI
#    define PMW3320_CPI 200 //Default CPI. 1600 is fine.
#endif

#ifndef PMW3320_CLOCK_SPEED
#    define PMW3320_CLOCK_SPEED 500000 //Max SPI 1MHz
#endif

#ifndef PMW3320_SPI_LSBFIRST
#    define PMW3320_SPI_LSBFIRST false
#endif

#ifndef PMW3320_SPI_MODE
#    define PMW3320_SPI_MODE 3
#endif

#ifndef PMW3320_SPI_DIVISOR
#    ifdef __AVR__
#        define PMW3320_SPI_DIVISOR (F_CPU / PMW3320_CLOCK_SPEED)
#    else
#        define PMW3320_SPI_DIVISOR 16
#    endif
#endif

#ifndef PMW3320_LIFTOFF_DISTANCE
#    define PMW3320_LIFTOFF_DISTANCE 0x02
#endif

#ifndef ROTATIONAL_TRANSFORM_ANGLE
#    define ROTATIONAL_TRANSFORM_ANGLE 0x00
#endif

#ifndef PMW3320_CS_PIN
#    error "No chip select pin defined -- missing PMW3320_CS_PIN"
#endif

typedef struct {
    int8_t  motion;
    bool    isMotion;    // True if a motion is detected.
    bool    isOnSurface; // True when a chip is on a surface
    int8_t dx;          // displacement on x directions. Unit: Count. (CPI * Count = Inch value)
    int8_t mdx;
    int8_t dy; // displacement on y directions.
    int8_t mdy;
} report_pmw3320_t;

bool     pmw3320_init(void);
uint16_t pmw3320_get_cpi(void);
void     pmw3320_set_cpi(uint16_t cpi);
/* Reads and clears the current delta values on the sensor */
report_pmw3320_t pmw3320_read_burst(void);
//report_pmw3320_t pmw3320_read_burst_sec(void);
