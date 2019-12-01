#include <Windows.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <SDL2/SDL_image.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "MenuEngine.h"
#include "Constanses.h"
#include "FileHandler.h"
#include "GameboardEvents.h"
#include <time.h>
#include "cJSON.h"
#include "debugmalloc.h"
#include "GameMenuControl.h"

/** Egy számot beilleszt egy adott formátumú stringbe.
 * A hívó felelőssége a kapott stringet által foglalt memóriaterületet felszabadítani.
 * @param int num
 * @param const char *format
 * @return char * formatted
 * */
char *generateFormattedStringFromNumber(int num, const char *format) {
    int length = (int) floor(log10(num)) + strlen(format) + 1;
    char *formatted;
    formatted = (char *) malloc(length * sizeof(char));
    sprintf(formatted, format, num);
    return formatted;
}

void printGridForDebug(Cell **grid) {
    printf("\n");
    for (int i = 0; i < GRID_W; i++) {

        for (int j = 0; j <= GRID_W; j++) {
            printf(" {[%d|%d] ", i + 1, j + 1);
            if (grid[i][j].block_id != -1) {
                if (grid[i][j].isLeftSide) {
                    printf("[bal oldal]");
                } else {
                    printf("[jobb oldal]");
                }
                printf("[blockId: %d]", grid[i][j].block_id);
            } else {
                printf("[-]");
            }
            printf("}\t");
        }
        printf("\n");
    }
}


/** Beállítja a kezdőértékeit egy cím szerint átvett mezőnek (Cell).
 * @param bool isLeft
 * @oaram int block_id
 * @param int rotation
 * @param Cell *cell
 * */
void initCell(bool isLeft, int block_id, int rotation, Cell *cell) {
    cell->block_id = block_id;
    cell->isMoveable = !isLeft;
    cell->isPlacedByUser = false;
    cell->rotation = rotation;
    cell->isRotatable = !isLeft;
    cell->isLeftSide = isLeft;
    cell->hasLaserTouchedIt = false;
    cell->laserTouchedItDir = nowhere;
    cell->display = true;
}

/** Létrehozza a pálya adatait tartalmazó 2 dimenziós tömböt (táblázatot), melyben leírja,
 * hogy melyik mezőben milyen típusú blokk található. Ezt a kapott cJSON típusú változóból állítja elő, ami egy teljes pálya
 * összes adata. A hívó felelőssége felszabadítani a lefogalat memóriaterületet.
 * @param cJSON *mapData
 * @param Cell **grid
 * */
Cell **initGridStructure(cJSON *mapdata) {
    cJSON *leftBlocks = cJSON_GetObjectItem(mapdata, MAP_BLOCKS_ARRAY);
    cJSON *rightBlocks = cJSON_GetObjectItem(mapdata, PLACEABLE_BLOCK_ARRAY);
    Cell **grid;
    grid = (Cell **) malloc(GRID_W * sizeof(Cell));

    grid[0] = (Cell *) malloc((GRID_W * (GRID_W + 1)) * sizeof(Cell));
    for (int i = 1; i < GRID_W; i++) {
        grid[i] = grid[0] + (i * (GRID_W + 1));
    }
    for (int i = 0; i < GRID_W; i++) {
        for (int j = 0; j <= GRID_W; j++) grid[i][j].block_id = -1;
    }

    for (int i = 0; i < cJSON_GetArraySize(leftBlocks); i++) {
        cJSON *block = cJSON_GetArrayItem(leftBlocks, i);
        int col = cJSON_GetObjectItem(block, MAP_BLOCKS_COL)->valueint;
        int row = cJSON_GetObjectItem(block, MAP_BLOCKS_ROW)->valueint;
        initCell(true, cJSON_GetObjectItem(block, MAP_BLOCKS_ID)->valueint,
                 cJSON_GetObjectItem(block, MAP_BLOCKS_ROTATION)->valueint,
                 &grid[row - 1][col - 1]);
    }

    for (int i = 0; i < cJSON_GetArraySize(rightBlocks); i++) {
        cJSON *block = cJSON_GetArrayItem(rightBlocks, i);

        int row = cJSON_GetObjectItem(block, MAP_BLOCKS_ROW)->valueint;
        initCell(false, cJSON_GetObjectItem(block, MAP_BLOCKS_ID)->valueint,
                 cJSON_GetObjectItem(block, MAP_BLOCKS_ROTATION)->valueint,
                 &grid[row - 1][GRID_W]);
    }

    return grid;
}

