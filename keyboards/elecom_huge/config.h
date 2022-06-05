// Copyright 2022 Somgo (@Somgo)
// SPDX-License-Identifier: GPL-2.0-or-later

//#pragma once

/*
 * Feature disable options
 *  These options are also useful to firmware size reduction.
 */

/* disable debug print */
//#define NO_DEBUG

/* disable print */
//#define NO_PRINT

/* disable action features */
//#define NO_ACTION_LAYER
//#define NO_ACTION_TAPPING
//#define NO_ACTION_ONESHOT

#pragma once


#include "config_common.h"

/* USB Device descriptor parameter */
#define VENDOR_ID 0x5442
#define PRODUCT_ID 0xFEED
#define DEVICE_VER 0x0001
#define PRODUCT Trackball

/* key matrix size */
#define MATRIX_ROWS 1
#define MATRIX_COLS 10


/* Debounce reduces chatter (unintended double-presses) - set 0 if debouncing is not needed */
#define DEBOUNCE 5

/* define if matrix has ghost (lacks anti-ghosting diodes) */
//#define MATRIX_HAS_GHOST

/* disable action features */
//#define NO_ACTION_LAYER
//#define NO_ACTION_TAPPING
//#define NO_ACTION_ONESHOT

#define USB_MAX_POWER_CONSUMPTION 100

//#define USB_POLLING_INTERVAL_MS 50

/* Bootmagic Lite key configuration */
#define BOOTMAGIC_LITE_ROW 0
#define BOOTMAGIC_LITE_COLUMN 0

//#define ROTATIONAL_TRANSFORM_ANGLE 20

// If board has a debug LED, you can enable it by defining this
// #define DEBUG_LED_PIN F7

/* PMW3360 Settings */
#define PMW3320_CS_PIN             B6
//#define POINTING_DEVICE_INVERT_Y

#define ENCODERS_PAD_A {D1}
#define ENCODERS_PAD_B {D4}
#define ENCODER_RESOLUTION 2

#define TAPPING_TERM 200