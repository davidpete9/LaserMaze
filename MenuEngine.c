#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include "MenuEngine.h"
#include "GameMenuControl.h"
#include "Constanses.h"
#include "GameStarter.h"
#include "debugmalloc.h"

const double DEFAULT_PADDING_RATIO = 0.02;
const SDL_Color DEFAULT_TEXT_COLOR = {255, 255, 255, 255};
const SDL_Color DEFAULT_BUTTON_COLOR = {255, 0, 0, 255};
const SDL_Color DEFAULT_BACKGROUND_COLOR = {0, 0, 0, 255};

/** Megnézi, hogy az aktuális oldalon hány gomb van a gombok tömbjében.
 * Kikeresi a Constanses.c fájlban deklarált konstansok közül
 * @param Page currentPage
 * @return int length
 * */
int getCurrentButtonArraySize(Page currentPage) {
    switch (currentPage) {
        case mainMenu:
            return M_BTN_NUM;
        case settings:
            return S_BTN_NUM;
        case inGame:
            return IG_BTN_NUM;
        case gameMenu:
            return G_BTN_NUM;
        case levelFinished:
            return LEVEL_FINISHED_BTN_NUM;
        case gameFinished:
            return GAME_FINISHED_BTN_NUM;
        default:
            return 0;
    }
}

/**
 * Leírja a képernyőre az adott szöveget.
 * @param SDL_Renderer *renderer
 * @param const StringToDisplay * strObj
 * */
void writeTextToDisplay(SDL_Renderer *renderer, const StringToDisplay *strObj) {

    TTF_Font *font = TTF_OpenFont("LiberationSerif-Regular.ttf", strObj->fontSize);
    if (!font) {
        SDL_Log("Nem sikerult megnyitni a fontot! %s\n", TTF_GetError());
        exit(1);
    }

    SDL_Surface *sign;
    sign = TTF_RenderUTF8_Blended(font, strObj->title, strObj->titleColor);

    SDL_Texture *sign_t;
    sign_t = SDL_CreateTextureFromSurface(renderer, sign);
    printf("x=%d, y=%d, w=%d, h=%d", strObj->pos.x, strObj->pos.y, strObj->pos.w, strObj->pos.h);
    SDL_RenderCopy(renderer, sign_t, NULL, &strObj->pos);

    SDL_FreeSurface(sign);
    SDL_DestroyTexture(sign);

    TTF_CloseFont(font);
}

/**
 * Kirajzolja az adott gombot a képernyőre
 * @param SDL_Renderer * render
 * @param ButtonRect *btn
 * */
void drawButton(SDL_Renderer *renderer, ButtonRect *btn) {

    SDL_SetRenderDrawColor(renderer, btn->col.r, btn->col.g, btn->col.b, btn->col.a);
    SDL_RenderFillRect(renderer, &btn->rec);
    writeTextToDisplay(renderer, &btn->btnTitle);
}

/**
 * Létrehoz egy Button típusú struktúrát, alapértelmezetten beállítja a gomb színét, betűméretét, és a szöveget pozicionálja a gombon belül.
 * @param SDL_Rect pos
 * @param char *title
 * @param ButtonId id
 * @return ButtonRect * btn
 * */
ButtonRect *initBtn(SDL_Rect pos, char *title, int id) {
    ButtonRect *btn;
    btn = (ButtonRect *) malloc(sizeof(ButtonRect));
    btn->rec = pos;
    btn->col = DEFAULT_BUTTON_COLOR;
    btn->btnId = id;

    btn->btnTitle.title = title;
    btn->btnTitle.fontSize = calculateSuitableFontSizeInBtn(pos);
    btn->btnTitle.titleColor = DEFAULT_TEXT_COLOR;
    btn->btnTitle.isTitleMalloced = false;

    setBtnTitlePos(&btn->btnTitle.pos, pos);

    return btn;
}


/**
 * Kiszámolja és beállítja, hogy hol legyen az a négyszög, amelybe majd a gomb szövegét kell írni.
 * Az adott négyszög kisebb mint az eredeti, mivel kihagy egy kevés margót minden szélen.
 * @param SDL_Rect * pos
  * */
void setBtnTitlePos(SDL_Rect *strPos, SDL_Rect pos) {
    double padding_ratio = DEFAULT_PADDING_RATIO / 2.0;
    *strPos = (SDL_Rect) {(int) pos.x * (1 + padding_ratio), (int) pos.y * (1 - padding_ratio),
                          (int) pos.w * (1 - padding_ratio), (int) pos.h * (1 - padding_ratio)};

}

/**
 * Kiszámolja, hogy egy adott méretű gombba melyik a legmegelelőbb betűméret.
 * @param SDL_Rect pos
 * @return int fontSize
 * */