/** Segédfüggvény, megnézi, hogy a tömb tartalmazza e az adott elemet
 * @param int *arr
 * @param int length
 * @param int n
 * */
bool isArrayIncludes(int *arr, int length, int n) {
    for (int i = 0; i < length; i++) {
        if (arr[i] == n) return true;
    }
    return false;
}

/** Kiválasztja véletlenszerően 5 (MAPS_IN_LEVEL) pályq indexét, qmelyek az adott szinteten játszandó pályákat fogják azonosítani
 * a fájlból beolvasott tömbben (cJSON struktúrában). Amennyiben nincs 5 pálya a szinten, annyi pályát fog kiválasztani
 * véletlenszerű sorrendben, ahány van.
 * A cím szerint átvett array_length változóba írja a létrejött tömb méretét.
 * A hívó kötelessége felszabadítani a kapott tömbre mutató memóriaterületet.
 * @param int allMapsNum
 *  @param int *array_length
 *  @return int * random_indexes
 * */
int *selectRandomIndexes(int allMapsNum, int *array_length) {
    if (allMapsNum <= 0) return NULL;
    srand(time(0));

    *array_length = allMapsNum >= MAPS_IN_A_LEVEL ? MAPS_IN_A_LEVEL : allMapsNum;

    int *random_indexes = (int *) malloc((*array_length) * sizeof(int));
    for (int i = 0; i < *array_length; i++) random_indexes[i] = -1;
    for (int i = 0; i < *array_length; i++) {
        int r_num = rand() % *array_length;
        while (isArrayIncludes(random_indexes, *array_length, r_num)) r_num = rand() % *array_length;
        random_indexes[i] = r_num;
    }
    return random_indexes;
}

/** Kiválasztja az összes pályát az adott szinthez.
 * A hívónak kötelessége felszabadítani a lefoglalt memóriaterületet cJSON_Delete függvénnyel.
 * @param int level
 * @return cJSON * selectedMaps
 * */
cJSON *getMapsForLevel(int level) {
    char *filename = generateFormattedStringFromNumber(level, MAPS_FILENAME_FORMAT);
    cJSON *allMap = getParsedJSONContentOfFile(filename);
    free(filename);
    return allMap;
}

/** Az actual.json fájlt létrehozza, és elmenti benne az aktuális levelt.
 * @param int level
 * */
void initializeFileWithLevel(int level) {

    cJSON *structure = cJSON_CreateObject();
    cJSON_AddItemToObject(structure, IS_GAME_INITIALIZED, cJSON_CreateNumber(0));
    cJSON_AddItemToObject(structure, LEVEL_STR, cJSON_CreateNumber((int) level));

    printStructureIntoFileAndClose(ACTUAL_STATUS_FILE_NAME, structure);
}

/** Megnézi, hogy jó e a pálya megoldása, úgy hogy érintett e minden blokkot a lézer, amint érintenie kellett
 * vagyis azokat, amiket nem a játékos rakott le.
 * @param Cell **grid
 * @return bool isSucceed
 * */
bool checkSolution(Cell **grid) {
    for (int i = 0; i < GRID_W; i++) {
        for (int j = 0; j < GRID_W; j++) {
            if (grid[i][j].block_id != -1 && grid[i][j].display && !grid[i][j].hasLaserTouchedIt &&
                grid[i][j].block_id != BRICK) {
                if (!grid[i][j].isPlacedByUser) return false;
            }
        }
    }
    return true;
}

/** Amennyiben van már megkezdett játék, visszatér annak a szintjével, különben -1 -gyel tér vissza.
 * @return int level
 * */
