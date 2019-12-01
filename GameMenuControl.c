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
#include "FileHandler.h"

enum {
    LVL_BTN_W = 50,
    LVL_BTN_H = 50,
    BTNS_IN_ROW = 5,
    STAR_H = 30,
    BTN_ROWS = LEVEL_NUM / BTNS_IN_ROW,
    btn_margin = 100,
    top_margin = 120,
    left_margin = (S_WIDTH - (BTNS_IN_ROW * (btn_margin + LVL_BTN_W))) / 2 + btn_margin / 2
};

/** Megnézi, hogy a levels.json fájlban melyik a legmagasabb sorszámú teljesített szint, visszatér az azt
 * követővel, az azt követő az a pálya amit még a játékos nem teljesített.
 * @return int currentLevel
 * */
int getCurrentLevel() {
    cJSON *doneLevels = getParsedJSONContentOfFile(LEVELS_FILE_NAME);
    if (doneLevels == NULL) return 1;
    int max = 1;
    for (int i = 0; i < cJSON_GetArraySize(doneLevels); i++) {
        int solvedLevel = cJSON_GetObjectItem(cJSON_GetArrayItem(doneLevels, i), LEVEL_STR)->valueint;
        if (solvedLevel > max) {
            max = solvedLevel;
        }
    }
    return max + 1;
}

cJSON *getResultsForCurrentLevel(int level) {
    cJSON *allResults = getParsedJSONContentOfFile(LEVELS_FILE_NAME);
    if (allResults == NULL) return NULL;
    for (int i = 0; i < cJSON_GetArraySize(allResults); i++) {
        cJSON *item = cJSON_GetArrayItem(allResults, i);
        if (cJSON_GetObjectItem(item, LEVEL_STR)->valueint == level) {
            return item;
        }
    }
    return NULL;
}

/**
 * Megnézei a levels.json fájl a alapján, hogy teljesítve van e már az adott level-e, vagy sem.
 * @param int level
 * @return bool isDone
 * */
bool isLevelAlreadyDone(int level) {
    return getResultsForCurrentLevel(level) != NULL;

}

/**Lekezeli, hogy mi történik, ha egy játékmenüben található gombra kattint a felhasználó.
 * Amennyiben nem érvényes gombra kattintott a főmenűbe irányít vissza. Érvényes szintre való kattintás
 * esetén előkészíti a leendő játékfájlt, melyben eltárolja a szintet.
 * @param int btnId
 * @return Page nextPageToGo
 * */
Page clickedOnLevel(int btnId) {
    if (btnId <= 0 || btnId > LEVEL_NUM) return mainMenu;

    if (isUserDeveloper() || isLevelAlreadyDone(btnId) || getCurrentLevel() == btnId) {
        if (getInitializedGameLevelIfExists() != btnId) initializeFileWithLevel(btnId);
        return inGame;
    }

    return gameMenu;
}

/** Amennyiben az adott pálya már teljesítve volt, vagy még nincs feloldva,
 * vagy éppem félbe lett hagyva különböző háttérszínteket kapnak a gombok.
 *  @param ButtonRect *btn
 **/
void refreshButtonColors(ButtonRect **btns) {
    cJSON *doneLevels = getParsedJSONContentOfFile(LEVELS_FILE_NAME);
    if (doneLevels != NULL) {

        for (int i = 0; i < LEVEL_NUM; i++) btns[i]->col = (SDL_Color) {100, 100, 100, 255};

        for (int i = 0; i < cJSON_GetArraySize(doneLevels); i++) {
            int solvedLevel = cJSON_GetObjectItem(cJSON_GetArrayItem(doneLevels, i), LEVEL_STR)->valueint;
            btns[solvedLevel - 1]->col = (SDL_Color) {0, 255, 0, 255};
        }


        cJSON *currentData = getParsedJSONContentOfFile(ACTUAL_STATUS_FILE_NAME);
        if (currentData != NULL && cJSON_GetObjectItem(currentData, IS_GAME_INITIALIZED)->valueint == 1) {
            int level = cJSON_GetObjectItem(currentData, LEVEL_STR)->valueint;
            btns[level - 1]->col = (SDL_Color) {255, 0, 0, 255};
        }
    }
    int current_level = getCurrentLevel();
    if (current_level <= LEVEL_NUM) {
        btns[current_level - 1]->col = (SDL_Color) {33, 124, 255, 255};
    }
}

