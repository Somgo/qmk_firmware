#include "elecom_huge.h"

#ifndef OPT_DEBOUNCE
#    define OPT_DEBOUNCE 5  // (ms) 			Time between scroll events
#endif
#ifndef SCROLL_BUTT_DEBOUNCE
#    define SCROLL_BUTT_DEBOUNCE 100  // (ms) 			Time between scroll events
#endif
#ifndef OPT_THRES
#    define OPT_THRES 150  // (0-1024) 	Threshold for actication
#endif
#ifndef OPT_SCALE
#    define OPT_SCALE 1  // Multiplier for wheel
#endif
#ifndef ELECOM_CPI_OPTIONS
#    define ELECOM_CPI_OPTIONS \
        { 1200, 1600, 2400 }
#    ifndef ELECOM_CPI_DEFAULT
#        define ELECOM_CPI_DEFAULT 1
#    endif
#endif
#ifndef ELECOM_CPI_DEFAULT
#    define ELECOM_CPI_DEFAULT 0
#endif
#ifndef ELECOM_DRAGSCROLL_CPI
#    define ELECOM_DRAGSCROLL_CPI 100  // Fixed-CPI Drag Scroll
#endif
#ifndef ELECOM_DRAGSCROLL_MULTIPLIER
#    define ELECOM_DRAGSCROLL_MULTIPLIER 0.75  // Variable-CPI Drag Scroll
#endif

keyboard_config_t keyboard_config;
uint16_t          cpi_array[] = ELECOM_CPI_OPTIONS;
#define CPI_OPTION_SIZE (sizeof(cpi_array) / sizeof(uint16_t))

// TODO: Implement libinput profiles
// https://wayland.freedesktop.org/libinput/doc/latest/pointer-acceleration.html
// Compile time accel selection
// Valid options are ACC_NONE, ACC_LINEAR, ACC_CUSTOM, ACC_QUADRATIC

// elecom_huge State
bool     is_scroll_clicked = false;
bool     BurstState        = false;  // init burst state for elecom_huge module
uint16_t MotionStart       = 0;      // Timer for accel, 0 is resting state
uint16_t lastScroll        = 0;      // Previous confirmed wheel event
uint16_t lastMidClick      = 0;      // Stops scrollwheel from being read if it was pressed
//uint8_t  OptLowPin         = OPT_ENC1;
bool     debug_encoder     = false;
bool     is_drag_scroll    = false;

#ifdef ENCODER_ENABLE
bool encoder_update_kb(uint8_t index, bool clockwise) {
    return encoder_update_user(index, clockwise);
}

bool encoder_update_user(uint8_t index, bool clockwise) {
    if (clockwise) {
		tap_code16(KC_WH_U);
	} else {
		tap_code16(KC_WH_D);
	}
	return false;
}

#endif