int getInitializedGameLevelIfExists() {
    cJSON *currentData = getParsedJSONContentOfFile(ACTUAL_STATUS_FILE_NAME);
    if (currentData == NULL) return -1;
    if (cJSON_GetObjectItem(currentData, IS_GAME_INITIALIZED)->valueint == 1) {
        return cJSON_GetObjectItem(currentData, LEVEL_STR)->valueint;
    }
    return -1;

}

/**Visszaállítja alapértelmezett állapotba a cím szerint átvett táblázatnak a
 * lézersugárra vonatkozó adatait, miután a lézerrel lőtt a játékos .
 * @param Cell **grid
 * */
void resetGridAfterShot(Cell **grid) {
    for (int i = 0; i < GRID_W; i++) {
        for (int j = 0; j < GRID_W; j++) {
            grid[i][j].hasLaserTouchedIt = false;
            grid[i][j].laserTouchedItDir = nowhere;
        }
    }
}
//todo áthleyezni?
/**
 * Felszabadítja rekrúzív módszerrel a a lézerfény útját tartalmazó bináris fát.
 * @param LaserPath
 * */
void freeTree(LaserPath *r) {
    if (r == NULL) return;
    freeTree(r->next);
    freeTree(r->next2);
    free(r);
}

/** Felszabadítja a táblázatot.
 * @param Cell **grid
 * */
void freeGrid(Cell **grid) {
    free(grid[0]);
    free(grid);
}

/** Beállíja a közetkező pálya adatait a megfelelő, cím szerintt átvett adatokat.
 * a maps cJSON struktúra-töb kapott indexű eleme lesz az új pálya.
 * @param cJSON *maps
 * @param int mapInd
 * @param Cell ***grid
 * @param cJSON **actualMap
 * */
void selectAndLoadNextMap(cJSON *maps, int mapInd, Cell ***grid, cJSON **actualMap) {
    if (maps == NULL) return;
    if (*grid != NULL) freeGrid(*grid);
    *actualMap = cJSON_GetArrayItem(maps, mapInd);
    *grid = initGridStructure(*actualMap);
}

StringToDisplay * getGameCounterObj(SDL_Renderer *renderer, int level, int mapInd, int size, int mapId) {
    char  * stringToShow = (char *) malloc(50*sizeof(char));
    int len = sprintf(stringToShow, "Szint: %d, Pálya: %d/%d, Pálya id: %d", level, mapInd, size, mapId);
    stringToShow[len] = '\0';
    StringToDisplay * msg = (StringToDisplay*) malloc(sizeof(StringToDisplay));
    msg->fontSize = 16;
    msg->pos = (SDL_Rect) {550, 5, 300, 50};
    msg->title = stringToShow;
    msg->titleColor = (SDL_Color) {255, 255, 255, 255};
    return msg;
}

/** A képernyő tetejére kiírja a kapott üzenetet.
 * @param SDL_Renderer *renderer
 * @param char *text
 * @bool isSuccess
 * */
void writeMessageToTop(SDL_Renderer *renderer, char *text, bool isSuccess) {
    StringToDisplay msg;
    msg.fontSize = 20;
    msg.pos = (SDL_Rect) {100, 0, 400, 50};
    msg.title = text;
    msg.titleColor = isSuccess ? (SDL_Color) {0, 255, 0, 255} : (SDL_Color) {255, 0, 0, 255};

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderFillRect(renderer, &msg.pos);

    writeTextToDisplay(renderer, &msg);
    SDL_RenderPresent(renderer);
}


/** Beálltja, hogy milyen jutalmakat kap a játékos a teljesítménye alapján.
 * Beállítja a cím szerint átvett változók értékeit.
 * Egy meglévő eredmény rosszabbal nem ír felül.
 * @param cJSON *result
 * @param usedSkip
 * @param elapsedTime
 * */
