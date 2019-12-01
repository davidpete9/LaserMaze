#Programozói dokumentáció

A program összes függvény dokumentálva van, itt csak a fontosabb egységek működését írom le.

##1. A program által használt külső könyvtárak, fejlesztőkörnyezet
* A program a grafikus megjelenítéshez az SDL2 függvénykönyvtárt használja
* A JSON adatszerkezetű fájlok kezelésére: a cJSON függvénykönyvtár (`cJSON.c` és `cJSON.h`)
   * [Github libary](https://github.com/DaveGamble/cJSON)
   * Ezek a fájlok a függvény hívási gráfokban ha megjelennek, rekrúzívan hivatkoznak egymásra.
* CodeBlocks fejlesztőkörnyezetben készült Windowson, az SDL2 az INFOC-n leírt módszerrel telepítettem.

##2. A program felépítése, a program forrásfájlai
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
 A menü működését teszi lehetővé. A `runMenuPage` függvény hívásával létrehozza, és kirajzolja az oldal gombjait,
 és utána az alapján, hogy a felhasználó melyik gombra kattintott, mindig a megfelelő függvényeket futtatja le.
 
 A gombok egy `ButtonRect ` típusú tömbben vannak
 benne, melyek tartalmazzák a gomb adatait. A gomb azonosítója (`btnId`) alapján tudja a program azonosítani a gombokat. Minden `btnId`-hoz csak egy gomb tartozik,
 ezekbek a leírsa a `Constanses.h` fájlban történik.

 ##GameMenuControl.c
 A játékmenü funkciójit kezeli. A `clickedOnLevel` függvény egy leveln történő kattintást kezel le,
 eldönti, hogy elindítja e a játékot az alábbiak alapján.
 
     if (isUserDeveloper() || isLevelAlreadyDone(btnId) || getCurrentLevel() == btnId) {
         if (getInitializedGameLevelIfExists() != btnId) initializeFileWithLevel(btnId);
         return inGame;
     }
Tehát ha játékos fejlesztő módban van, vagy a szintet már előzőleg teljesítette, vagy éppen az adott szint következik, akkor 
elindíja a játékot, az `inGame`oldalra irányítással. Az `initializeFileWithLevel()` függvény segítségével az `actual.json`  fájlba írja bele azt, 
hogy melyik szinten fog elkezdődni a játék. Ha már az adott játékot elkezdte a játékos, és a játékállás el van mentve, akkor az `GameStarter.c` majd azt
azt fogja betölteni.

A fájl többek között még tartalmzza a `drawResultsToFinishPage` függvény is, aminek az a feladata, hogy amikor a játékos 
egy pályát sikeresen teljesített, akkor annak az eredményeit kirajzolja a képenyőre. 
   
 ##FileHandler.c
 A fájlkezelést segítő függvényeket tartalmazza.
 
 Fontosabbak:
 * `getParsedJSONContentOfFile` - Beolvassa a kapott JSON fájl teljes tartalmát, és cJSON struktúrává alakítja.
 * `printStructureIntoFileAndClose`  -A kapott nevű fájlba beleírja az egész JSON objektumot, amit paraméterként kapott,
                                         a fájl tartalmát felülírja vele.
 ###GameStarter.c
 A játékindítást, és a játékmenetet irányító eseményeket kezeli.
 `startGame` beállítja hogy milyen pályákon fog játszani a játékos, a `runGame` pedig a játékmenetet kezeli.
 Minden pályát előszőr betölt egy megfelő json fájlból, majd a tartalmát egy `Cell` adatszerkezetű kétdimenziós tömbben tárol el
 hogy könnyebben tudja kezelni a `GameBoardEvents.c`. Azt a tömböt az `initGridStructure` függvény hozza létre.
 
 Bővebben:
  
  A `runGame` A játék futtatását kezeli. Az alapján tér vissza egy logikai értékkel, hogy a játékos teljesítette e pályát vagy sem.
  Előszőr az `actual.json` fájlból betölti a játékállapotot, és a `GameBoardEvents.c` fájl függvényei segítségével kirajzolja a
  pályát. A `runGameEvents()` függvény visszatérési értéke alapján dolgozik, ami a `GameEvent enum` értékeivel tré vissza. Addig fut egy a program,
  amíg a játékos be nem fejezte a pályát, vagy ki nem lépett. Az utóbbi esetben az aktuális játékállapotot elmenti a program az `actual.json` fájlba.  
 ###GameBoardEvents.c
 A játékban történő események menetét kezeli. A `runGameEvents` a központi függvény, ami kezeli a 
 játékos kattintásait, amint valamelyik gombra kattintot visszatér a megfelelő eseménnyel. Továbbá kezeli a 
 mozgatható blokkok mozgatását.
 
 Ha a játékos a "Tűz" gombra kattintott, akkor kirajzolja a lézerfény útját, ami egy bináris fában tárol el.
Ezt a fát a `createLaserPathTree` rekrúzív függvény építi meg.  
 
 ###MirrorBounce.c
 A különböző tükrökről történő visszapattanásokat kezeli.
 A `findDirection` függvény megmondja, hogy egy adott irányból egy adott típusú blokkról milyen irányba pattan le a lézersugár.
 
 ###Constanses.c
 A program működéséhez szükséges konstansokat tárolja el.
  
##Főbb adatszerkezetek:
### ButtonRect és StringToDisplay
A gombokat, és a hozzá tartozó feliratokat tárolja.

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
### Cell**
Az a táblázat, amely a pályán lévő blokkokat tárolja. Egy elem:

    typedef struct Cell {
        int block_id;
        bool isPlacedByUser;
        bool isRotatable;
        bool isMoveable;
        int rotation;
        bool isLeftSide;
        bool display;
        bool hasLaserTouchedIt;
        Direction laserTouchedItDir;
    } Cell;

### LaserPath *
Bináris fa, a lézerfény újjának a tárolásáért felel. Egy elem:

    typedef struct LaserPath {
        int startRow;
        int startCol;
        Direction dir;
        struct LaserPath * next;
        struct LaserPath * next2;
    } LaserPath;