/*
__attribute__((weak)) bool encoder_update_user(uint8_t index, bool clockwise) { return true; }

bool encoder_update_kb(uint8_t index, bool clockwise) {
    if (!encoder_update_user(index, clockwise)) {
        return false;
    }
#ifdef MOUSEKEY_ENABLE
    tap_code(clockwise ? KC_WH_U : KC_WH_D);
#else
    mouse_report_t mouse_report = pointing_device_get_report();
    mouse_report.v = clockwise ? 1 : -1;
    pointing_device_set_report(mouse_report);
    pointing_device_send();
#endif
    return true;
}


void process_wheel(void) {
    // TODO: Replace this with interrupt driven code,  polling is S L O W
    // Lovingly ripped from the ELECOM Source

    // If the mouse wheel was just released, do not scroll.
    if (timer_elapsed(lastMidClick) < SCROLL_BUTT_DEBOUNCE) {
        return;
    }

    // Limit the number of scrolls per unit time.
    if (timer_elapsed(lastScroll) < OPT_DEBOUNCE) {
        return;
    }

    // Don't scroll if the middle button is depressed.
    if (is_scroll_clicked) {
#ifndef IGNORE_SCROLL_CLICK
        return;
#endif
    }

    lastScroll  = timer_read();
    uint16_t p1 = adc_read(OPT_ENC1_MUX);
    uint16_t p2 = adc_read(OPT_ENC2_MUX);
    if (debug_encoder) dprintf("OPT1: %d, OPT2: %d\n", p1, p2);

    int dir = opt_encoder_handler(p1, p2);

    if (dir == 0) return;
    encoder_update_kb(0, dir == 1);
}

report_mouse_t pointing_device_task_kb(report_mouse_t mouse_report) {
    process_wheel();

    if (is_drag_scroll) {
        mouse_report.h = mouse_report.x;
#ifdef ELECOM_DRAGSCROLL_INVERT
        // Invert vertical scroll direction
        mouse_report.v = -mouse_report.y;
#else
        mouse_report.v = mouse_report.y;
#endif
        mouse_report.x = 0;
        mouse_report.y = 0;
    }

    return pointing_device_task_user(mouse_report);
}
*/
/*
bool process_record_kb(uint16_t keycode, keyrecord_t* record) {
    if (true) {
        xprintf("KL: kc: %u, col: %u, row: %u, pressed: %u\n", keycode, record->event.key.col, record->event.key.row, record->event.pressed);
    }

    // Update Timer to prevent accidental scrolls
    if ((record->event.key.col == 1) && (record->event.key.row == 0)) {
        lastMidClick      = timer_read();
        is_scroll_clicked = record->event.pressed;
    }

    if (!process_record_user(keycode, record)) {
        return false;
    }

    if (keycode == CPI_CONFIG && record->event.pressed) {
        keyboard_config.cpi_config = (keyboard_config.cpi_config + 1) % CPI_OPTION_SIZE;
        eeconfig_update_kb(keyboard_config.raw);
        pointing_device_set_cpi(cpi_array[keyboard_config.cpi_config]);
    }

    if (keycode == DRAG_SCROLL) {
#ifndef ELECOM_DRAGSCROLL_MOMENTARY
        if (record->event.pressed)
#endif
        {
            is_drag_scroll ^= 1;
        }
#ifdef ELECOM_DRAGSCROLL_FIXED
        pointing_device_set_cpi(is_drag_scroll ? ELECOM_DRAGSCROLL_CPI : cpi_array[keyboard_config.cpi_config]);
#else
        pointing_device_set_cpi(is_drag_scroll ? (cpi_array[keyboard_config.cpi_config] * ELECOM_DRAGSCROLL_MULTIPLIER) : cpi_array[keyboard_config.cpi_config]);
#endif
    }

 // If Mousekeys is disabled, then use handle the mouse button
 // keycodes.  This makes things simpler, and allows usage of
 // the keycodes in a consistent manner.  But only do this if
 // Mousekeys is not enable, so it's not handled twice.
 //
#ifndef MOUSEKEY_ENABLE
    if (IS_MOUSEKEY_BUTTON(keycode)) {
        report_mouse_t currentReport = pointing_device_get_report();
        currentReport.buttons        = pointing_device_handle_buttons(currentReport.buttons, record->event.pressed, keycode - KC_MS_BTN1);
        pointing_device_set_report(currentReport);
        pointing_device_send();
    }
#endif

    return true;
}
*/
// Hardware Setup
void keyboard_pre_init_kb(void) {
    //debug_enable  = true;
    // debug_matrix  = true;
    //debug_mouse   = true;
    // debug_encoder = true;

    //setPinInput(OPT_ENC1);
    //setPinInput(OPT_ENC2);
	setPinOutput(D2); //somgo put this here to not run the useless cosmetic led
	writePinHigh(D2); //see above

    /* Ground all output pins connected to ground. This provides additional
     * pathways to ground. If you're messing with this, know this: driving ANY
     * of these pins high will cause a short. On the MCU. Ka-blooey.
     */
#ifdef UNUSED_PINS
    const pin_t unused_pins[] = UNUSED_PINS;

    for (uint8_t i = 0; i < (sizeof(unused_pins) / sizeof(pin_t)); i++) {
        setPinOutput(unused_pins[i]);
        writePinLow(unused_pins[i]);
    }
#endif

    // This is the debug LED.
#if defined(DEBUG_LED_PIN)
    setPinOutput(DEBUG_LED_PIN);
    writePin(DEBUG_LED_PIN, debug_enable);
#endif

    keyboard_pre_init_user();
}

void pointing_device_init_kb(void) {
    pointing_device_set_cpi(cpi_array[keyboard_config.cpi_config]);
    // initialize the scroll wheel's optical encoder retard
    //opt_encoder_init();
}

void eeconfig_init_kb(void) {
    keyboard_config.cpi_config = ELECOM_CPI_DEFAULT;
    eeconfig_update_kb(keyboard_config.raw);
    eeconfig_init_user();
}

void matrix_init_kb(void) {
    // is safe to just read CPI setting since matrix init
    // comes before pointing device init.
    keyboard_config.raw = eeconfig_read_kb();
    if (keyboard_config.cpi_config > CPI_OPTION_SIZE) {
        eeconfig_init_kb();
    }
    matrix_init_user();
}

void keyboard_post_init_kb(void) {
    pointing_device_set_cpi(cpi_array[keyboard_config.cpi_config]);

    keyboard_post_init_user();
}