int calculateSuitableFontSizeInBtn(SDL_Rect pos) {
    int padding = (int) pos.h * DEFAULT_PADDING_RATIO;
    int fontSize = pxToPt(pos.h - padding);
    if (fontSize < SMALLEST_FONT_SIZE) return SMALLEST_FONT_SIZE;
    if (fontSize > BIGGEST_FONT_SIZE) return BIGGEST_FONT_SIZE;
    return fontSize;
}

/**
 * Az adott pixelmennyiséget (px) átkonvertálja pontba (pt).
 * @param int px
 * @return int pt
 * */
int pxToPt(int px) {
    return (int) floor(px * 0.75);
}

/**
 * Megnézi, hogy az adott kattintás egy adott gomb területén történe-e
 * @param SDL_Rect *btnCord
 * @param int x
 * @oaram int y
 * @return bool
 * */
bool isClickedOnBtn(SDL_Rect *btnCord, int x, int y) {
    return btnCord->x <= x && btnCord->x + btnCord->w >= x && btnCord->y <= y && btnCord->y + btnCord->h >= y;
}

/**
 * Lerajzolja az összes gombot, amit az adott oldalon tartalmaz a menü
 * @param SDL_Renderer *renderer,
 * @param ButtonRect ** buttons,
 * @param int btnArrSize
 * */
void drawAllCurrentButtons(SDL_Renderer *renderer, ButtonRect **buttons, int btnArrSize) {
    for (int i = 0; i < btnArrSize; i++) {
        drawButton(renderer, buttons[i]);
    }
}

/**
 * Megnézi, hogy az adott koordináta (x,y) melyik gombon van rajta. Ha egyiken sincs akkor a visszatérési érték -1
 * @param ButtonRect ** buttons
 * @param int x
 * @param int y
 * @return ButtonId
 * */
int getClickedButtonIdIfExists(ButtonRect **buttons, Page currentPage, int x, int y) {
    for (int i = 0; i < getCurrentButtonArraySize(currentPage); i++) {
        if (isClickedOnBtn(&buttons[i]->rec, x, y)) {
            return buttons[i]->btnId;
        }
    }
    return -1;
}

/** Leírja, hogy mi történjen, ha a szint befejezése utáni értékelési oldaon kattintunk.
 *  Arra az oldalra a játékos mindig egy szint sikeres végigvitele után jut, ezért itt törölöm
 *  az aktuális játékot tartalmazó fájl tartalmát.
 * @param int buttonId
 * @return Page next
 * */
Page handleFinishedButtonClicks(int buttonId) {
    cJSON *currentData = getParsedJSONContentOfFile(ACTUAL_STATUS_FILE_NAME);
    printStructureIntoFileAndClose(ACTUAL_STATUS_FILE_NAME, NULL);
    if (buttonId == FIN_GAME_MENU) return gameMenu;
    if (buttonId == FIN_NEXT_LEVEL) {
        if (currentData != NULL) {
            int nextLevel = cJSON_GetObjectItem(currentData,LEVEL_STR)->valueint+1;
            if (nextLevel > LEVEL_NUM) return gameFinished;
            initializeFileWithLevel(nextLevel);
            return inGame;
        }
        return gameMenu;
    }
}


/**
 * Lekezeli azt, ha a játékos valamelyik gombra kattintott. Ha az adott oldalról egy másikba kéne váltani, akkor visszatér a következő Page
 * értékével, vagy -1-gyel, ha maradunk az adott ha maradunk az adott oldlon.
 * (Ameddig nincsen az összes funkció implementálva, addig minden esetben, ha olyan gombra kattint a felhasználó, aminek a
   működése nincsen elkészítve, akkor a főmenübe tér vissza alapértelmezetten.)
 * @param currentButtonId
 * @return Page next
 * */
Page handleBtnClickAndGetNextPageIfShould(int currentButtonId, Page currentPage) {
    if (currentButtonId == BACK_TO_MAIN_BTN) return mainMenu;

    switch (currentPage) {
        case mainMenu:
            return getNextPageOnMainMenuClickOrQuit(currentButtonId);
        case gameMenu:
            return clickedOnLevel(currentButtonId);
        case levelFinished:
            return handleFinishedButtonClicks(currentButtonId);
        case inGame:
            if (currentButtonId == IG_BACK_BTN) return gameMenu;
            break;
        case settings:
            handleSettingsClick(currentButtonId);
            return mainMenu;
        default:
            return mainMenu;
    }
}

/** Lekezeli azt, hogy a kurzor amennyiben gomb fölött helyezkedik el válozzon át mutatóvá, egyébként legyen nyíl.
 * @param ButtonRect **buttons
 * @param int x
 * @param int y
 * @param Page currentPage
 * */
