
#ifndef LASERMAZE_MENUENGINE_H
#define LASERMAZE_MENUENGINE_H

typedef struct StringToDisplay {
    const char *title;
    SDL_Color titleColor;
    int fontSize;
    SDL_Rect pos;
} StringToDisplay;

typedef struct ButtonRect {
    SDL_Rect rec;
    SDL_Color col;
    StringToDisplay btnTitle;

} ButtonRect;

void writeText(SDL_Renderer *renderer, const StringToDisplay *strObj);

void drawButton(SDL_Renderer *renderer, ButtonRect *btn);

ButtonRect *initBtn(SDL_Rect pos, char *title);

void initializeMenu(SDL_Renderer *renderer);

#endif //LASERMAZE_MENUENGINE_H
