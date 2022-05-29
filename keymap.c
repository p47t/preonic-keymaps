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

#include "audio.h"
#include "keycode.h"
#include QMK_KEYBOARD_H
#include "muse.h"
#include "macros.h"

// Tap dance

typedef enum {
    TAP_STATE_RESET,
    SINGLE_TAP,
    SINGLE_HOLD,
    DOUBLE_TAP,
    DOUBLE_HOLD,
    TRIPLE_TAP,
    TRIPLE_HOLD,
    SUCCESSIVE_TAP,
    SUCCESSIVE_HOLD,
} tap_state_t;

// Global TapDance State
static struct t_tap {
    tap_state_t quotes;
    tap_state_t grave;
    tap_state_t magic;
} qk_tap_state = {
    .quotes = 0,
    .grave = 0,
    .magic = 0,
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
    } else {
        if (state->interrupted || !state->pressed) {
            return SUCCESSIVE_TAP;
        } else {
            return SUCCESSIVE_HOLD;
        }
    }
}

void td_quotes_finished(qk_tap_dance_state_t *state, void *user_data) {
    qk_tap_state.quotes = get_tapdance_state(state);
    switch (qk_tap_state.quotes) {
        case SINGLE_TAP:
        case SINGLE_HOLD:
        case SUCCESSIVE_TAP:
        case SUCCESSIVE_HOLD:
            register_code16(KC_QUOTE);
            break;
        case DOUBLE_TAP:
        case DOUBLE_HOLD:
            register_code16(KC_DOUBLE_QUOTE);
            break;
        case TRIPLE_TAP:
        case TRIPLE_HOLD:
            register_code16(KC_PIPE);
        default:
            break;
    }
}

void td_quotes_reset(qk_tap_dance_state_t *state, void *user_data) {
    switch (qk_tap_state.quotes) {
        case SINGLE_TAP:
        case SINGLE_HOLD:
        case SUCCESSIVE_TAP:
        case SUCCESSIVE_HOLD:
            unregister_code16(KC_QUOTE);
            break;
        case DOUBLE_TAP:
        case DOUBLE_HOLD:
            unregister_code16(KC_DOUBLE_QUOTE);
            break;
        case TRIPLE_TAP:
        case TRIPLE_HOLD:
            unregister_code16(KC_PIPE);
            break;
        default:
            break;
    }
    qk_tap_state.quotes = 0;
}

void td_grave_finished(qk_tap_dance_state_t *state, void *user_data) {
    qk_tap_state.grave = get_tapdance_state(state);
    switch (qk_tap_state.grave) {
        case SINGLE_TAP:
        case SINGLE_HOLD:
        case SUCCESSIVE_TAP:
        case SUCCESSIVE_HOLD:
            register_code16(KC_GRV);
            break;
        case DOUBLE_TAP:
        case DOUBLE_HOLD:
            register_code16(KC_TILDE);
            break;
        case TRIPLE_TAP:
        case TRIPLE_HOLD:
            SEND_STRING("```");
            break;
        default:
            break;
    }
}

void td_grave_reset(qk_tap_dance_state_t *state, void *user_data) {
    switch (qk_tap_state.grave) {
        case SINGLE_TAP:
        case SINGLE_HOLD:
        case SUCCESSIVE_TAP:
        case SUCCESSIVE_HOLD:
            unregister_code16(KC_GRV);
            break;
        case DOUBLE_TAP:
        case DOUBLE_HOLD:
            unregister_code16(KC_TILDE);
            break;
        case TRIPLE_TAP:
        case TRIPLE_HOLD:
            break;
        default:
            break;
    }
    qk_tap_state.grave = 0;
}

