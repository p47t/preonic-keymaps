/* Copyright 2015-2021 Jack Humbert
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

#include "keycode.h"
#include QMK_KEYBOARD_H
#include "muse.h"

// Tap dance

typedef enum {
    TAP_STATE_RESET,
    SINGLE_TAP,
    SINGLE_HOLD,
    DOUBLE_TAP,
    DOUBLE_HOLD,
    TRIPLE_TAP,
    TRIPLE_HOLD,
} tap_state_t;

// Global TapDance State
static struct t_tap {
    tap_state_t l_brackets;
    tap_state_t r_brackets;
    tap_state_t quotes;
    tap_state_t semicolon;
    tap_state_t grave;
} qk_tap_state = {
    .l_brackets = 0,
    .r_brackets = 0,
    .quotes = 0,
    .semicolon = 0,
    .grave = 0,
};

/* Sentinel value for invalid tap dance exit */
#define TAP_DANCE_NO_MATCH 64

tap_state_t get_tapdance_state(qk_tap_dance_state_t *state) {
    if (state->count == 1) {
        if (state->interrupted || !state->pressed) {
            return SINGLE_TAP;
        } else {
            return SINGLE_HOLD;
        }
    } else if (state->count == 2) {
        if (state->interrupted || !state->pressed) {
            return DOUBLE_TAP;
        } else {
            return DOUBLE_HOLD;
        }
    } else if (state->count == 3) {
        if (state->interrupted || !state->pressed) {
            return TRIPLE_TAP;
        } else {
            return TRIPLE_HOLD;
        }
    } else
        return TAP_DANCE_NO_MATCH;
}

void td_l_brackets_finished(qk_tap_dance_state_t *state, void *user_data) {
    qk_tap_state.l_brackets = get_tapdance_state(state);
    switch (qk_tap_state.l_brackets) {
        case SINGLE_TAP:
            register_code16(KC_LEFT_PAREN);
            break;
        case DOUBLE_TAP:
            register_code(KC_LBRACKET);
            break;
        case TRIPLE_TAP:
            register_code16(KC_LEFT_CURLY_BRACE);
            break;
        default:
            break;
    }
}

void td_l_brackets_reset(qk_tap_dance_state_t *state, void *user_data) {
    switch (qk_tap_state.l_brackets) {
        case SINGLE_TAP:
            unregister_code16(KC_LEFT_PAREN);
            break;
        case DOUBLE_TAP:
            unregister_code(KC_LBRACKET);
            break;
        case TRIPLE_TAP:
            unregister_code16(KC_LEFT_CURLY_BRACE);
            break;
        default:
            break;
    }
    qk_tap_state.l_brackets = 0;
}

void td_r_brackets_finished(qk_tap_dance_state_t *state, void *user_data) {
    qk_tap_state.r_brackets = get_tapdance_state(state);
    switch (qk_tap_state.r_brackets) {
        case SINGLE_TAP:
            register_code16(KC_RIGHT_PAREN);
            break;
        case DOUBLE_TAP:
            register_code(KC_RBRACKET);
            break;
        case TRIPLE_TAP:
            register_code16(KC_RIGHT_CURLY_BRACE);
            break;
        default:
            break;
    }
}

void td_r_brackets_reset(qk_tap_dance_state_t *state, void *user_data) {
    switch (qk_tap_state.r_brackets) {
        case SINGLE_TAP:
            unregister_code16(KC_RIGHT_PAREN);
            break;
        case DOUBLE_TAP:
            unregister_code(KC_RBRACKET);
            break;
        case TRIPLE_TAP:
            unregister_code16(KC_RIGHT_CURLY_BRACE);
            break;
        default:
            break;
    }
    qk_tap_state.r_brackets = 0;
}

void td_quotes_finished(qk_tap_dance_state_t *state, void *user_data) {
    qk_tap_state.quotes = get_tapdance_state(state);
    switch (qk_tap_state.quotes) {
        case SINGLE_TAP:
            register_code16(KC_QUOTE);
            break;
        case DOUBLE_TAP:
            register_code16(KC_DOUBLE_QUOTE);
            break;
        case TRIPLE_TAP:
            register_code16(KC_PIPE);
        default:
            break;
    }
}

void td_quotes_reset(qk_tap_dance_state_t *state, void *user_data) {
    switch (qk_tap_state.quotes) {
        case SINGLE_TAP:
            unregister_code16(KC_QUOTE);
            break;
        case DOUBLE_TAP:
            unregister_code16(KC_DOUBLE_QUOTE);
            break;
        case TRIPLE_TAP:
            unregister_code16(KC_PIPE);
            break;
        default:
            break;
    }
    qk_tap_state.quotes = 0;
}

