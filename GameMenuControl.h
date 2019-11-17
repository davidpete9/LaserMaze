#ifndef GAMEMENUCONTROL_H_INCLUDED
#define GAMEMENUCONTROL_H_INCLUDED

/**
* Előkészíti a szintek gombjait a játékmenüben, úgy hogy a képernyő közepén sorokba legyenek rendezbe.
*  lefogalalja nekik a szükséges memóriaterületet, a hívó felelőssége a memóriafelszabadítás.
* @return ButtonRect ** buttons
* */
ButtonRect **createAllGameButton();
/**Lekezeli, hogy mi történik, ha egy játékmenüben található gombra kattint a felhasználó.
* Amennyiben nem érvényes gombra kattintott a főmenűbe irányít vissza. Érvényes szintre való kattintás
* esetén előkészíti a leendő játékfájlt, melyben eltárolja a szintet.
* @param int btnId
* @return Page nextPageToGo
* */
Page clickedOnLevel(int btnId);
/**
*  Előkészíti a játékpályán lévő gombokat.
*  lefogalalja nekik a szükséges memóriaterületet, a hívó felelőssége a memóriafelszabadítás.
* @return ButtonRect ** buttons
* */
ButtonRect ** createInGameButtons();

#endif // GAMEMENUCONTROL_H_INCLUDED
