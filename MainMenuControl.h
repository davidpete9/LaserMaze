#ifndef MAINMENUCONTROL_H_INCLUDED
#define MAINMENUCONTROL_H_INCLUDED

/**
* Elkészíti a főmenü gombjait, lefogalalja nekik a szükséges memóriaterületet.
* @return ButtonRect ** buttons
* */
ButtonRect **createMainMenuButtons();
/**
* Egy főmenübeli gombon való kattintást kezel le. A nextPage változóba jrja azt, hogy melyik a következő Page amit
* be kell tölteni. (Vagy a Kilépés gombra kattintva leállítja a program futását.)
* @param int buttonId
* @return Page nextPage
* */
Page getNextPageOnMainMenuClickOrQuit(int buttonId);


#endif // MAINMENUCONTROL_H_INCLUDED
