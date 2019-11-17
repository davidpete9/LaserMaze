
#ifndef LASERMAZE_MENUENGINE_H
#define LASERMAZE_MENUENGINE_H

typedef enum Page {
    mainMenu, settings, gameMenu, inGame, exitgame
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

/**
* Létrehoz egy Button típusú struktúrát, alapértelmezetten beállítja a gomb színét, betűméretét, és a szöveget pozicionálja a gombon belül.
* @param SDL_Rect pos
* @param char *title
* @param ButtonId id
* @return ButtonRect * btn
* */
ButtonRect *initBtn(SDL_Rect pos, char *title, int id);
/**
* Kiszámolja, hogy egy adott méretű gombba melyik a legmegelelőbb betűméret.
* @param SDL_Rect pos
* @return int fontSize
* */
int calculateSuitableFontSizeInBtn(SDL_Rect pos);
/**
* Lerajzolja az összes gombot, amit az adott oldalon tartalmaz a menü
* @param SDL_Renderer *renderer,
* @param ButtonRect ** buttons,
* @param Page currentPage
* */
void drawAllCurrentButtons(SDL_Renderer *renderer, ButtonRect **buttons, Page currentPage);
/**
* Kirajzolja az adott gombot a képernyőre
* @param SDL_Renderer * render
* @param ButtonRect *btn
* */
void drawButton(SDL_Renderer *renderer, ButtonRect *btn);
/**
* Megnézi, hogy az adott koordináta (x,y) melyik gombon van rajta. Ha egyiken sincs akkor a visszatérési érték -1
* @param ButtonRect ** buttons
* @param int x
* @param int y
* @return ButtonId
* */
int getClickedButtonIdIfExists(ButtonRect **buttons, Page currentPage, int x, int y);
/** Megnézi, hogy az aktuális oldalon hány gomb van a gombok tömbjében.
* Kikeresi a Constanses.c fájlban deklarált konstansok közül
* @param Page currentPage
* @return int length
* */
int getCurrentButtonArraySize(Page currentPage);
/**
* Lekezeli azt, ha a játékos valamelyik gombra kattintott. Ha az adott oldalról egy másikba kéne váltani, akkor visszatér a következő Page
* értékével, vagy -1-gyel, ha maradunk az adott ha maradunk az adott oldlon.
* (Ameddig nincsen az összes funkció implementálva, addig minden esetben, ha olyan gombra kattint a felhasználó, aminek a
működése nincsen elkészítve, akkor a főmenübe tér vissza alapértelmezetten.)
* @param currentButtonId
* @return Page next
* */
Page handleBtnClickAndGetNextPageIfShould(int currentButtonId, Page currentPage);
/** Lekezeli azt, hogy a kurzor amennyiben gomb fölött helyezkedik el válozzon át mutatóvá, egyébként legyen nyíl.
* @param ButtonRect **buttons
* @param int x
* @param int y
* @param Page currentPage
* */
void handleCursor(ButtonRect **buttons, int x, int y, Page currentPage);
/** Elkészíti a menüt egy adott oldalon. Leírja, hogy melyik oldal esetén milyen gombokat tartalmazzon a menü.
*  A runMenu függvény segítségével végzi az Event ellenőrzését.
*  Mikor oldalváltás történik a függvény a runMenu által újra meghívódik rekrúzívan.
*  Akkor fejezi be a futását, mikor a játékból kilépett a felhasználó.
*  @param SDL_Renderer *renderer
*  @param Page currentPage
* */
void initializeMenu(SDL_Renderer *renderer, Page currentPage);
/**
* Megnézi, hogy az adott kattintás egy adott gomb területén történe-e
* @param SDL_Rect *btnCord
* @param int x
* @oaram int y
* @return bool
* */
bool isClickedOnBtn(SDL_Rect *btnCord, int x, int y);
/**
* Az adott pixelmennyiséget (px) átkonvertálja pontba (pt).
* @param int px
* @return int pt
* */
int pxToPt(int px);
/** Visszaállítja a képernyőt alapállapotba. A függvény akkor fut le, ha oldalváltás történi a menüben.
*  Felszabadítja az összes, gombok számára lefoglalt memóriaterületet.
*  @param SDL_Renderer * renderer,
*  @param ButtonRect ** buttons,
*  @param Page currentPage
* */
void resetScreenAndFreeButtonsArray(SDL_Renderer *renderer, ButtonRect **buttons, Page currentPage);
/** A menü futtatást végzi, lekezeli azt, mikor a felhasználó valahol kattint, amennyiben oldalt kell váltania
*  meghívja az initializeMenu függvényt, ami előkészíti a következő oldalt.
*  @param SDL_Renderer * renderer
*  @param ButtonRect **buttons
*  @param Page currentPage
* */
void runMenu(SDL_Renderer *renderer, ButtonRect **buttons, Page currentPage);
/**
* Kiszámolja és beállítja, hogy hol legyen az a négyszög, amelybe majd a gomb szövegét kell írni.
* Az adott négyszög kisebb mint az eredeti, mivel kihagy egy kevés margót minden szélen.
* @param SDL_Rect * pos
* */
void setBtnTitlePos(SDL_Rect *strPos, SDL_Rect pos);
/**
* Leírja a képernyőre az adott szöveget.
* @param SDL_Renderer *renderer
* @param const StringToDisplay * strObj
* */
void writeTextToDisplay(SDL_Renderer *renderer, const StringToDisplay *strObj);


#endif //LASERMAZE_MENUENGINE_H