void td_magic_finished(qk_tap_dance_state_t *state, void *user_data) {
    qk_tap_state.magic = get_tapdance_state(state);
    switch (qk_tap_state.magic) {
        case SINGLE_TAP: {
            SEND_STRING(SS_LSFT(SS_LCTL(SS_LALT(SS_LGUI("x"))))); // Hyper-X
            break;
        }
        case DOUBLE_TAP: {
            SEND_STRING(SS_LSFT(SS_LCTL(SS_LALT(SS_LGUI("y"))))); // Hyper-Y
            break;
        }
        case TRIPLE_TAP: {
            SEND_STRING(SS_LSFT(SS_LCTL(SS_LALT(SS_LGUI("z"))))); // Hyper-Z
            break;
        }
        case SINGLE_HOLD:
        case DOUBLE_HOLD:
        case TRIPLE_HOLD:
        case SUCCESSIVE_HOLD:
            register_mods(MOD_BIT(KC_LCTL));
            break;
        default:
            break;
    }
}

void td_magic_reset(qk_tap_dance_state_t *state, void *user_data) {
    switch (qk_tap_state.magic) {
        case SINGLE_TAP:
        case DOUBLE_TAP:
        case TRIPLE_TAP:
        case SUCCESSIVE_TAP:
            break;
        case SINGLE_HOLD:
        case DOUBLE_HOLD:
        case TRIPLE_HOLD:
        case SUCCESSIVE_HOLD:
            unregister_mods(MOD_BIT(KC_LCTL));
            break;
        default:
            break;
    }
    qk_tap_state.magic = 0;
}

// Tap Dance Declarations
enum tapdance_keycodes {
    TD_QUOTES,
    TD_GRAVE,
    TD_APP_MAGIC,
    TD_5_F5,
    TD_7_F7,
    TD_8_F8,
    TD_9_F9,
};

// Tap Dance Definitions
qk_tap_dance_action_t tap_dance_actions[] = {
    // once: single quote, twice: double quote, thrice: backtick
    [TD_QUOTES] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, td_quotes_finished, td_quotes_reset),

    // once: `, twice: ~, thrice: `|`
    [TD_GRAVE] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, td_grave_finished, td_grave_reset),

    // once: mute, hold: Ctrl
    [TD_APP_MAGIC] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, td_magic_finished, td_magic_reset),

    [TD_5_F5] = ACTION_TAP_DANCE_DOUBLE(KC_5, KC_F5),
    [TD_7_F7] = ACTION_TAP_DANCE_DOUBLE(KC_7, KC_F7),
    [TD_8_F8] = ACTION_TAP_DANCE_DOUBLE(KC_8, KC_F8),
    [TD_9_F9] = ACTION_TAP_DANCE_DOUBLE(KC_9, KC_F9),
};

#define TD_QUOT TD(TD_QUOTES)
#define TD_GRV  TD(TD_GRAVE)
#define TD_5    TD(TD_5_F5)
#define TD_7    TD(TD_7_F7)
#define TD_8    TD(TD_8_F8)
#define TD_9    TD(TD_9_F9)
#define TD_MAGIC TD(TD_APP_MAGIC)

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
    TRAILING_SEMICOLON,
    TRAILING_COMMA,
    MACKEY1,
    MACKEY2,
    MACKEY3,
    MACKEY4,
    MACKEY5,
};

#define MT_LSFT MT(MOD_LSFT, KC_MINUS)
#define MT_RSFT MT(MOD_RSFT, KC_EQUAL)
#define MT_LCTL MT(MOD_LCTL, KC_LPRN)
#define MT_LALT MT(MOD_LALT, KC_LBRC)
#define MT_LGUI MT(MOD_LGUI, KC_LCBR)

#define OS_LSFT OSM(MOD_LSFT)
#define OS_RSFT OSM(MOD_RSFT)
#define OS_LCTL OSM(MOD_LCTL)
#define OS_LALT OSM(MOD_LALT)
#define OS_LGUI OSM(MOD_LGUI)

#define TR_SCLN TRAILING_SEMICOLON
#define TR_COMM TRAILING_COMMA

// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