void setBadges(cJSON *result, bool usedSkip, int elapsedTime) {
    bool hasTimeBadge = cJSON_GetObjectItem(result, HAS_TIME_BONUS_BADGE)->valueint == 1 ? true : false;
    bool hasNoSkipBadge = cJSON_GetObjectItem(result, HAS_NO_SKIP_BADGE)->valueint == 1 ? true : false;
    //Ha mar megszerezte előzőleg mina kettőt
    if (hasTimeBadge && hasNoSkipBadge) return;
    //Ha semelyik badget sem kapná meg
    if (usedSkip && elapsedTime > TIME_BADGE_GOT_BELOW) return;
    if (!usedSkip) {
        cJSON_DeleteItemFromObject(result, HAS_NO_SKIP_BADGE);
        cJSON_AddNumberToObject(result, HAS_NO_SKIP_BADGE, 1);
    }
    if (elapsedTime <= TIME_BADGE_GOT_BELOW && !usedSkip) {
        cJSON_DeleteItemFromObject(result, HAS_TIME_BONUS_BADGE);
        cJSON_AddNumberToObject(result, HAS_TIME_BONUS_BADGE, 1);
    }
}

/** Beállítja egyy üres, cím szerint átvett cJSON struktúra alapértelmezett érékeit úgy,
 * hogy az a levels.json fájlnak megjeleljen.
 * @param cJSON *result
 * @param int level
 * @param int elapsedTime
 * */
static void initEmptyResultObj(cJSON *result, int level, int elapsedTime) {
    cJSON_AddNumberToObject(result, LEVEL_STR, level);
    cJSON_AddNumberToObject(result, ELAPSED_TIME_STR, elapsedTime);
    cJSON_AddNumberToObject(result, HAS_NO_SKIP_BADGE, 0);
    cJSON_AddNumberToObject(result, HAS_TIME_BONUS_BADGE, 0);
}

/**Elmenti a leves.json fájlba a előző szint teljesítésének az eredményeit.
 * Ha jobb idő alatt teljesítette a játékos az adott pályát, akkor az eredményét azzal felülírja.
 * @param int level
 * @param int elapsedTime
 * @oaram bool usedSkip
 * @return cJSON * result
 * */
cJSON *saveResultToFile(int level, int elapsedTime, bool usedSkip) {
    cJSON *finishedLevels = getParsedJSONContentOfFile(LEVELS_FILE_NAME);
    if (finishedLevels == NULL) {
        finishedLevels = cJSON_CreateArray();
    }

    bool hasAlreadyDone = false;
    cJSON *result;
    for (int i = 0; i < cJSON_GetArraySize(finishedLevels); i++) {
        cJSON *currentItem = cJSON_GetArrayItem(finishedLevels, i);
        if (cJSON_GetObjectItem(currentItem, LEVEL_STR)->valueint == level) {
            hasAlreadyDone = true;
            if (elapsedTime < cJSON_GetObjectItem(currentItem, ELAPSED_TIME_STR)->valuedouble) {
                //todo record = 1
                cJSON_DeleteItemFromObject(currentItem, ELAPSED_TIME_STR);
                cJSON_AddNumberToObject(currentItem, ELAPSED_TIME_STR, elapsedTime);
            }
            setBadges(currentItem, usedSkip, elapsedTime);
            result = currentItem;
        }
    }
    if (!hasAlreadyDone) {
        result = cJSON_CreateObject();
        initEmptyResultObj(result, level, elapsedTime);
        setBadges(result, usedSkip, elapsedTime);
        cJSON_AddItemToArray(finishedLevels, result);
    }
    printStructureIntoFileAndClose(LEVELS_FILE_NAME, finishedLevels);
    return result;


}

/**Segédfüggvény, egy cJSON struktúrából kimásol egy tömbbot.
 * A tömbre mutató memóriaterület felszabadítása a felhasználó felelőssége.
 * @param cJSON * obj
 * @param char * key
 * @param int *size
 * @return int *arr
 **/
int *getArrayFromObject(cJSON *obj, char *key, int *size) {

    cJSON *jsonArr = cJSON_GetObjectItem(obj, key);
    *size = cJSON_GetArraySize(jsonArr);

    int *arr = (int *) malloc(sizeof(int) * (*size));
    for (int i = 0; i < *size; i++) {
        arr[i] = cJSON_GetArrayItem(jsonArr, i)->valueint;
    }

    return arr;
}