void td_semicolon_finished(qk_tap_dance_state_t *state, void *user_data) {
    qk_tap_state.semicolon = get_tapdance_state(state);
    switch (qk_tap_state.semicolon) {
        case SINGLE_TAP:
            register_code16(KC_SEMICOLON);
            break;
        case DOUBLE_TAP:
            SEND_STRING(SS_TAP(X_END)";");
            break;
        default:
            break;
    }
}

void td_semicolon_reset(qk_tap_dance_state_t *state, void *user_data) {
    switch (qk_tap_state.semicolon) {
        case SINGLE_TAP:
            unregister_code16(KC_SEMICOLON);
            break;
        case DOUBLE_TAP:
            break;
        default:
            break;
    }
    qk_tap_state.semicolon = 0;
}

void td_grave_finished(qk_tap_dance_state_t *state, void *user_data) {
    qk_tap_state.grave = get_tapdance_state(state);
    switch (qk_tap_state.grave) {
        case SINGLE_TAP:
            register_code16(KC_GRV);
            break;
        case DOUBLE_TAP:
            register_code16(KC_TILDE);
            break;
        case TRIPLE_TAP:
            SEND_STRING("```");
            break;
        default:
            break;
    }
}

void td_grave_reset(qk_tap_dance_state_t *state, void *user_data) {
    switch (qk_tap_state.grave) {
        case SINGLE_TAP:
            unregister_code16(KC_GRV);
            break;
        case DOUBLE_TAP:
            unregister_code16(KC_TILDE);
            break;
        case TRIPLE_TAP:
            break;
        default:
            break;
    }
    qk_tap_state.grave = 0;
}

// Tap Dance Declarations
enum tapdance_keycodes {
    TD_L_BRACKETS,
    TD_R_BRACKETS,
    TD_QUOTES,
    TD_SEMICOLON,
    TD_SLASHES,
    TD_GRV,
};

// Tap Dance Definitions
qk_tap_dance_action_t tap_dance_actions[] = {
    // once: left parenthesis, twice: left bracket, thrice: left brace
    [TD_L_BRACKETS] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, td_l_brackets_finished, td_l_brackets_reset),

    // once: right parenthesis, twice: right bracket, thrice: right brace
    [TD_R_BRACKETS] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, td_r_brackets_finished, td_r_brackets_reset),

    // once: single quote, twice: double quote, thrice: backtick
    [TD_QUOTES] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, td_quotes_finished, td_quotes_reset),

    // once: semicolon, twice: end and semicolon
    [TD_SEMICOLON] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, td_semicolon_finished, td_semicolon_reset),

    // once: slash, twice: back slash
    [TD_SLASHES] = ACTION_TAP_DANCE_DOUBLE(KC_SLASH, KC_BACKSLASH),

    // once: `, twice: ~, thrice: `|`
    [TD_GRV] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, td_grave_finished, td_grave_reset),
};

#define TD_LBRK TD(TD_L_BRACKETS)
#define TD_RBRK TD(TD_R_BRACKETS)
#define TD_QUOT TD(TD_QUOTES)
#define TD_SCLN TD(TD_SEMICOLON)
#define TD_SLSH TD(TD_SLASHES)
#define TD_GRV TD(TD_GRV)

// Layers

enum preonic_layers {
    LAYER_QWERTY,
    LAYER_LOWER,
    LAYER_RAISE,
    LAYER_ADJUST,
};

