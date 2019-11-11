
#ifndef LASERMAZE_MENUENGINE_H
#define LASERMAZE_MENUENGINE_H

enum {
    SMALLEST_FONT_SIZE = 10,
    BIGGEST_FONT_SIZE = 48,

    M_BTN_NUM = 3,
    S_BTN_NUM = 3,
    IG_BTN_NUM = 4,
    G_BTN_NUM = 1,
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

ButtonRect **createGameButtons();
ButtonRect **createMainMenuButtons();
ButtonRect *initBtn(SDL_Rect pos, char *title, int id);
int calculateSuitableFontSizeInBtn(SDL_Rect pos);
void drawAllCurrentButtons(SDL_Renderer *renderer, ButtonRect ** buttons, Page currentPage);
void drawButton(SDL_Renderer *renderer, ButtonRect *btn);
int getClickedButtonIdIfExists(ButtonRect **buttons, Page currentPage, int x, int y);
int getCurrentButtonArraySize(Page currentPage);
Page getNextPageOnMainMenuClickOrQuit(int buttonId);
Page handleBtnClickAndGetNextPageIfShould(int currentButtonId, Page currentPage);
void handleCursor(ButtonRect ** buttons, int x, int y, Page currentPage);
void initializeMenu(SDL_Renderer *renderer, Page page);
bool isClickedOnBtn(SDL_Rect *btnCord, int x, int y);
int pxToPt(int px);
void resetScreenAndFreeButtonsArray(SDL_Renderer *renderer, ButtonRect **buttons, Page currentPage);
void runMenu(SDL_Renderer *renderer, ButtonRect **buttons, Page page);
void setBtnTitlePos(SDL_Rect *strPos, SDL_Rect pos);
void writeTextToDisplay(SDL_Renderer *renderer, const StringToDisplay *strObj);


#endif //LASERMAZE_MENUENGINE_H
