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
#include "cJSON.h"
#include "debugmalloc.h"

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

void initCell(bool isLeft, int block_id, int rotation, Cell *cell) {
    cell->block_id = block_id;
    cell->isMoveable = !isLeft;
    cell->isPlacedByUser = false;
    cell->rotation = rotation;
    cell->isRotatable = !isLeft;
    cell->isLeftSide = isLeft;
    cell->hasLaserTouchedIt = false;
    cell->display = true;
}

Cell **initGridStructure(const cJSON *mapdata) {
    cJSON *leftBlocks = cJSON_GetObjectItem(mapdata, MAP_BLOCKS_ARRAY);
    cJSON *rightBlocks = cJSON_GetObjectItem(mapdata, PLACEABLE_BLOCK_ARRAY);
    Cell **grid;
    grid = (Cell **) malloc(GRID_W * sizeof(Cell));

    grid[0] = (Cell *) malloc((GRID_W * (GRID_W + 1)) * sizeof(Cell));
    for (int i = 1; i < GRID_W; i++) {
        grid[i] = grid[0] + (i * (GRID_W + 1));
    }
    for (int i = 0; i < GRID_W; i++) {
        for (int j = 0; j <= GRID_W; j++) {
            grid[i][j].block_id = -1;
        }
    }

    for (int i = 0; i < cJSON_GetArraySize(leftBlocks); i++) {
        cJSON *block = cJSON_GetArrayItem(leftBlocks, i);
        int col = cJSON_GetObjectItem(block, MAP_BLOCKS_COL)->valueint;
        int row = cJSON_GetObjectItem(block, MAP_BLOCKS_ROW)->valueint;
        initCell(true,cJSON_GetObjectItem(block, MAP_BLOCKS_ID)->valueint,
                 cJSON_GetObjectItem(block, MAP_BLOCKS_ROTATION)->valueint,
                 &grid[row-1][col-1]);
    }

    for (int i = 0; i < cJSON_GetArraySize(rightBlocks); i++) {
        cJSON *block = cJSON_GetArrayItem(rightBlocks, i);

        int row = cJSON_GetObjectItem(block, MAP_BLOCKS_ROW)->valueint;
        initCell(false,cJSON_GetObjectItem(block, MAP_BLOCKS_ID)->valueint,
                 cJSON_GetObjectItem(block, MAP_BLOCKS_ROTATION)->valueint,
                 &grid[row-1][GRID_W]);
    }

    return grid;
}

/** Kiválasztja a megfelelő számú (5) darab pályát a szintfájlban tároltak közül.
 *  Még nincsen kész teljesen.
 * @param cJSON * allMap
 * @param int * selectedNum
 * @return cJSON *allMap
 * */
cJSON *selectRandomMaps(cJSON *allMap) {
    int length = cJSON_GetArraySize(allMap);
    if (length == 0) return NULL;
    if (length <= MAPS_IN_A_LEVEL) {
        return allMap;
    }
    return allMap;
    //TODO: SELECT RANDOM 5
}


//!!TEMP!!!!!! FREE, malloc djgkdj
/** Kiválasztja az összes pályát az adott szinthez.
 * A hívónak kötelessége felszabadítani a lefoglalt memóriaterületet cJSON_Delete függvénnyel.
 * @param int level
 * @return cJSON * selectedMaps
 * */
cJSON *selectMapsForLevel(int level) {
    char *filename = generateFormattedStringFromNumber(level, MAPS_FILENAME_FORMAT);
    cJSON *allMap = getParsedJSONContentOfFile(filename);
    free(filename);
    if (allMap == NULL) return NULL;
    cJSON *selectedMaps = selectRandomMaps(allMap);
    return selectedMaps;
}

//tofo wtf...
/**
 * @param cJSON *structure
 * @param bool val
 * */
void setGameInitalizationStatus(cJSON *structure, bool val) {
    cJSON_AddItemToObject(structure, IS_GAME_INITIALIZED, cJSON_CreateNumber(val ? 1 : 0));
}

/**
 * @param cJSON *structure
 * @param int level
 * */
void setLevel(cJSON *structure, int level) {
    cJSON_AddItemToObject(structure, LEVEL_STR, cJSON_CreateNumber((int) level));
}


/** Az actual.json fájlt létrehozza, és elmenti benne az aktuális levelt.
 * @param int level
 * */
void initializeFileWithLevel(int level) {
    FILE *fp = createNewFile(ACTUAL_STATUS_FILE_NAME);
    cJSON *structure = cJSON_CreateObject();

    setGameInitalizationStatus(structure, false);
    setLevel(structure, level);

    printStructureIntoFileAndClose(fp, structure);
}

/** Elindítja a játékot, úgy hogy előszőr betölti a szintet az actual.json fájlból, majd kiválasztaja az aktuális
 * pálylákat, és kirajzolja az elsőt. Amennyiben a játék sikeresen elindult, a pálya kirajzolódott true-val tér vissza, különben valamilyen
 * hiba esetén false-sal.
 * @param SDL_Renderer * renderer
 * @return Page isSucceed
 * */
Page startGame(SDL_Renderer *renderer) {
    cJSON *currentData = getParsedJSONContentOfFile(ACTUAL_STATUS_FILE_NAME);
    if (currentData == NULL) return -1;

    bool isGameInitialized = cJSON_GetObjectItem(currentData, IS_GAME_INITIALIZED)->valueint ? true : false;
    int level = cJSON_GetObjectItem(currentData, LEVEL_STR)->valueint;

    if (!isGameInitialized) {

        cJSON *maps = selectMapsForLevel(level);
        if (maps == NULL) return -1;
        /*IDEIGLENES MEGOLDÁS, CSAK A FÉLKÉSZ ÁLLAPOTHOZ, HOGY KIRAJZOLJON EGY PÁLYÁT*/
        cJSON *firstMap = cJSON_GetArrayItem(maps, 0);
        ButtonRect **buttons = createButtonsForCurrentPage(inGame);
        Cell **grid = initGridStructure(firstMap);

        GameEvent next = runGameEvents(renderer, grid, buttons);
        while (next != leave) {
            switch (next) {
                case reset:
                    free(grid[0]);
                    free(grid);
                    grid = initGridStructure(firstMap);
                    next = runGameEvents(renderer, grid, buttons);;
                    break;
                case fire:
                    runLaser(renderer, grid);
                    next = runGameEvents(renderer, grid, buttons);
                    break;
                default: //TEMP
                    next = leave;
            }
        }
        free(grid[0]);
        free(grid);
        resetScreenAndFreeButtonsArray(renderer, buttons, inGame);
        cJSON_Delete(currentData);
        //TEMP
        cJSON_Delete(maps);
        return gameMenu;
    } else {
        /*
        Ebben az esetben fogja a rendszer betolteni az előző mentést.
        */
        return gameMenu;
    }
}