enum preonic_keycodes {
    LOWER = SAFE_RANGE,
    RAISE,
    BACKLIT,
    MICMUTE, // mute microphone
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

// Mod-tap for easy - and =
#define MT_LSFT MT(MOD_LSFT, KC_MINUS)
#define MT_RSFT MT(MOD_RSFT, KC_EQUAL)
#define MT_LCTL MT(MOD_LCTL, KC_LPRN)
#define MT_LALT MT(MOD_LALT, KC_LBRC)
#define MT_LGUI MT(MOD_LGUI, KC_LCBR)

/* Qwerty
 * ,-----------------------------------------------------------------------------------------------.
 * |  Esc  |   1   |   2   |   3   |   4   |   5   |   6   |   7   |   8   |   9   |   0   | Bksp  |
 * |-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------|
 * |  Tab  |   Q   |   W   |   E   |   R   |   T   |   Y   |   U   |   I   |   O   |   P   |  '"|  |
 * |-------+-------+-------+-------+-------+---------------+-------+-------+-------+-------+-------|
 * |   `   |   A   |   S   |   D   |   F   |   G   |   H   |   J   |   K   |   L   |   ;   | Enter |
 * |-------+-------+-------+-------+-------+-------|-------+-------+-------+-------+-------+-------|
 * |Shift -|   Z   |   X   |   C   |   V   |   B   |   N   |   M   |   ,   |   .   |  /\   |Shift =|
 * |-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------|
 * |MicMute| Ctrl (| Alt [ | GUI { |Lower  |     Space     | Raise | Left  | Down  |  Up   | Right |
 * `-----------------------------------------------------------------------------------------------'
 */
[LAYER_QWERTY] = LAYOUT_preonic_grid(
    KC_ESC,  KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_BSPC,
    KC_TAB,  KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    TD_QUOT,
    TD_GRV,  KC_A,    KC_S,    KC_D,    KC_F,    KC_G,    KC_H,    KC_J,    KC_K,    KC_L,    TD_SCLN, KC_ENT,
    MT_LSFT, KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_N,    KC_M,    KC_COMM, KC_DOT,  TD_SLSH, MT_RSFT,
    MICMUTE, MT_LCTL, MT_LALT, MT_LGUI, LOWER,   KC_SPC,  KC_SPC,  RAISE,   KC_LEFT, KC_DOWN, KC_UP,   KC_RGHT
),

/* Lower
 * ,-----------------------------------------------------------------------------------------------.
 * |  Del  |   !   |   @   |   #   |   $   |   %   |   ^   |   &   |   *   |   (   |   )   |  Del  |
 * |-------+-------+-------+-------+-------+---------------+-------+-------+-------+-------+-------|
 * |   ~   |   !   |   @   |   #   |   $   |   %   |   ^   |   &   |   *   |   [   |   ]   |   |   |
 * |-------+-------+-------+-------+-------+---------------+-------+-------+-------+-------+-------|
 * |   ~   |  F1   |  F2   |  F3   |  F4   |  F5   |  F6   |   _   |   +   |   {   |   }   |       |
 * |-------+-------+-------+-------+-------+-------|-------+-------+-------+-------+-------+-------|
 * |       |  F7   |  F8   |  F9   |  F10  |  F11  |  F12  |ISO ~  |ISO |  | Home  | End   |       |
 * |-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------|
 * |       |       |       |       |       |               |       | Next  | Vol-  | Vol+  |  Play |
 * `-----------------------------------------------------------------------------------------------'
 */
[LAYER_LOWER] = LAYOUT_preonic_grid(
    KC_DEL,  KC_EXLM, KC_AT,   KC_HASH, KC_DLR,  KC_PERC, KC_CIRC, KC_AMPR,    KC_ASTR,    KC_LPRN, KC_RPRN, KC_DEL,
    KC_TILD, KC_EXLM, KC_AT,   KC_HASH, KC_DLR,  KC_PERC, KC_CIRC, KC_AMPR,    KC_ASTR,    KC_LBRC, KC_RBRC, KC_PIPE,
    KC_TILD, KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,   KC_UNDS,    KC_PLUS,    KC_LCBR, KC_RCBR, _______,
    _______, KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,  KC_F12,  S(KC_NUHS), S(KC_NUBS), KC_HOME, KC_END,  _______,
    _______, _______, _______, _______, _______, _______, _______, _______,    KC_MNXT,    KC_VOLD, KC_VOLU, KC_MPLY
),

/* Raise
 * ,-----------------------------------------------------------------------------------------------.
 * |  Del  |  F1   |  F2   |  F3   |  F4   |  F5   |  F6   |  F7   |  F8   |  F9   | F10   |  Del  |
 * |-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------|
 * |   `   |  F11  |  F12  |  F13  |  F14  |  F15  |  F16  |  F17  |  F18  |  F19  | F20   |   \   |
 * |-------+-------+-------+-------+-------+---------------+-------+-------+-------+-------+-------|
 * |   `   |       |       |       |       |       |       |   -   |   =   |   [   |   ]   |       |
 * |-------+-------+-------+-------+-------+-------|-------+-------+-------+-------+-------+-------|
 * |       |       |       |       |       |       |       |ISO #  |ISO /  | Pg Up | Pg Dn |       |
 * |-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------|
 * |       |       |       |       |       |               |       | Next  | Vol-  | Vol+  |  Play |
 * `-----------------------------------------------------------------------------------------------'
 */
[LAYER_RAISE] = LAYOUT_preonic_grid(
    KC_DEL,  KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_DEL,
    KC_GRV,  KC_F11,  KC_F12,  KC_F13,  KC_F14,  KC_F15,  KC_F16,  KC_F17,  KC_F18,  KC_F19,  KC_F20,  KC_BSLS,
    KC_GRV,  _______, _______, _______, _______, _______, _______, KC_MINS, KC_EQL,  KC_LBRC, KC_RBRC, _______,
    _______, _______, _______, _______, _______, _______, _______, KC_NUHS, KC_NUBS, KC_PGUP, KC_PGDN, _______,
    _______, _______, _______, _______, _______, _______, _______, _______, KC_MNXT, KC_VOLD, KC_VOLU, KC_MPLY
),

/* Adjust (Lower + Raise)
 * ,-----------------------------------------------------------------------------------------------.
 * |  F1   |  F2   |  F3   |  F4   |  F5   |  F6   |  F7   |  F8   |  F9   |  F10  |  F11  |  F12  |
 * |-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------|
 * |       | Reset | Debug |       |       |       |       |TermOf |TermOn |       |       |       |
 * |-------+-------+-------+-------+-------+---------------+-------+-------+-------+-------+-------|
 * |       |       |Aud cy |Aud on |AudOff |AGnorm |AGswap |       |       |       |       |       |
 * |-------+-------+-------+-------+-------+-------|-------+-------+-------+-------+-------+-------|
 * |       |Voice- |Voice+ |Mus on |MusOff |MidiOn |MidOff |       |       |       |       |       |
 * |-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------|
 * | Brite |       |       |       |       |               |       |       |       |       |       |
 * `-----------------------------------------------------------------------------------------------'
 */
[LAYER_ADJUST] = LAYOUT_preonic_grid(
    KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,  KC_F12,
    _______, RESET,   DEBUG,   _______, _______, _______, _______, TERM_ON, TERM_OFF,_______, _______, _______,
    _______, _______, MU_MOD,  AU_ON,   AU_OFF,  AG_NORM, AG_SWAP, _______, _______, _______, _______, _______,
    _______, MUV_DE,  MUV_IN,  MU_ON,   MU_OFF,  MI_ON,   MI_OFF,  _______, _______, _______, _______, _______,
    BACKLIT, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______
)

}; // keymaps

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
    case LOWER:
        if (record->event.pressed) {
            layer_on(LAYER_LOWER);
            update_tri_layer(LAYER_LOWER, LAYER_RAISE, LAYER_ADJUST);
        } else {
            layer_off(LAYER_LOWER);
            update_tri_layer(LAYER_LOWER, LAYER_RAISE, LAYER_ADJUST);
        }
        return false;
    case RAISE:
        if (record->event.pressed) {
            layer_on(LAYER_RAISE);
            update_tri_layer(LAYER_LOWER, LAYER_RAISE, LAYER_ADJUST);
        } else {
            layer_off(LAYER_RAISE);
            update_tri_layer(LAYER_LOWER, LAYER_RAISE, LAYER_ADJUST);
        }
        return false;
    case BACKLIT:
        if (record->event.pressed) {
            register_code(KC_RSFT);
#ifdef BACKLIGHT_ENABLE
            backlight_step();
#endif
#ifdef RGBLIGHT_ENABLE
            rgblight_step();
#endif
#ifdef __AVR__
            writePinLow(E6);
#endif
        } else {
            unregister_code(KC_RSFT);
#ifdef __AVR__
            writePinHigh(E6);
#endif
        }
        return false;
    case MICMUTE:
        if (record->event.pressed) {
           SEND_STRING(SS_LGUI("y")); // Cmd-Y for chime
        }
        break;
    }
    return true;
}

