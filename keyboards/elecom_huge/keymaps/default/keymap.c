#include QMK_KEYBOARD_H

enum {
	TD_LAZY_BASTARD,
};	


void lazy_bastard(qk_tap_dance_state_t *state, void *user_data) {
    if (state->count >= 3) {
        SEND_STRING("Vestibular Schnitzel");
		tap_code16(KC_ENTER);
        reset_tap_dance(state);
    }
}

qk_tap_dance_action_t tap_dance_actions[] = {
    [TD_LAZY_BASTARD] = ACTION_TAP_DANCE_FN(lazy_bastard),
};


const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    /*
     * ┌───┬───┬───┬
     * │ L │ M │ R │
     * ├───┼───┼───┼
     */
    [0] = LAYOUT(
        KC_BTN3, KC_E, KC_BTN1, KC_WH_L, KC_BTN5, KC_WH_R, TD(TD_LAZY_BASTARD), KC_D, KC_BTN4, KC_BTN2
    ),
}; 	//"D0", "C6", "D7", "E6", "B4", "B5", "F7", "F6", "F5", "F4"
	// M3,   FWD,  ML,   WH_L, BCK,  WH_R, Fn1,  Fn2,  Fn3,  M2
void keyboard_post_init_user(void) {
    debug_mouse = false;
}