void handleCursor(ButtonRect **buttons, int x, int y, Page currentPage) {
    SDL_Cursor *cursor;
    if (getClickedButtonIdIfExists(buttons, currentPage, x, y) != -1) {
        cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND);
    } else {
        cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);
    }
    SDL_SetCursor(cursor);
}

/** Betölti a háttérképet, és kirajzolja.
 * @param SDL_Renderer * renderer
 * */
void drawBackground(SDL_Renderer *renderer) {
    SDL_Texture *background = IMG_LoadTexture(renderer, BACKGROUND_FILE_NAME);
    SDL_Rect dest = (SDL_Rect) {0, 0, S_WIDTH, S_HEIGHT};
    SDL_RenderCopy(renderer, background, NULL, &dest);
    SDL_DestroyTexture(background);

    SDL_RenderPresent(renderer);
}

/** Visszaállítja a képernyőt alapállapotba. A függvény akkor fut le, ha oldalváltás történi a menüben.
 *  Felszabadítja az összes, gombok számára lefoglalt memóriaterületet.
 *  @param SDL_Renderer * renderer,
 *  @param ButtonRect ** buttons,
 *  @param Page currentPage
 * */
void resetScreenAndFreeButtonsArray(SDL_Renderer *renderer, ButtonRect **buttons, Page currentPage) {
    for (int i = 0; i < getCurrentButtonArraySize(currentPage); i++) {

        if (buttons[i]->btnTitle.isTitleMalloced) {
            free(buttons[i]->btnTitle.title);
        }
        free(buttons[i]);
    }
    free(buttons);
    drawBackground(renderer);
}


/** Elkészíti azokat a gombokat, amelyek az egy szint teljesítését követő oldalon jelennek meg.
 * @return ButtonRect** finishButtons
 * */
ButtonRect **createLevelFinishedButtons() {
    ButtonRect **finishButtons;
    finishButtons = (ButtonRect **) malloc(LEVEL_FINISHED_BTN_NUM * sizeof(ButtonRect));
    finishButtons[0] = initBtn((SDL_Rect) {S_WIDTH / 2 - 75, S_HEIGHT / 2 + 150, 150, 50}, "Játékmenü", FIN_GAME_MENU);
    finishButtons[1] = initBtn((SDL_Rect) {S_WIDTH / 2 - 75, S_HEIGHT / 2 + 225, 150, 50}, "Következő szint",
                               FIN_NEXT_LEVEL);
    return finishButtons;
}

/**
 * Az összes pálya kijátszását követő oldal gombját készíti el.
 * @return ButtonRect** finishButtons
 * */
ButtonRect **createGameFinishedButtons() {
    ButtonRect **finishButtons;
    finishButtons = (ButtonRect **) malloc( GAME_FINISHED_BTN_NUM* sizeof(ButtonRect));
    finishButtons[0] = initBtn((SDL_Rect) {S_WIDTH / 2 - 75, S_HEIGHT / 2 + 150, 150, 50}, "Vissza a főmenübe", BACK_TO_MAIN_BTN);
    return finishButtons;
}

/**
 * Az adott oldalnak megfelelő gombok tömbjével tér vissza.
 * @param Page currentPage
* @return ButtonRect** buttons
 * */
ButtonRect **createButtonsForCurrentPage(Page currentPage) {

    ButtonRect **buttons;

    switch (currentPage) {
        case mainMenu:
            buttons = createMainMenuButtons();
            break;
        case settings:
            buttons = createSettingsMenuButtons();
            break;
        case gameMenu:
            buttons = createAllGameButton();
            refreshButtonColors(buttons);
            break;
        case inGame:
            buttons = createInGameButtons();
            break;
        case exitgame:
            return;
        case levelFinished:
            buttons = createLevelFinishedButtons();
            break;
        case gameFinished:
            buttons = createGameFinishedButtons();
            break;
        default:
            buttons = createMainMenuButtons();
            break;
    }

    return buttons;
}


Page runMenuPage(SDL_Renderer *renderer, Page current) {
    ButtonRect **buttons = createButtonsForCurrentPage(current);

    if (current == gameMenu) drawStarsForLevels(renderer, buttons);
    else if (current == levelFinished) drawResultsToFinishPage(renderer);
    else if (current == gameFinished) drawGameFinishedPage(renderer);
    else if (current == settings) writeMessagesToSettings(renderer);

    drawAllCurrentButtons(renderer, buttons, getCurrentButtonArraySize(current));

    SDL_RenderPresent(renderer);

    SDL_Event event;
    bool shouldLeavePage = false;
    Page nextPage = -1;

    while (SDL_WaitEvent(&event) && event.type != SDL_QUIT && !shouldLeavePage) {
        /* SDL_RenderPresent(renderer); - MacOS Mojave esetén */
        switch (event.type) {
            case SDL_MOUSEBUTTONUP:
                if (event.button.button == SDL_BUTTON_LEFT) {
                    int clickedOnBtnId = getClickedButtonIdIfExists(buttons, current, event.button.x,
                                                                    event.button.y);
                    if (clickedOnBtnId != -1) {
                        nextPage = handleBtnClickAndGetNextPageIfShould(clickedOnBtnId, current);
                    }
                }
                break;
            case SDL_MOUSEMOTION:
                handleCursor(buttons, event.motion.x, event.motion.y, current);
                break;
        }

        if (nextPage != -1) {
            shouldLeavePage = true;
            break;
        }
    }

    if (shouldLeavePage) {
        resetScreenAndFreeButtonsArray(renderer, buttons, current);
        return nextPage;
    }
    return -1;
}