/** Az adott szinthez, amennyiben teljesítve van kirajzolja az értékléseket csillagok formályában,
 *  A levels.json fájl tartalma szerint.
 * @param SDL_Renderer *renderer
 * @param ButtonRect **btns
 * */
void drawStarsForLevels(SDL_Renderer *renderer, ButtonRect **btns) {

    cJSON *levels = getParsedJSONContentOfFile(LEVELS_FILE_NAME);
    if (levels == NULL) return;

    SDL_Texture *star = IMG_LoadTexture(renderer, STAR_FILE_NAME);

    for (int i = 0; i < cJSON_GetArraySize(levels); i++) {
        cJSON *item = cJSON_GetArrayItem(levels, i);
        ButtonRect *current = btns[cJSON_GetObjectItem(item, LEVEL_STR)->valueint - 1];
        bool usedSkipBtn = cJSON_GetObjectItem(item, HAS_NO_SKIP_BADGE)->valueint ? true : false;
        bool hasTimeBonus = cJSON_GetObjectItem(item, HAS_TIME_BONUS_BADGE)->valueint ? true : false;
        SDL_Rect dest2 = (SDL_Rect) {current->rec.x + LVL_BTN_W / 2 - STAR_H / 2, current->rec.y - (STAR_H + 15),
                                     STAR_H, STAR_H};
        SDL_RenderCopy(renderer, star, NULL, &dest2);

        if (usedSkipBtn) {
            SDL_Rect dest1 = (SDL_Rect) {current->rec.x - STAR_H, current->rec.y - STAR_H, STAR_H, STAR_H};
            SDL_RenderCopy(renderer, star, NULL, &dest1);
        }
        if (hasTimeBonus) {
            SDL_Rect dest3 = (SDL_Rect) {current->rec.x + LVL_BTN_W, current->rec.y - STAR_H, STAR_H, STAR_H};
            SDL_RenderCopy(renderer, star, NULL, &dest3);
        }

    }
    SDL_DestroyTexture(star);
}

/**
 * Előkészíti a szintek gombjait a játékmenüben, úgy hogy a képernyő közepén sorokba legyenek rendezbe.
 *  lefogalalja nekik a szükséges memóriaterületet, a hívó felelőssége a memóriafelszabadítás.
 * @return ButtonRect ** buttons
 * */
ButtonRect **createAllGameButton() {
    ButtonRect **gameBtns;
    gameBtns = (ButtonRect **) malloc(G_BTN_NUM * sizeof(ButtonRect));

    int k = 0;
    for (int i = 0; i < BTN_ROWS; i++) {
        for (int j = 0; j < BTNS_IN_ROW; j++) {
            gameBtns[k] = initBtn((SDL_Rect) {
                                          left_margin + (j % BTNS_IN_ROW) * (LVL_BTN_W + btn_margin),
                                          top_margin + (i * (LVL_BTN_H + btn_margin)), LVL_BTN_W, LVL_BTN_W},
                                  generateFormattedStringFromNumber(k + 1, "%d."), k + 1);
            gameBtns[k]->btnTitle.isTitleMalloced = true;

            k++;
        }
    }
    gameBtns[G_BTN_NUM - 1] = initBtn((SDL_Rect) {S_WIDTH / 2 - 75, S_HEIGHT - 100, 150, 50}, "Vissza a főmenübe",
                                      BACK_TO_MAIN_BTN);
    return gameBtns;
}


/**
 *  Előkészíti a játékpályán lévő gombokat.
 *  lefogalalja nekik a szükséges memóriaterületet, a hívó felelőssége a memóriafelszabadítás.
 * @return ButtonRect ** buttons
 * */
ButtonRect **createInGameButtons() {
    ButtonRect **buttons;
    buttons = (ButtonRect **) malloc(IG_BTN_NUM * sizeof(ButtonRect));

    buttons[0] = initBtn((SDL_Rect) {10, 50, 100, 50}, "Tűz", IG_FIRE_BTN);
    buttons[0]->col = (SDL_Color) {0, 120, 120, 255};

    buttons[1] = initBtn((SDL_Rect) {10, 400, 100, 50}, "Kihagy", IG_SKIP_BTN);
    buttons[1]->col = (SDL_Color) {100, 100, 100, 255};

    buttons[2] = initBtn((SDL_Rect) {10, 460, 100, 50}, "Visszaállít", IG_RESET_BTN);
    buttons[2]->col = (SDL_Color) {100, 100, 100, 255};

    buttons[3] = initBtn((SDL_Rect) {10, 520, 100, 50}, "Kilép", IG_BACK_BTN);
    buttons[3]->col = (SDL_Color) {100, 100, 100, 255};

    return buttons;

}

