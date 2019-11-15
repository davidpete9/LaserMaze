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
#include "debugmalloc.h"

/**
 * Egy főmenübeli gombon való kattintást kezel le. A nextPage változóba jrja azt, hogy melyik a következő Page amit
 * be kell tölteni. (Vagy a Kilépés gombra kattintva leállítja a program futását.)
 * @param int buttonId
 * @return Page nextPage
 * */
Page getNextPageOnMainMenuClickOrQuit(int buttonId) {
    switch (buttonId) {
        case M_EXIT_BTN:
             exit(0);
        case M_GAME_BTN:
            return gameMenu;
        case M_SETTINGS_BTN:
            return settings;
        default:
            return mainMenu;
    }
}

/**
 * Elkészíti a főmenü gombjait, lefogalalja nekik a szükséges memóriaterületet.
 * @return ButtonRect ** buttons
 * */
ButtonRect **createMainMenuButtons() {

    ButtonRect **mainButtons;
    mainButtons = (ButtonRect **) malloc(M_BTN_NUM * sizeof(ButtonRect));

    mainButtons[0] = initBtn((SDL_Rect) {S_WIDTH / 2 - 75, S_HEIGHT / 2, 150, 50}, "Játék", M_GAME_BTN);
    mainButtons[1] = initBtn((SDL_Rect) {S_WIDTH / 2 - 75, S_HEIGHT / 2 + 75, 150, 50}, "Beállítások", M_SETTINGS_BTN);
    mainButtons[2] = initBtn((SDL_Rect) {S_WIDTH / 2 - 75, S_HEIGHT / 2 + 150, 150, 50}, "Kilépés", M_EXIT_BTN);

    return mainButtons;
}
