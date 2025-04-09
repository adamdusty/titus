#pragma once

#include <SDL3/SDL.h>

typedef struct CoreFrameInput {
    size_t count;
    SDL_Event events[255];
} CoreFrameInput;

typedef enum CORE_KEY_STATE {
    CORE_KEY_STATE_DOWN,
    CORE_KEY_STATE_UP,
} CORE_KEY_STATE;

typedef enum CORE_BUTTON_STATE {
    CORE_BUTTON_STATE_DOWN,
    CORE_BUTTON_STATE_UP,
} CORE_BUTTON_STATE;

typedef struct CoreFrameInputState {
    CORE_KEY_STATE* scancodes;        // input.scancode[SDL_SCANCODE_A]
    CORE_BUTTON_STATE* mouse_buttons; // input.mouse_buttons[0]
} CoreFrameInputState;

typedef struct CoreFrameInputComponent {
    CoreFrameInputState state[2]; // state[0] current, state[1] previous
} CoreFrameInputComponent;