/* Qwerty
 * ,-----------------------------------------------------------------------------------------------.
 * |  Esc  |   1   |   2   |   3   |   4   |   5   |   6   |   7   |   8   |   9   |   0   | Bksp  |
 * |-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------|
 * |  Tab  |   Q   |   W   |   E   |   R   |   T   |   Y   |   U   |   I   |   O   |   P   |  '"|  |
 * |-------+-------+-------+-------+-------+---------------+-------+-------+-------+-------+-------|
 * |   `   |   A   |   S   |   D   |   F   |   G   |   H   |   J   |   K   |   L   |   ;   | Enter |
 * |-------+-------+-------+-------+-------+-------|-------+-------+-------+-------+-------+-------|
 * | Shift |   Z   |   X   |   C   |   V   |   B   |   N   |   M   |   ,   |   .   |   /   | Shift |
 * |-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------|
 * | Mute  | Ctrl (| Alt [ | GUI { |Lower  |     Space     | Raise | Left  | Down  |  Up   | Right |
 * `-----------------------------------------------------------------------------------------------'
 */
[LAYER_QWERTY] = LAYOUT_preonic_grid(
    KC_ESC,   KC_1,    KC_2,    KC_3,    KC_4,    TD_5,    KC_6,    TD_7,    TD_8,    TD_9,    KC_0,    KC_BSPC,
    KC_TAB,   KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    TD_QUOT,
    TD_GRV,   KC_A,    KC_S,    KC_D,    KC_F,    KC_G,    KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_ENT,
    KC_LSFT,  KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH, OS_RSFT,
    TD_MAGIC, KC_LCTL, KC_LALT, KC_LGUI, LOWER,   KC_SPC,  KC_SPC,  RAISE,   KC_LEFT, KC_DOWN, KC_UP,   KC_RGHT
),

/* Lower
 * ,-----------------------------------------------------------------------------------------------.
 * |       | Macro | Macro | Macro | Macro | Macro |       |       |       |       |       |  Del  |
 * |-------+-------+-------+-------+-------+---------------+-------+-------+-------+-------+-------|
 * |  Esc  |   B1  |   MU  |  B2   |       |       |       |   (   |   )   |       |   +   | Bksc  |
 * |-------+-------+-------+-------+-------+---------------+-------+-------+-------+-------+-------|
 * |   ~   |   ML  |   MD  |  MR   |       |       |       |   {   |   }   |   =   |       | End+; |
 * |-------+-------+-------+-------+-------+-------|-------+-------+-------+-------+-------+-------|
 * |       |   WL  |   WD  |   WU  |  WR   |       |       |   [   |   ]   |   >   |   \   | End+, |
 * |-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------|
 * |       |       |       |       |       |               |       | Home  | Pg Dn | Pg Up |  End |
 * `-----------------------------------------------------------------------------------------------'
 */
[LAYER_LOWER] = LAYOUT_preonic_grid(
    _______, MACKEY1, MACKEY2, MACKEY3, MACKEY4, MACKEY5, _______, _______, _______, _______, _______, KC_DEL,
    KC_ESC,  KC_BTN1, KC_MS_U, KC_BTN2, _______, _______, _______, KC_LPRN, KC_RPRN, _______, KC_PLUS, KC_BSPC,
    KC_TILD, KC_MS_L, KC_MS_D, KC_MS_R, _______, _______, _______, KC_LCBR, KC_RCBR, KC_EQL,  _______, TR_SCLN,
    _______, KC_WH_L, KC_WH_D, KC_WH_U, KC_WH_R, _______, _______, KC_LBRC, KC_RBRC, KC_RABK, KC_BSLS, TR_COMM,
    _______, KC_ACL0, KC_ACL1, KC_ACL2, _______, KC_MINS, KC_MINS, _______, KC_HOME, KC_PGDN, KC_PGUP, KC_END
),

#define CMD_Z LGUI(KC_Z)
#define CMD_X LGUI(KC_X)
#define CMD_C LGUI(KC_C)
#define CMD_V LGUI(KC_V)

