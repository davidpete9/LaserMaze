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
#include "cJSON.h"
#include "debugmalloc.h"


/** Egy számot beilleszt egy adott formátumú stringbe.
 * A hívó felelőssége a kapott stringet által foglalt memóriaterületet felszabadítani.
 * @param int num
 * @param const char *format
 * @return char * formatted
 * */
char * generateFormattedStringFromNumber(int num, const char * format) {
        int length = (int) floor(log10(num))+strlen(format)+1;
        char * formatted;
        formatted = (char *) malloc(length*sizeof(char));
        sprintf(formatted, format, num);
        return formatted;
}

SDL_Texture * getBlockTexture(SDL_Renderer *renderer, int blockId) {
    char *filename = generateFormattedStringFromNumber(blockId, BLOCKS_TEXTURE_FILENAME_FORMAT);
    SDL_Texture * blockImg = IMG_LoadTexture(renderer, filename);
    free(filename);
    return blockImg;
}

void drawBlock(SDL_Renderer * renderer, cJSON * block, SDL_Rect DEST, int GRID_COLS) {
    int xCoord = cJSON_GetObjectItem(block, MAP_BLOCKS_COL)->valueint;
    int yCoord = cJSON_GetObjectItem(block, MAP_BLOCKS_ROW)->valueint;
    int squere_w = (int) DEST.w/GRID_COLS;

    SDL_Rect block_place = (SDL_Rect) {DEST.x+(xCoord-1)*squere_w, DEST.y+(yCoord-1)*squere_w, squere_w, squere_w};
    //SDL_SetRenderDrawColor(renderer, 0,255,0,255);
    SDL_Texture * imgTexture = getBlockTexture(renderer, cJSON_GetObjectItem(block, MAP_BLOCKS_ID)->valueint);
    if (!imgTexture) {
        SDL_SetRenderDrawColor(renderer, 0,255,0,120);
        SDL_RenderFillRect(renderer, &block_place);
    }
    else {
        int rotationAngle = cJSON_GetObjectItem(block, MAP_BLOCKS_ROTATION)->valueint*90;
        SDL_RenderCopyEx(renderer, imgTexture, NULL, &block_place, rotationAngle, NULL, SDL_FLIP_NONE);
        SDL_DestroyTexture(imgTexture);
    }
}

void placeBlocksToGrid(SDL_Renderer * renderer, cJSON * blocksArr, SDL_Rect DEST_GRID, int GRID_COLS) {
    for (int i = 0; i < cJSON_GetArraySize(blocksArr); i++) {
        cJSON *block = cJSON_GetArrayItem(blocksArr, i);
        drawBlock(renderer, block, DEST_GRID, GRID_COLS);
        cJSON_Delete(block);
    }

}

cJSON * selectRandomMaps(cJSON * allMap, int * selectedNum) {
    int length = cJSON_GetArraySize(allMap);
    if (length == 0) return NULL;
    if (length <= MAPS_IN_A_LEVEL) {
        *selectedNum = length;
        return allMap;
    }
    //TEMP!!!!
    *selectedNum = length;
    return allMap;
    //TODO: SELECT RANDOM 5
}

cJSON * selectMapsForLevel(SDL_Renderer *renderer, int level) {
     char * filename = generateFormattedStringFromNumber(level, MAPS_FILENAME_FORMAT);
     cJSON * allMap = getParsedJSONContentOfFile(filename);
     free(filename);
     if (allMap == NULL) return NULL;
     int mapsSelectedNum = 0;
     cJSON * selectedMaps = selectRandomMaps(allMap, &mapsSelectedNum);
     cJSON_Delete(allMap);
     return selectedMaps;
}

void drawGameTable(SDL_Renderer *renderer, int level) {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDrawRect(renderer, &TABLE_RECT);
    SDL_RenderDrawRect(renderer, &RIGHT_SIDE_RECT);
    drawGrid(renderer);
}

void drawGrid(SDL_Renderer * renderer) {
    int squere_w = (int) TABLE_RECT.w/GRID_W;
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    for (int i = 1; i < GRID_W; i++) {
        SDL_RenderDrawLine(renderer, TABLE_RECT.x+(i*squere_w), TABLE_RECT.y, TABLE_RECT.x+(i*squere_w), TABLE_RECT.y+TABLE_RECT.h);
        SDL_RenderDrawLine(renderer, TABLE_RECT.x, TABLE_RECT.y+(i*squere_w), TABLE_RECT.x+TABLE_RECT.w, TABLE_RECT.y+(i*squere_w));

        SDL_RenderDrawLine(renderer, RIGHT_SIDE_RECT.x, RIGHT_SIDE_RECT.y+(i*squere_w), RIGHT_SIDE_RECT.x+RIGHT_SIDE_RECT.w, RIGHT_SIDE_RECT.y+(i*squere_w));
    }
}

void setGameInitalizationStatus(cJSON *structure, bool val) {
    cJSON_AddItemToObject(structure, IS_GAME_INITIALIZED, cJSON_CreateNumber(val ? 1 : 0));
}

void setLevel(cJSON *structure, int level) {
    cJSON_AddItemToObject(structure, LEVEL_STR, cJSON_CreateNumber((int)level));
}

void initializeFileWithLevel(int level) {
    FILE *fp = createNewFile(ACTUAL_STATUS_FILE_NAME);
    cJSON * structure = cJSON_CreateObject();

    setGameInitalizationStatus(structure, false);
    setLevel(structure, level);

    printStructureIntoFileAndClose(fp, structure);
}


/** Elindítja a játékot, úgy hogy előszőr betölti a szintet az actual.json fájlból, majd kiválasztaja az aktuális
 * pálylákat, és kirajzolja az elsőt. Amennyiben a játék sikeresen elindult, a pálya kirajzolódott true-val tér vissza, különben valamilyen
 * hiba esetén false-sal.
 * @param SDL_Renderer * renderer
 * @return bool isSucceed
 * */
bool startGame(SDL_Renderer *renderer) {
    cJSON * currentData = getParsedJSONContentOfFile(ACTUAL_STATUS_FILE_NAME);
    if (currentData == NULL) return false;

    bool isGameInitialized = cJSON_GetObjectItem(currentData,IS_GAME_INITIALIZED)->valueint ? true : false;
    int level = cJSON_GetObjectItem(currentData,LEVEL_STR)->valueint;

    if (!isGameInitialized) {
        cJSON * maps = selectMapsForLevel(renderer, level);
        if (maps == NULL) return false;
        drawGameTable(renderer, level);
        /*IDEIGLENES MEGOLDÁS, CSAK A FÉLKÉSZ ÁLLAPOTHOZ, HOGY KIRAJZOLJON EGY PÁLYÁT*/
        cJSON *firstMap = cJSON_GetArrayItem(maps, 0);
        cJSON *firstMapBlockArr = cJSON_GetObjectItem(firstMap, MAP_BLOCKS_ARRAY);
        placeBlocksToGrid(renderer, firstMapBlockArr, TABLE_RECT, GRID_W);
        cJSON *rightSideBlockArr  = cJSON_GetObjectItem(firstMap, PLACEABLE_BLOCK_ARRAY);
        placeBlocksToGrid(renderer, rightSideBlockArr, RIGHT_SIDE_RECT, 1);

        cJSON_Delete(rightSideBlockArr);
        cJSON_Delete(firstMap);
        cJSON_Delete(firstMapBlockArr);
        SDL_RenderPresent(renderer);
    }
    else {
       /*
       Ebben az esetben fogja a rendszer betolteni az előző mentést.
       */
    }
    cJSON_Delete(currentData);

    return true;
}

