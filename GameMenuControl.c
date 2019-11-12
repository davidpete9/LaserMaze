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

enum {
    LVL_BTN_W = 50,
    LVL_BTN_H = 50,
    BTNS_IN_ROW = 5,
    BTN_ROWS = LEVEL_NUM/BTNS_IN_ROW,
    btn_margin = 50,
    top_margin = 150,
    left_margin = (S_WIDTH-(BTNS_IN_ROW*(btn_margin+LVL_BTN_W)))/2+btn_margin/2
};

Page clickedOnLevel(int btnId) {
    //if not valid level just go to main menu by default
    if (btnId <= 0 || btnId > LEVEL_NUM) return mainMenu;
    initalizeActualGameFile(btnId);
    return inGame;
}

void initalizeActualGameFile(int level) {

}

/**
 * Elkészíti a szintek gombjait, lefogalalja nekik a szükséges memóriaterületet.
 * @return ButtonRect ** buttons
 * */

ButtonRect **createAllGameButton() {
    ButtonRect **gameBtns;
    gameBtns = (ButtonRect **) malloc(G_BTN_NUM * sizeof(ButtonRect));

    int k= 0;
    for (int i = 1; i <= BTN_ROWS; i++) {
        for (int j = 0; j < BTNS_IN_ROW; j++) {

        char * btn_title;
        //todo free
        btn_title = (char *) malloc(4*sizeof(char));
        sprintf(btn_title, "%d.", k+1);

        gameBtns[k++] = initBtn((SDL_Rect) {
                                    left_margin+(j%BTNS_IN_ROW)*(LVL_BTN_W+btn_margin),
                                    top_margin+(i*(LVL_BTN_H+btn_margin))
                                    ,LVL_BTN_W, LVL_BTN_W}, btn_title, i);
        }
    }

    gameBtns[G_BTN_NUM-1] = initBtn((SDL_Rect) {S_WIDTH/2-75,S_HEIGHT-100,150,50}, "Vissza a főmenübe", BACK_TO_MAIN_BTN);


    return gameBtns;
}


ButtonRect ** createInGameButtons() {
    ButtonRect **buttons;
    buttons = (ButtonRect **) malloc(IG_BTN_NUM * sizeof(ButtonRect));

    buttons[0] = initBtn((SDL_Rect) { 75, 300, 150, 50}, "Tűz!", IG_FIRE_BTN );
    buttons[1] = initBtn((SDL_Rect) { 75, 150, 150, 50}, "Kihagy", IG_SKIP_BTN);
    buttons[2] = initBtn((SDL_Rect) { 650, 250, 150, 50}, "Reset", IG_RESET_BTN);
    buttons[3] = initBtn((SDL_Rect) { 650, 150, 150, 50}, "Megállít", IG_PAUSE_BTN);
    buttons[4] = initBtn((SDL_Rect) { 650, 350, 150, 50}, "Kilép", BACK_TO_MAIN_BTN);

    return buttons;

}