/**
 * Egy főmenübeli gombon való kattintást kezel le. A nextPage változóba jrja azt, hogy melyik a következő Page amit
 * be kell tölteni. (Vagy a Kilépés gombra kattintva leállítja a program futását.)
 * @param int buttonId
 * @return Page nextPage
 * */
Page getNextPageOnMainMenuClickOrQuit(int buttonId) {
    switch (buttonId) {
        case M_EXIT_BTN:
            return exitgame;
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

/** Elkészíti a beállítások menü gombjait, lefogalalja nekik a szükséges memóriaterületet.
* @return ButtonRect ** buttons
 * */
ButtonRect **createSettingsMenuButtons() {

    ButtonRect **buttons;
    buttons = (ButtonRect **) malloc(S_BTN_NUM * sizeof(ButtonRect));

    if (isUserDeveloper()) {
    buttons[0] = initBtn((SDL_Rect) {S_WIDTH / 2 - 75, S_HEIGHT / 2, 150, 50}, "Fejlesztő mód ki", S_DEV_MODE_BTN);
    }
    else {
    buttons[0] = initBtn((SDL_Rect) {S_WIDTH / 2 - 75, S_HEIGHT / 2, 150, 50}, "Fejlesztő mód be", S_DEV_MODE_BTN);
    }


    buttons[1] = initBtn((SDL_Rect) {S_WIDTH / 2 - 75, S_HEIGHT / 2 + 75, 150, 50}, "Reset", S_RESET_BTN);
    buttons[2] = initBtn((SDL_Rect) {S_WIDTH / 2 - 75, S_HEIGHT / 2 + 150, 150, 50}, "Vissza a főmenübe", BACK_TO_MAIN_BTN);

    return buttons;
}

/** Megmondja a settings.json fájl alapjámón, hogy a felhasználó fejlesztő e
 * @return isUserDeveloper
 * */
bool isUserDeveloper() {
    cJSON * settings = getParsedJSONContentOfFile(SETTINGS_FILE_NAME);
    if (settings == NULL) return false;
    return cJSON_GetObjectItem(settings, IS_USER_DEVELOPER)->valueint == 1;
}

/**
 * Figyelmeztető szövegeket ír a beállítások menüre.
 * @param SDL_Renderer *renderer
 * */
void writeMessagesToSettings(SDL_Renderer * renderer) {
    writeText(renderer, (SDL_Rect) {S_WIDTH/2-400, 100, 800,50}, "A reset gombra kattintva visszavonhatatlanul törlődik az összes eddigi haladás!",20);
    writeText(renderer, (SDL_Rect) {S_WIDTH/2-400, 200, 800,50}, "Fejlesztő módban bármilyen pályával lehet játszani, bármennyit ki lehet hagyni.",20);
}

/**
 * A beállítások menübeli kattintásokat kezeli le.
 *  A reset gombra kattintva törli a levels.json, és a setup.json fájl tartalmár.
 *  A fejlsztői mód be/ki gombra kattintva a settngs.json fájl tartalmát módosítja.
 *  @param int buttonId
 **/
void handleSettingsClick(int buttonId) {
    if (buttonId == S_RESET_BTN) {
        printStructureIntoFileAndClose(LEVELS_FILE_NAME, NULL);
        printStructureIntoFileAndClose(SETTINGS_FILE_NAME, NULL);
    }
    else if (buttonId == S_DEV_MODE_BTN) {
        cJSON * settings = getParsedJSONContentOfFile(SETTINGS_FILE_NAME);
        if (settings == NULL || cJSON_GetObjectItem(settings, IS_USER_DEVELOPER)->valueint == 0) {
            settings = cJSON_CreateObject();
            cJSON_AddNumberToObject(settings, IS_USER_DEVELOPER, 1);
        }
        else {
            cJSON_DeleteItemFromObject(settings, IS_USER_DEVELOPER);
            cJSON_AddNumberToObject(settings, IS_USER_DEVELOPER, 0);
        }
        printStructureIntoFileAndClose(SETTINGS_FILE_NAME, settings);
        cJSON_Delete(settings);
    }
}


