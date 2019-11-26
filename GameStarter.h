#ifndef GAMESTARTER_H_INCLUDED
#define GAMESTARTER_H_INCLUDED
#include "cJSON.h"

/**
* Lerajzolja a játéktáblára a kapott blokkot, vagy a nagy táblára, vagy a jobb oldali mezőre.
* @param SDL_Renderer * renderer
* @param cJSON * block
* @param SDL_Rect DEST
* @param int GRID_COLS
* */
void drawBlock(SDL_Renderer * renderer, cJSON * block, SDL_Rect DEST, int GRID_COLS);
/** Kirajzolja a játéktábla alapjait (négyzetrácsok, jobb oldali sáv)
* @param SDL_Renderer *renderer
* @pararm int level
* */
void drawGameTable(SDL_Renderer *renderer, int level);
/** Kirajzolja a négyzetrácsokat.
* @pararm SDL_Renderer * renderer
* */
void drawGrid(SDL_Renderer * renderer);
/** Egy számot beilleszt egy adott formátumú stringbe.
* A hívó felelőssége a kapott stringet által foglalt memóriaterületet felszabadítani.
* @param int num
* @param const char *format
* @return char * formatted
* */
char * generateFormattedStringFromNumber(int num, const char * format);
/** Visszatér az adott blokk textúrájával, amit fájlból töltött be.
* A hívó felelőssége ezt késöbb SDL_DestroyTexture()-rel törölni.
* @param SDL_Renderer *renderer,
* @param int blockId
* @return SDL_Texture * blockImg
* */
SDL_Texture * getBlockTexture(SDL_Renderer *renderer, int blockId);
/** Az actual.json fájlt létrehozza, és elmenti benne az aktuális levelt.
* @param int level
* */
void initializeFileWithLevel(int level);
/**
* Lerajzolja a az összes blokkot egy pályán, a nagy táblára, vagy a jobb oldali mezőre.
* @param SDL_Renderer * renderer
* @param cJSON * blocksArr
* @param SDL_Rect DEST_GRID
* @param int GRID_COLS
* */
void placeBlocksToGrid(SDL_Renderer * renderer, cJSON * blocksArr, SDL_Rect DEST_GRID, int GRID_COLS);
/** Kiválasztja az összes pályát az adott szinthez.
* A hívónak kötelessége felszabadítani a lefoglalt memóriaterületet cJSON_Delete függvénnyel.
* @param int level
* @return cJSON * selectedMaps
* */
cJSON * selectMapsForLevel(int level);
/** Kiválasztja a megfelelő számú (5) darab pályát a szintfájlban tároltak közül.
*  Még nincsen kész teljesen.
* @param cJSON * allMap
* @param int * selectedNum
* @return cJSON *allMap
* */
cJSON * selectRandomMaps(cJSON * allMap, int * selectedNum);
/**
* @param cJSON *structure
* @param bool val
* */
void setGameInitalizationStatus(cJSON *structure, bool val);
/**
* @param cJSON *structure
* @param int level
* */
void setLevel(cJSON *structure, int level);
/** Elindítja a játékot, úgy hogy előszőr betölti a szintet az actual.json fájlból, majd kiválasztaja az aktuális
* pálylákat, és kirajzolja az elsőt. Amennyiben a játék sikeresen elindult, a pálya kirajzolódott true-val tér vissza, különben valamilyen
* hiba esetén false-sal.
* @param SDL_Renderer * renderer
* @return Page isSucceed
* */
Page startGame(SDL_Renderer *renderer);

#endif // GAMESTARTER_H_INCLUDED