/** A kapott szöveget a képernyőre írja, elkészíti a StringToDisplay struktúrát,
 * ami ehhez szükséges.
 * @param SDL_Renderer *renderer
 * @param SD_Rect pos
 * @param char * text
 * @parar int fontSize
 * */
void writeText(SDL_Renderer *renderer, SDL_Rect pos, char *text, int fontsize) {
    StringToDisplay msg;
    msg.fontSize = fontsize;
    msg.title = text;
    msg.titleColor = (SDL_Color) {255, 255, 255, 255};
    msg.pos = pos;

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderFillRect(renderer, &msg.pos);

    writeTextToDisplay(renderer, &msg);
}

/**Kirajzolja egy sikeresen teljesített szint utáni értékelési oldalt.
 * Kirajzolja, hogy milyen csillagokat kapott a játékkos, és kiiírja az időket.
 * @param SDL_Renderer *renderer
 * */
void drawResultsToFinishPage(SDL_Renderer *renderer) {

    SDL_Texture *star = IMG_LoadTexture(renderer, STAR_FILE_NAME);
    SDL_RenderCopy(renderer, star, NULL, &((SDL_Rect) {S_WIDTH / 2 - 40, 200, 80, 80}));

    writeText(renderer, (SDL_Rect) {S_WIDTH / 2 - 100, 300, 200, 50}, "Szint sikeresen teljesítve.", 20);

    cJSON *currentData = getParsedJSONContentOfFile(ACTUAL_STATUS_FILE_NAME);
    if (currentData == NULL) return;
    bool hasSkipBtnUsed = cJSON_GetObjectItem(currentData, USED_SKIP_BUTTON)->valueint == 1 ? true : false;
    if (!hasSkipBtnUsed) {
        writeText(renderer, (SDL_Rect) {S_WIDTH / 3 - 100, 350, 200, 50}, "Nincs kihagyott pálya!", 20);
        SDL_RenderCopy(renderer, star, NULL, &((SDL_Rect) {S_WIDTH / 3 - 40, 250, 80, 80}));
    }


    int currentTime = cJSON_GetObjectItem(currentData, ELAPSED_TIME_STR)->valueint;
    if (currentTime <= TIME_BADGE_GOT_BELOW && !hasSkipBtnUsed) {
        writeText(renderer, (SDL_Rect) {(2 * S_WIDTH) / 3 - 100, 350, 200, 50}, "30 mp alatt, kihagyás nélkül!", 20);
        SDL_RenderCopy(renderer, star, NULL, &((SDL_Rect) {(2 * S_WIDTH) / 3 - 40, 250, 80, 80}));
    }

    cJSON *result = getResultsForCurrentLevel(cJSON_GetObjectItem(currentData, LEVEL_STR)->valueint);
    if (result != NULL) {
        int recordTime = cJSON_GetObjectItem(result, ELAPSED_TIME_STR)->valueint;
        char *recordTimeStr = generateFormattedStringFromNumber(recordTime, "A rekordidő: %d mp");
        writeText(renderer, (SDL_Rect) {S_WIDTH / 3 - 100, 430, 200, 50}, recordTimeStr, 20);
        free(recordTimeStr);

        if (currentTime <= recordTime) {
            writeText(renderer, (SDL_Rect) {S_WIDTH / 2 - 150, 100, 300, 60}, "Új rekord!", 25);
        }
    }
    char *timeStr = generateFormattedStringFromNumber(currentTime, "Teljesítve %d mp alatt.");
    writeText(renderer, (SDL_Rect) {(2 * S_WIDTH) / 3 - 100, 430, 200, 50}, timeStr, 20);
    free(timeStr);

    cJSON_Delete(currentData);
    SDL_DestroyTexture(star);
}

/**
 * Kirajzolja azt az oldalt, amelyre a 15. szint teljesítése után kerülöünk.
 * @param SDL_Renderer *renderer
 * */
void drawGameFinishedPage(SDL_Renderer *renderer) {
    writeText(renderer, (SDL_Rect) {S_WIDTH / 2 - 250, 50, 500, 50}, "Gratulálok, hogy kijátszottad a játékot! :)", 20);
}
