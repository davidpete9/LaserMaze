#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>


enum {
    SMALLEST_FONT_SIZE = 10,
    BIGGEST_FONT_SIZE = 48,

    M_BTN_NUM = 3,
    S_BTN_NUM = 3,
    IG_BTN_NUM = 4,
    G_BTN_NUM =1,
};

enum {
    //Main menu buttons
    M_GAME_BTN = 101,
    M_SETTINGS_BTN = 102,
    M_EXIT_BTN = 103,

    //Settings buttons
    S_MUTE_BTN = 201,
    S_RESET_BTN = 202,

    //Game menu  buttons
    //all number between: 1 - num of levels

    //Back to main menu
    BACK_TO_MAIN_BTN = 999,

    //In game buttons
    IG_FIRE_BTN = 301,
    IG_RESET_BTN = 302,
    IG_SKIP_BTN = 303,
    IG_PAUSE_BTN = 304,
};

typedef enum Page {
    mainMenu, settings, gameMenu, inGame
} Page;

Page CurrentPage = mainMenu;

const int S_WIDTH = 880;
const int S_HEIGHT = 720;

const double DEFAULT_PADDING_RATIO = 0.02;
const SDL_Color DEFAULT_TEXT_COLOR = {255, 255, 255, 255};
const SDL_Color DEFAULT_BUTTON_COLOR = {255, 0, 0, 255};
const SDL_Color DEFAULT_BACKGROUND_COLOR = {0,0,0,255};


typedef struct StringToDisplay {
    const char *title;
    SDL_Color titleColor;
    int fontSize;
    SDL_Rect pos;
} StringToDisplay;

typedef struct ButtonRect {
    int btnId;
    SDL_Rect rec;
    SDL_Color col;
    StringToDisplay btnTitle;

} ButtonRect;


int getCurrentButtonArraySize() {
    switch (CurrentPage) {
        case mainMenu:
            return M_BTN_NUM;
        case settings:
            return S_BTN_NUM;
        case inGame:
            return IG_BTN_NUM;
        case gameMenu:
            return G_BTN_NUM;

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

    SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0x00, 0xFF);
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

//Todo másik fájlba
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

ButtonRect **createGameButtons() {
    ButtonRect **gameBtns;
    gameBtns = (ButtonRect **) malloc(2 * sizeof(ButtonRect));

    gameBtns[0] = initBtn((SDL_Rect) {S_WIDTH / 2 - 75, S_HEIGHT / 2, 150, 50}, "Vissza a főmenübe", BACK_TO_MAIN_BTN);

    return gameBtns;
}


void initializeMenu(SDL_Renderer *renderer) {

    ButtonRect **buttons;

    switch (CurrentPage) {
        case mainMenu:
            buttons = createMainMenuButtons();
            break;
        case settings:
            //buttons = createSettingsButtons();
            break;
        case gameMenu:
            buttons = createGameButtons();
            break;
        case inGame:
            //buttons = createInGameButtons();
            break;
        default:
            break;
    }

    drawAllCurrentButtons(renderer, buttons);
    runMenu(renderer, buttons);

}

void drawAllCurrentButtons(SDL_Renderer *renderer, ButtonRect ** buttons) {
    for (int i = 0; i < getCurrentButtonArraySize(); i++) {
        drawButton(renderer, buttons[i]);
    }
    SDL_RenderPresent(renderer);

}

/**
 * Megnézi, hogy az adott koordináta (x,y) melyik gombon van rajta. Ha egyiken sincs akkor a visszatérési érték -1
 * @param ButtonRect ** buttons
 * @param int x
 * @param int y
 * @return ButtonId
 * */
int getClickedButtonIdIfExists(ButtonRect ** buttons, int x, int y) {
    for (int i = 0; i < getCurrentButtonArraySize(); i++) {
        if (isClickedOnBtn(&buttons[i]->rec, x, y)) {
            return buttons[i]->btnId;
        }
    }
    return -1;
}


//  TODO: majd átrakni külön, MainMenuEngine.c fájlba
/**
 * Egy főmenübeli gombon való kattintást kezel le. A nextPage változóba jrja azt, hogy melyik a következő Page amit
 * be kell tölteni. (Vagy a Kilépés gombra kattintva leállítja a program futását.)
 * @param int buttonId
 * @return Page nextPage
 * */
Page getNextPageOnMainMenuClickOrQuit(int buttonId) {
    switch (buttonId) {
        case M_EXIT_BTN:
            exit(1);
        case M_GAME_BTN:
            return gameMenu;
        case M_SETTINGS_BTN:
            return settings;
        default:
            return mainMenu;
    }
}




/**
 * Lekezeli azt, ha a játékos valamelyik gombra kattintott. Ha az adott oldalról egy másikba kéne váltani, akkor visszatér a következő Page
 * értékével, vagy -1 - gyel, ha maradunk az adott oldlon.
 * @param currentButtonId
 * @return Page next
 * */
Page handleBtnClickAndGetNextPageIfShould(int currentButtonId) {
    switch (CurrentPage) {
        case mainMenu:
            return getNextPageOnMainMenuClickOrQuit(currentButtonId);
        case gameMenu:
            if (currentButtonId == BACK_TO_MAIN_BTN) {
                return mainMenu;
            }
    }
    return -1;
}


void handleCursor(ButtonRect ** buttons, int x, int y) {
    SDL_Cursor* cursor;
    if (getClickedButtonIdIfExists(buttons,x,y) != -1) {
        cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND);
    }
    else {
        cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);
    }
    SDL_SetCursor(cursor);
}

void resetScreenAndFreeButtonsArray(SDL_Renderer *renderer, ButtonRect **buttons) {
    for (int i = 0; i < getCurrentButtonArraySize(); i++) {
        free(buttons[i]);
    }
    free(buttons);

    SDL_SetRenderDrawColor(renderer, DEFAULT_BACKGROUND_COLOR.r, DEFAULT_BACKGROUND_COLOR.g,
                            DEFAULT_BACKGROUND_COLOR.b, DEFAULT_BACKGROUND_COLOR.a);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);
}

void runMenu(SDL_Renderer *renderer, ButtonRect **buttons) {
    SDL_Event event;
    bool shouldLeavePage = false;
    Page nextPage = -1;

    while (SDL_WaitEvent(&event) && event.type != SDL_QUIT && !shouldLeavePage) {
        /* SDL_RenderPresent(renderer); - MacOS Mojave esetén */
        switch (event.type) {
            case SDL_MOUSEBUTTONUP:
                if (event.button.button == SDL_BUTTON_LEFT) {
                    int clickedOnBtnId = getClickedButtonIdIfExists(buttons, event.button.x, event.button.y);
                    if (clickedOnBtnId != -1) {
                        nextPage = handleBtnClickAndGetNextPageIfShould(clickedOnBtnId);
                        if (nextPage != -1) {
                             resetScreenAndFreeButtonsArray(renderer, buttons);
                             shouldLeavePage = true;
                             CurrentPage = nextPage;
                        }
                    }
                }
                break;
            case SDL_MOUSEMOTION:
                handleCursor(buttons, event.motion.x, event.motion.y);
                break;
            case SDL_QUIT:
                exit(1);
                SDL_Quit();
                break;
        }
    }
    initializeMenu(renderer);
}






