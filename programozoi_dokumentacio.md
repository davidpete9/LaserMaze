#Programozói dokumentáció

##1. A program által használt fájlok 
### main.c
Elindítja a menüt, a `runMenuPage()` függvény segítségével, ami mindig 
visszatér a következő oldallal, ahova éppen a felhasználó navigált.
Amennyiben a játékos egy pályát elindított, akkor a `startGame()` függvény fogja visszaadni a következő oldalt.
A program `while` ciklusból csak akkor lép ki, ha a játékos a kilépés gombra kattntott.

    Page next = runMenuPage(renderer, mainMenu);
    while (next != -1 && next != exitgame) {
        if (next == inGame) {
            next = startGame(renderer);
        }
        else {
            next = runMenuPage(renderer, next);
        }
    }
### MenuEngine.c
A menü működését teszi lehetővé. A `runMenuPage` függvény hívásával létrehozza, majd kirajzolja az oldal gombjait,
 és utána ellenőrzi azt, hogy ha a felhasználó valamelyik gombra kattintott, akkor mi történjen. A gombok egy `ButtonRect ` típusú tömbben vannak
 benne, melyek tartalmazzák a gomb adatait. A gomb azonosítója (`btnId`) alapján tudja a program azonosítani a gombokat. Minden `btnId`-hoz csak egy gomb tartozik,
 ezekbek a leírsa a `Constanses.h` fájlban történik.
 ##GameMenuControl.c
 A játémenü működésiért felel.
 ##FileHandler.c
 A fájlkezelést segítő függvények.
 ###GameStarter.c
 A játékindítást, és a játékmenet folyamatáért felel.
 ###GameBoardEvents.c
 A játékban történő események menetét kezeli.
 ###MirrorBounce.c
 A különböző tükrökről történő visszapattanások
 
 ##Főbb adatszerkezetek.
### ButtonRect
### Cell**
### cJSON*
### LaserPath *

  