/** Növeli az idő az actual.json fájl tartalmának megfelelő cJSON struktúrában.
 * @param cJSON *currentData
 * @param double elapsed
 * */
void incrementTime(cJSON *currentData, double elapsed) {
    if (cJSON_HasObjectItem(currentData, ELAPSED_TIME_STR)) {
        double current = cJSON_GetObjectItem(currentData, ELAPSED_TIME_STR)->valuedouble;
        cJSON_DeleteItemFromObject(currentData, ELAPSED_TIME_STR);
        elapsed += current;
    }
    cJSON_AddNumberToObject(currentData, ELAPSED_TIME_STR, elapsed);
}

/** Beállítja a kiválasztott pályák indexeit a cím szerint átvett cJSON struktúra megfelelő mezőjébe. (ami az acutal.json fájl tartalma)
 * @param cJSON *data
 * @param int* selectedMapIndexes
 * @param int size
 * */
void setSelectedNumbers(cJSON *currentData, int *selectedMapIndexes, int size) {
    cJSON *arr = cJSON_AddArrayToObject(currentData, SELECTED_MAP_INDEXES_ARR);
    for (int i = 0; i < size; i++) {
        cJSON_AddItemToArray(arr, cJSON_CreateNumber(selectedMapIndexes[i]));
    }
}

/** A játék futtatását kezeli.
 * todo
 * @param SDL_Renderer * renderer
 * @param cJSON *currentData
 * @pararm cJSON *maps
 * */
bool runGame(SDL_Renderer *renderer, cJSON *currentData, cJSON *maps) {
    int sizeOfMaps;
    int *selectedMapIndexes = getArrayFromObject(currentData, SELECTED_MAP_INDEXES_ARR, &sizeOfMaps);
    int *finishedMaps = cJSON_GetObjectItem(currentData, FINISHED_MAPS_ARR);
    int mapIndex = cJSON_GetArraySize(finishedMaps);
    if (mapIndex == sizeOfMaps) {
        free(selectedMapIndexes);
        return true;
    }
    StringToDisplay * statusStrObj;
    bool isGameFinished = false;
    ButtonRect **buttons = createButtonsForCurrentPage(inGame);
    int level = cJSON_GetObjectItem(currentData, LEVEL_STR)->valueint;
    bool canUseSkipButton = cJSON_GetObjectItem(currentData, USED_SKIP_BUTTON)->valueint == 1 ? false : true;
    cJSON *actualMap = NULL;
    Cell **grid = NULL;
    LaserPath *path;
    selectAndLoadNextMap(maps, selectedMapIndexes[mapIndex++], &grid, &actualMap);
    GameEvent next = restart;
    while (next != leave) {

        switch (next) {
            case reset:
                freeGrid(grid);
                grid = initGridStructure(actualMap);
                next = restart;
                break;
            case fire:
                path = runLaser(renderer, grid);
                if (path == NULL) {
                    writeMessageToTop(renderer, "Hiba! Nincsen lehelyezve a lézerlövő.", false);
                    Sleep(1500);
                    next = restart;
                    break;
                }
                if (checkSolution(grid)) {
                    writeMessageToTop(renderer, "Helyes megoldás. Következő pálya betöltése...", true);

                    if (mapIndex <= sizeOfMaps) {
                        cJSON_AddItemToArray(finishedMaps, cJSON_CreateNumber(selectedMapIndexes[mapIndex - 1]));
                        selectAndLoadNextMap(maps, selectedMapIndexes[mapIndex++], &grid, &actualMap);
                    }
                    next = mapIndex <= sizeOfMaps ? restart : finished;
                } else {
                    writeMessageToTop(renderer, "Hibás megoldás", false);
                    resetGridAfterShot(grid);
                    next = restart;
                }
                freeTree(path);
                Sleep(2500);
                break;
            case skip:
                if (canUseSkipButton || isUserDeveloper()) {
                    writeMessageToTop(renderer, "Pálya kihagyva, új pálya betöltése..", true);
                    if (mapIndex <= sizeOfMaps) {
                        cJSON_AddItemToArray(finishedMaps, cJSON_CreateNumber(selectedMapIndexes[mapIndex - 1]));
                        selectAndLoadNextMap(maps, selectedMapIndexes[mapIndex++], &grid, &actualMap);
                    }
                    next = mapIndex <= sizeOfMaps ? restart : finished;
                    cJSON_DeleteItemFromObject(currentData, USED_SKIP_BUTTON);
                    cJSON_AddNumberToObject(currentData, USED_SKIP_BUTTON, 1);
                    canUseSkipButton = false;
                    //Sleep(1000);
                } else {
                    next = restart;
                }
                break;
            case restart:
                statusStrObj = getGameCounterObj(renderer, level, mapIndex, sizeOfMaps,
                                          cJSON_GetObjectItem(actualMap, MAP_ID)->valueint);
                writeMessageToTop(renderer, "", true);

                time_t start_t, end_t;
                time(&start_t);
                next = runGameEvents(renderer, grid, buttons, statusStrObj);
                free(statusStrObj->title);
                free(statusStrObj);
                time(&end_t);
                incrementTime(currentData, difftime(end_t, start_t));
                break;
            case finished:
                isGameFinished = true;
                cJSON *result = saveResultToFile(level, cJSON_GetObjectItem(currentData, ELAPSED_TIME_STR)->valuedouble,
                                                 !canUseSkipButton);
                writeMessageToTop(renderer, "Szint teljesítve. Vissza a főmenübe...", true);
                next = beforeleave;
                Sleep(1500);
                break;
            case beforeleave:
                if (canUseSkipButton) cJSON_AddNumberToObject(currentData, USED_SKIP_BUTTON, 0);
                printStructureIntoFileAndClose(ACTUAL_STATUS_FILE_NAME, currentData);
                resetScreenAndFreeButtonsArray(renderer, buttons, inGame);
                free(selectedMapIndexes);
                freeGrid(grid);
                cJSON_Delete(currentData);
                cJSON_Delete(maps);
                next = leave;
                break;
            default:
                next = beforeleave;
        }
    }
    return isGameFinished;
}
//todo?? másik talán
/** Elindítja a játékot, úgy hogy előszőr betölti a szintet az actual.json fájlból, majd kiválasztaja az aktuális
 * pálylákat, és kirajzolja az elsőt. Amennyiben a játék sikeresen elindult, a pálya kirajzolódott true-val tér vissza, különben valamilyen
 * hiba esetén false-sal.
 * @param SDL_Renderer * renderer
 * @return Page isSucceed
 * */
