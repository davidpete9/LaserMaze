
#ifndef LASERMAZE_MENUENGINE_H
#define LASERMAZE_MENUENGINE_H

typedef enum Page {
    mainMenu, settings, gameMenu, inGame, levelFinished, gameFinished, exitgame
} Page;

typedef struct StringToDisplay {
    const char *title;
    bool isTitleMalloced;
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

ButtonRect **createButtonsForCurrentPage(Page currentPage);
ButtonRect **createGameFinishedButtons();
ButtonRect **createLevelFinishedButtons();
ButtonRect **createMainMenuButtons();
ButtonRect **createSettingsMenuButtons();
ButtonRect *initBtn(SDL_Rect pos, char *title, int id);
int calculateSuitableFontSizeInBtn(SDL_Rect pos);
void drawAllCurrentButtons(SDL_Renderer *renderer, ButtonRect **buttons, int btnArrSize);
void drawBackground(SDL_Renderer *renderer);
void drawButton(SDL_Renderer *renderer, ButtonRect *btn);
int getClickedButtonIdIfExists(ButtonRect **buttons, Page currentPage, int x, int y);
int getCurrentButtonArraySize(Page currentPage);
Page getNextPageOnMainMenuClickOrQuit(int buttonId);
Page handleBtnClickAndGetNextPageIfShould(int currentButtonId, Page currentPage);
void handleCursor(ButtonRect **buttons, int x, int y, Page currentPage);
Page handleFinishedButtonClicks(int buttonId);
void handleSettingsClick(int buttonId);
bool isClickedOnBtn(SDL_Rect *btnCord, int x, int y);
bool isUserDeveloper();
int pxToPt(int px);
void resetScreenAndFreeButtonsArray(SDL_Renderer *renderer, ButtonRect **buttons, Page currentPage);
Page runMenuPage(SDL_Renderer *renderer, Page current);
void setBtnTitlePos(SDL_Rect *strPos, SDL_Rect pos);
void writeMessagesToSettings(SDL_Renderer * renderer);
void writeTextToDisplay(SDL_Renderer *renderer, const StringToDisplay *strObj);

#endif //LASERMAZE_MENUENGINE_H
