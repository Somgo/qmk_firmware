

#pragma once

#include "quantum.h"
#include "analog.h"
//#include "opt_encoder.h"
/*
#if defined(KEYBOARD_elecomco_trackball_rev1)
#    include "rev1.h"
#elif defined(KEYBOARD_elecomco_trackball_rev1_005)
#    include "rev1_005.h"
#endif
*/
// Sensor defs
//#define OPT_ENC1 D1
//#define OPT_ENC2 D4
//#define OPT_ENC1_MUX 1
//#define OPT_ENC2_MUX 4



#define LAYOUT(M3, M4, M1, WL, B5, WR, BF1, BF2, BF3, M2) \
    { {M3, M4, M1, WL, B5, WR, BF1, BF2, BF3, M2}, }

typedef union {
    uint32_t raw;
    struct {
        uint8_t cpi_config;
    };
} keyboard_config_t;

extern keyboard_config_t keyboard_config;
extern uint16_t          cpi_array[];

enum elecom_keycodes {
#ifdef VIA_ENABLE
    CPI_CONFIG = USER00,
#else
    CPI_CONFIG = SAFE_RANGE,
#endif
    DRAG_SCROLL,
#ifdef VIA_ENABLE
    ELECOM_SAFE_RANGE = SAFE_RANGE,
#else
    ELECOM_SAFE_RANGE,
#endif
};

//bool encoder_update_user(uint8_t index, bool clockwise);
bool encoder_update_kb(uint8_t index, bool clockwise);
bool encoder_update_user(uint8_t index, bool clockwise);

