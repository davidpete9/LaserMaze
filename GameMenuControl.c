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
#include "GameStarter.h"
#include "debugmalloc.h"

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

    initializeFileWithLevel(btnId);

    return inGame;
}

   //a felirat hossza = level szamjegyinek szama + pont + '\0'
char * numberToLevelSign(int num) {
        int signLength = (int) floor(log10(num))+3;
        char * btn_title;
        btn_title = (char *) malloc(signLength*sizeof(char));
        sprintf(btn_title, "%d.", num);
        return btn_title;
}

/**
 * Elkészíti a szintek gombjait, lefogalalja nekik a szükséges memóriaterületet.
 * @return ButtonRect ** buttons
 * */
ButtonRect **createAllGameButton() {
    ButtonRect **gameBtns;
    gameBtns = (ButtonRect **) malloc(G_BTN_NUM * sizeof(ButtonRect));

    int k = 0;
    for (int i = 1; i <= BTN_ROWS; i++) {
        for (int j = 0; j < BTNS_IN_ROW; j++) {
        gameBtns[k] = initBtn((SDL_Rect) {
                                    left_margin+(j%BTNS_IN_ROW)*(LVL_BTN_W+btn_margin),
                                    top_margin+(i*(LVL_BTN_H+btn_margin))
                                    ,LVL_BTN_W, LVL_BTN_W}, numberToLevelSign(k+1), k+1);
        gameBtns[k]->btnTitle.isTitleMalloced = true;
        k++;
        }
    }

    gameBtns[G_BTN_NUM-1] = initBtn((SDL_Rect) {S_WIDTH/2-75,S_HEIGHT-100,150,50}, "Vissza a főmenübe", BACK_TO_MAIN_BTN);


    return gameBtns;
}


ButtonRect ** createInGameButtons() {
    ButtonRect **buttons;
    buttons = (ButtonRect **) malloc(IG_BTN_NUM * sizeof(ButtonRect));

    buttons[0] = initBtn((SDL_Rect) { 750, 50, 100, 50}, "Tűz", IG_FIRE_BTN );
    buttons[0]->col = (SDL_Color){0,120,120,255};

    buttons[1] = initBtn((SDL_Rect) { 10, 450, 100, 50}, "Kihagy", IG_SKIP_BTN);
    buttons[1]->col = (SDL_Color){100,100,100,255};

    buttons[2] = initBtn((SDL_Rect) { 10, 510, 100, 50}, "Reset", IG_RESET_BTN);
    buttons[2]->col = (SDL_Color){100,100,100,255};

    buttons[3] = initBtn((SDL_Rect) { 10, 570, 100, 50}, "Megállít", IG_PAUSE_BTN);
    buttons[3]->col = (SDL_Color){100,100,100,255};

    buttons[4] = initBtn((SDL_Rect) { 10, 630, 100, 50}, "Kilép", BACK_TO_MAIN_BTN);
    buttons[4]->col = (SDL_Color){100,100,100,255};

    return buttons;

}