/* Raise
 * ,-----------------------------------------------------------------------------------------------.
 * |       |  F1   |  F2   |  F3   |  F4   |  F5   |  F6   |  F7   |  F8   |  F9   | F10   |  Del  |
 * |-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------|
 * |  Esc  |  F11  |  F12  |  F13  |  F14  |  F15  |  F16  |  F17  |  F18  |  F19  | F20   | Bksc  |
 * |-------+-------+-------+-------+-------+---------------+-------+-------+-------+-------+-------|
 * |   `   |       |       |       |       |       | Left  | Down  |  Up   | Right |       |       |
 * |-------+-------+-------+-------+-------+-------|-------+-------+-------+-------+-------+-------|
 * |       | Cmd-Z | Cmd-X | Cmd-C | Cmd-V |       |       | Home  | Pg Dn | Pg Up |   |   |       |
 * |-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------|
 * |       |       |       |       |       |               |       | Mute  | Vol-  | Vol+  |  Play |
 * `-----------------------------------------------------------------------------------------------'
 */
[LAYER_RAISE] = LAYOUT_preonic_grid(
    _______, KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_DEL,
    KC_ESC,  KC_F11,  KC_F12,  KC_F13,  KC_F14,  KC_F15,  KC_F16,  KC_F17,  KC_F18,  KC_F19,  KC_F20,  KC_BSPC,
    KC_GRV,  _______, _______, _______, _______, _______, KC_LEFT, KC_DOWN, KC_UP,   KC_RGHT, _______, _______,
    _______, CMD_Z,   CMD_X,   CMD_C,   CMD_V,   _______, _______, KC_HOME, KC_PGDN, KC_PGUP, KC_PIPE, _______,
    _______, _______, _______, _______, _______, KC_UNDS, KC_UNDS, _______, KC_MUTE, KC_VOLD, KC_VOLU, KC_MPLY
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
 * | Brite |       |       |       |       |               |       |       | DT DN | DT UP | DT PR |
 * `-----------------------------------------------------------------------------------------------'
 */
[LAYER_ADJUST] = LAYOUT_preonic_grid(
    KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,   KC_F7,   KC_F8,   KC_F9,    KC_F10,  KC_F11,  KC_F12,
    _______, RESET,   DEBUG,   _______, _______, _______, _______, TERM_ON, TERM_OFF, _______, _______, _______,
    _______, _______, MU_MOD,  AU_ON,   AU_OFF,  AG_NORM, AG_SWAP, _______, _______,  _______, _______, _______,
    _______, MUV_DE,  MUV_IN,  MU_ON,   MU_OFF,  MI_ON,   MI_OFF,  _______, _______,  _______, _______, _______,
    BACKLIT, _______, _______, _______, _______, _______, _______, _______, _______,  DT_DOWN, DT_UP,   DT_PRNT
)

}; // keymaps
// clang-format on

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
    case TRAILING_SEMICOLON:
        if (record->event.pressed) {
            SEND_STRING(SS_TAP(X_END)";");
            return false;
        }
        break;
    case TRAILING_COMMA:
        if (record->event.pressed) {
            SEND_STRING(SS_TAP(X_END)",");
            return false;
        }
        break;
    case MACKEY1:
        if (record->event.pressed) {
           SEND_STRING(MACRO_STRING1);
           return false;
        }
        break;
    case MACKEY2:
        if (record->event.pressed) {
           SEND_STRING(MACRO_STRING2);
           return false;
        }
        break;
    case MACKEY3:
        if (record->event.pressed) {
           SEND_STRING(MACRO_STRING3);
           return false;
        }
        break;
    case MACKEY4:
        if (record->event.pressed) {
           SEND_STRING(MACRO_STRING4);
           return false;
        }
        break;
    case MACKEY5:
        if (record->event.pressed) {
           SEND_STRING(MACRO_STRING5);
           return false;
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
