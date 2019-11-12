#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include "MenuEngine.h"
#include "Constanses.h"

ButtonRect **createSettingsMenuButtons() {

    ButtonRect **buttons;
    buttons = (ButtonRect **) malloc(S_BTN_NUM * sizeof(ButtonRect));

    buttons[0] = initBtn((SDL_Rect) {S_WIDTH / 2 - 75, S_HEIGHT / 2, 150, 50}, "Némítás", S_MUTE_BTN);
    buttons[1] = initBtn((SDL_Rect) {S_WIDTH / 2 - 75, S_HEIGHT / 2 + 75, 150, 50}, "Reset", S_RESET_BTN);
    buttons[2] = initBtn((SDL_Rect) {S_WIDTH / 2 - 75, S_HEIGHT / 2 + 150, 150, 50}, "Vissza a főmenübe", BACK_TO_MAIN_BTN);

    return buttons;
}
