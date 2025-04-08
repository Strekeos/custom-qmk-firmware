/* Copyright 2021 Glorious, LLC <salman@pcgamingrace.com>
 * Modified 2022 by rustedaperture for qmk_firmware
 * Further Modified 2025 by Strekeos

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include QMK_KEYBOARD_H
#include "analog.h"
#include "string.h"
#include "qmk_midi.h"
#include "joystick.h"

enum layers {
    _LAYER0,
    _LAYER1,
    _LAYER2,
};

// Set up EEPROM. Mostly boilerplate from QMK docs
typedef union {
    uint32_t raw;
    struct {
        uint8_t slider_func_state : 8; // Hold only the slider function state. Will be a low value integer, so 8 bits will be used to store.
        uint8_t slider_smoothing_state : 8; // Hold 0-4 for the slider smoothing value
        uint8_t midi_cc_channel_state : 8; // Hold a 0 or a 1 for whether the slider MIDI CC channel will change by layer
    };
} user_config_t;
user_config_t user_config; 

// Slider Variable init
uint8_t slider_func = 0;
uint8_t midi_cc_channel = 0x3E; //Default midi cc channel
uint8_t slider_smoothing = 0;

// NOTE: For rescale parameter 0x7C, minimum and maximum values for 'slider_value' are -115 and 119 respectively.
// We let calibration software on the host to rescale them perfectly to [-127, 127].
#define RESCALE_PARAM       0x7C
int8_t slider_reading;

// On keyboard initialization, pull the EEPROM values
void keyboard_post_init_user(void) {
    // Call the keymap level matrix init.
    user_config.raw = eeconfig_read_user();
    // Initialize the default values from EEPROM
    slider_func = user_config.slider_func_state;
    midi_cc_channel = user_config.midi_cc_channel_state;
    slider_smoothing = user_config.slider_smoothing_state;
	slider_reading = (int8_t)(analogReadPin(SLIDER_PIN) >> 3);
};

// When the EEPROM gets forcefully reset, set the initialization value
void eeconfig_init_user(void) {
    user_config.raw = 0;
    user_config.slider_func_state = 1;
    user_config.midi_cc_channel_state = 0x3F;
    user_config.slider_smoothing_state = 0;
    eeconfig_update_user(user_config.raw);
}

// ===================================
// ======= MATRIX AND ENCODER ========
// ===================================

#if defined(ENCODER_MAP_ENABLE)
const uint16_t PROGMEM encoder_map[][NUM_ENCODERS][2] = {
    [_LAYER0] = { ENCODER_CCW_CW(KC_VOLD, KC_VOLU)},
    [_LAYER1] = { ENCODER_CCW_CW(KC_VOLD, KC_VOLU)},
    [_LAYER2] = { ENCODER_CCW_CW(KC_VOLD, KC_VOLU)}
};
#endif

// Tap Dance declarations
enum {
    TD_SLS_LPRN,TD_AST_RPRN,TD_MNS_ESC,TD_PLS_DEL
};

// Tap Dance definitions
tap_dance_action_t tap_dance_actions[] = {
 
    // Tap once for '/', twice for '('
    [TD_SLS_LPRN] = ACTION_TAP_DANCE_DOUBLE(KC_PSLS, LSFT(KC_8)),
 
    // Tap once for '*', twice for ')'
    [TD_AST_RPRN] = ACTION_TAP_DANCE_DOUBLE(KC_PAST, LSFT(KC_9)),
 
    // Tap once for '-', twice for 'Escape' (used to Clear All in Calculator)
    [TD_MNS_ESC] = ACTION_TAP_DANCE_DOUBLE(KC_PMNS, KC_ESC),
 
    // Tap once for '+', twice for 'Delete' (used to Clear Entry in Calculator)
    [TD_PLS_DEL] = ACTION_TAP_DANCE_DOUBLE(KC_PPLS, KC_DEL)
};

// Defines the keycodes used by our macros in process_record_user
enum custom_keycodes { // In order to add more volume controls, add here a custom keycode, ex: VLC
    MIDI_62= SAFE_RANGE,MIDI_90,MIDI_89,MIDI_88,MIDI_87,MIDI_86,MIDI_85,SF_0,SF_1,SF_2,SF_3,SF_4//,VLC
};

bool process_record_user(uint16_t keycode, keyrecord_t *record) { // In order to add more volume controls, add here code to detect custom keycode, ex: VLC
    switch (keycode) {
        case MIDI_62:
            if (record->event.pressed) {
                midi_cc_channel= 62;
            } else {
            }
            break;
        
        case MIDI_90:
            if (record->event.pressed) {
                midi_cc_channel= 90;
            } else {
            }
            break;
        
        case MIDI_89:
            if (record->event.pressed) {
                midi_cc_channel= 89;
            } else {
            }
            break;
        
        case MIDI_88:
            if (record->event.pressed) {
                midi_cc_channel= 88;
            } else {
            }
            break;
        
        case MIDI_87:
            if (record->event.pressed) {
                midi_cc_channel= 87;
            } else {
            }
            break;
        
        case MIDI_86:
            if (record->event.pressed) {
                // when keycode CHROME is pressed
                midi_cc_channel= 86;
            } else {
            }
                    
            break;
        case MIDI_85:
            if (record->event.pressed) {
                midi_cc_channel= 85;
            } else {
            }
            break;
        
        case SF_0:
            if (record->event.pressed) {
                slider_func= 0;
            } else {
            }
            break;

        case SF_1:
            if (record->event.pressed) {
                slider_func= 1;
            } else {
            }
            break;

        case SF_2:
            if (record->event.pressed) {
                slider_func= 2;
            } else {
            }
            break;

        case SF_3:
            if (record->event.pressed) {
                slider_func= 3;
            } else {
            }
            break;

        case SF_4:
            if (record->event.pressed) {
                slider_func= 4;
            } else {
            }
            break;
    }
    return true;
}

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

//  NUM                / or (             * or )            -
//  7                  8                  9                 +
//  4                  5                  6                 ENCODER
//  1                  2                  3                 RET
//  0                                                       .

  [0] = LAYOUT(
    MO(1),             TD(TD_SLS_LPRN),   TD(TD_AST_RPRN),   TD(TD_MNS_ESC),
    KC_P7,             KC_P8,             KC_P9,             TD(TD_PLS_DEL),
    KC_P4,             KC_P5,             KC_P6,             TO(1),
    KC_P1,             KC_P2,             KC_P3,             KC_PENT,
    KC_P0,                                                   KC_PDOT
  ),
  
//  Auxiliary Layer
//  NUM                MEDIA_PREV          MEDIA_NEXT        MEDIA_PLAY_PAUSE
//  RGB_HUE+           RGB_BRIGHT+         RGB_SAT+          MEDIA_STOP
//  <RGB_MODE          RGB_TOGGLE          RGB_MODE>         ENCODER
//  RGB_HUE-           RGB_BRIGHT-         RGB_SAT-          MEDIA_PLAYER
//  SYS_SLEEP                                                KBD_BOOTLD_MODE
 
  [1] = LAYOUT(
    MO(1),             KC_MPRV,            KC_MNXT,          KC_MPLY,
    RGB_HUI,           RGB_VAI,            RGB_SAI,          KC_MSTP,
    RGB_RMOD,          RGB_TOG,            RGB_MOD,          TO(2),
    RGB_HUD,           RGB_VAD,            RGB_SAD,          KC_MSEL,
    KC_SLEP,                                                 QK_BOOT
  ),

//  Midi&Slider Layer
//  NUM                MEDIA_PREV          MEDIA_NEXT        MEDIA_PLAY_PAUSE
//  Slider Function 0  Slider Function 1   Slider Function 3 MEDIA_STOP
//  CC 87              CC 86               CC 85             ENCODER
//  CC 90              CC 89               CC 88             MEDIA_PLAYER
//  CC 62                                                    KBD_BOOTLD_MODE
 
  [2] = LAYOUT(
    KC_MUTE,           KC_MPRV,            KC_MNXT,          KC_MPLY,
    SF_1,              SF_0,               SF_3,             KC_MSTP,
    MIDI_87,           MIDI_86,            MIDI_85,          TO(0),
    MIDI_90,           MIDI_89,            MIDI_88,          KC_MSEL,
    MIDI_62,                                                 QK_BOOT
  )

};

// -----------------

// ====== Process VIA Custom UI ======
// Establish enums for the multiple possible configurations of ids that could arise.
// Each unique VIA configuration requires a separate enum value, which is the unique id
// that is used to define that value in the other functions throughout.
enum via_slider_control {
    id_slider_func = 1,
    id_midi_cc_channel = 2,
    id_slider_smoothing = 3,
};


// For Midi
extern MidiDevice midi_device;

// Define the function to set the slider value
void slider_func_set_value ( uint8_t *data ) {
    uint8_t *value_id   = &(data[0]);
    uint8_t *value_data = &(data[1]);

    switch ( *value_id )
    {
        case id_slider_func:
        {
            slider_func = *value_data;
            user_config.slider_func_state = *value_data;
            eeconfig_update_user(user_config.raw);
            break;
        }

        case id_midi_cc_channel:
        {
            midi_cc_channel = *value_data;
            user_config.midi_cc_channel_state = *value_data;
            eeconfig_update_user(user_config.raw);
            break;
        }

        case id_slider_smoothing:
        {
            slider_smoothing = *value_data;
            user_config.slider_smoothing_state = *value_data;
            eeconfig_update_user(user_config.raw);
            break;			
        }
    }
}

// Define the function to read the slider value [so that VIA UI knows what to display]
void slider_func_get_value( uint8_t *data ) {
    uint8_t *value_id   = &(data[0]);
    uint8_t *value_data = &(data[1]);

    switch ( *value_id )
    {
        case id_slider_func:
        {
            *value_data = slider_func;
            break;
        }

        case id_midi_cc_channel:
        {
            *value_data = midi_cc_channel;
            break;
        }

        case id_slider_smoothing:
        {
            *value_data = slider_smoothing;
            break;
        }
    }
}

// Boilerplate from VIA Custom UI Documentation.
void via_custom_value_command_kb(uint8_t *data, uint8_t length) {
    uint8_t *command_id        = &(data[0]);
    uint8_t *channel_id        = &(data[1]);
    uint8_t *value_id_and_data = &(data[2]);

    if ( *channel_id == id_custom_channel ) {
        switch ( *command_id )
        {
            case id_custom_set_value:
            {
                slider_func_set_value(value_id_and_data);
                break;
            }
            case id_custom_get_value:
            {
                slider_func_get_value(value_id_and_data);
                break;
            }
            case id_custom_save:
            {
                //buttglow_config_save(); // Not doing this
                break;
            }
            default:
            {
                *command_id = id_unhandled;
                break;
            }
        }
        return;
    }

    *command_id = id_unhandled;

    // DO NOT call raw_hid_send(data,length) here, let caller do this
}

// ==================================
// ====== MIDI Slider controls ======
// ==================================

// For RGB Matrix Brightness
uint8_t last_val = 0;
uint8_t rgb_hue = 0;
uint8_t rgb_sat = 0;
// For Slider Smoothing
uint16_t accumulator = 0;
uint8_t current_val = 0;
// For Joystick and Driving
uint8_t divisor = 0;

void slider(void) {

    if (divisor++) { // only run the slider function 1/256 times it's called
        return;
    }

    // Slider smoothing algorithm
    if (slider_smoothing == 0) {
        current_val = analogReadPin(SLIDER_PIN) >> 3; // Bit shift 2^10 [1024] to 2^7 [128]
    } else {
        uint8_t raw_val = analogReadPin(SLIDER_PIN) >> 3;
        accumulator = accumulator - (accumulator >> slider_smoothing) + raw_val;
        current_val = accumulator >> slider_smoothing;
    }
    
    // Slider Functions
    if (last_val - current_val < -1 || last_val - current_val > 1) { 

        // Underglow RGB Brightness
        if ( slider_func == 0 ) {
            uint8_t rounded_val = 0;
            uint8_t round_coefficient = 5; // Set round-to-zero value target
            rgb_hue = rgb_matrix_get_hue(); // Pull current hue and saturation values since we're just adjusting brightness
            rgb_sat = rgb_matrix_get_sat();

            // Since the lower end range of the slider can be a little bit noisy, it's going to make the zero-value a little hard to hit when it bounces around between 0-1-2. Better off to round any super low values to zero so the lights will affirmatively turn off at lower values.
            if ( current_val < round_coefficient ) {
                rounded_val = 0;
            } else {
                rounded_val = current_val;
            }

            rgb_matrix_sethsv(rgb_hue, rgb_sat, (rounded_val * 2 > 255) ? 255 : (rounded_val * 2)); //rgb_matrix_sethsv(rgb_hue, rgb_sat, (rounded_val * 2); // At 8 bits, it's going to be 0-128, so double to get full range (-1 For GMMK Numpad at full value RGB starts lagging).
        }

        // MIDI CC Channel Data
        else if (slider_func == 1) {
					midi_send_cc(&midi_device, 2,  midi_cc_channel, 0x7F + current_val);
				}

            
        // Layer shift between layers 0 - 2 
        else if ( slider_func == 2 ) {
            layer_move(current_val >>6); // Bit shift 2^7 to 2^2
        }
	
		// Joystick
        else if (slider_func == 3) {
			// We maintain a rolling average to reduce jitter
			slider_reading = (slider_reading >> 1) + (int8_t)(analogReadPin(SLIDER_PIN) >> 4);

            int8_t slider_value = ((RESCALE_PARAM - slider_reading) << 1) - 0x7F;

            joystick_set_axis(0, slider_value);
        }

		// Reverse Joystick
        else if (slider_func == 4) {
			// We maintain a rolling average to reduce jitter
			slider_reading = (slider_reading >> 1) + (int8_t)(analogReadPin(SLIDER_PIN) >> 4);

            int8_t slider_value = ((RESCALE_PARAM - slider_reading) << 1) - 0x7F;

            joystick_set_axis(0, -slider_value);
        }
	
	    else {
            return;
        }
	
        last_val = current_val;
    }
}

void housekeeping_task_user(void) {
    slider();
}

// ---------------------------------

// Enable VIA encoder support
bool encoder_update_user(uint8_t index, bool clockwise) {
    if (clockwise) {
        tap_code(KC_VOLU); // Example: Volume Up
    } else {
        tap_code(KC_VOLD); // Example: Volume Down
    }
    return true; // Ensures the function always returns a value
}

// ---------------------------------