// Muse

bool muse_mode = false;
uint8_t last_muse_note = 0;
uint16_t muse_counter = 0;
uint8_t muse_offset = 70;
uint16_t muse_tempo = 50;

bool encoder_update_user(uint8_t index, bool clockwise) {
    if (muse_mode) {
        if (IS_LAYER_ON(LAYER_RAISE)) {
            if (clockwise) {
                muse_offset++;
            } else {
                muse_offset--;
            }
        } else {
            if (clockwise) {
                muse_tempo += 1;
            } else {
                muse_tempo -= 1;
            }
        }
    } else {
        if (clockwise) {
            register_code(KC_PGDN);
            unregister_code(KC_PGDN);
        } else {
            register_code(KC_PGUP);
            unregister_code(KC_PGUP);
        }
    }
    return true;
}

bool dip_switch_update_user(uint8_t index, bool active) {
    switch (index) {
    case 0:
        if (active) {
            layer_on(LAYER_ADJUST);
        } else {
            layer_off(LAYER_ADJUST);
        }
        break;
    case 1:
        if (active) {
            muse_mode = true;
        } else {
            muse_mode = false;
        }
    }
    return true;
}

void matrix_scan_user(void) {
#ifdef AUDIO_ENABLE
    if (muse_mode) {
        if (muse_counter == 0) {
            uint8_t muse_note = muse_offset + SCALE[muse_clock_pulse()];
            if (muse_note != last_muse_note) {
                stop_note(compute_freq_for_midi_note(last_muse_note));
                play_note(compute_freq_for_midi_note(muse_note), 0xF);
                last_muse_note = muse_note;
            }
        }
        muse_counter = (muse_counter + 1) % muse_tempo;
    } else {
        if (muse_counter) {
            stop_all_notes();
            muse_counter = 0;
        }
    }
#endif
}

bool music_mask_user(uint16_t keycode) {
    switch (keycode) {
    case RAISE:
    case LOWER:
        return false;
    default:
        return true;
    }
}