Page startGame(SDL_Renderer *renderer) {
    cJSON *currentData = getParsedJSONContentOfFile(ACTUAL_STATUS_FILE_NAME);
    if (currentData == NULL) return -1;
    int level = cJSON_GetObjectItem(currentData, LEVEL_STR)->valueint;
    cJSON *maps = getMapsForLevel(level);
    if (maps == NULL) {
        writeMessageToTop(renderer, "Nem sikerült a pálya betöltése. Visszairányítás...", false);
        Sleep(2000);
        return gameMenu;
    }
    printItForDebugging(currentData);
    int isGameInitialized = cJSON_GetObjectItem(currentData, IS_GAME_INITIALIZED)->valueint;
    if (isGameInitialized == 0) {
        cJSON_DeleteItemFromObject(currentData, IS_GAME_INITIALIZED);
        cJSON_AddNumberToObject(currentData, IS_GAME_INITIALIZED, 1);

        int size;
        int *selectedMapIndexes = selectRandomIndexes(cJSON_GetArraySize(maps), &size);
        setSelectedNumbers(currentData, selectedMapIndexes, size);
        free(selectedMapIndexes);

        cJSON_AddArrayToObject(currentData, FINISHED_MAPS_ARR);
        cJSON_AddNumberToObject(currentData, USED_SKIP_BUTTON, 0);
        cJSON_AddNumberToObject(currentData, ELAPSED_TIME_STR, 0);
    }
    bool isGameFinished = runGame(renderer, currentData, maps);

    return isGameFinished ? levelFinished : gameMenu;
}
