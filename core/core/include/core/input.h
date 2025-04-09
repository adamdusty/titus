#pragma once

#include <SDL3/SDL.h>
#include <stdint.h>

typedef enum CORE_KEY_STATE : char {
    CORE_KEY_STATE_DOWN = 1,
    CORE_KEY_STATE_UP   = 2,
} CORE_KEY_STATE;

typedef enum CORE_BUTTON_STATE : char {
    CORE_BUTTON_STATE_DOWN = 1,
    CORE_BUTTON_STATE_UP   = 2,
} CORE_BUTTON_STATE;

typedef struct CoreFrameInputState {
    CORE_KEY_STATE scancodes[SDL_SCANCODE_COUNT]; // input.scancode[SDL_SCANCODE_A]
    CORE_BUTTON_STATE mouse_buttons[UINT8_MAX];   // input.mouse_buttons[0]
} CoreFrameInputState;

typedef struct CoreFrameInput {
    CoreFrameInputState state[2]; // state[0] current, state[1] previous
} CoreFrameInput;
