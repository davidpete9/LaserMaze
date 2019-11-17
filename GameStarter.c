#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "MenuEngine.h"
#include "Constanses.h"
#include "FileHandler.h"
#include "cJSON.h"
#include "debugmalloc.h"

char * generateFileNameStringByLevel(int level) {
        int length = (int) floor(log10(level))+strlen(MAPS_FILENAME_FORMAT);
        char * filename;
        filename = (char *) malloc(length*sizeof(char));
        sprintf(filename, MAPS_FILENAME_FORMAT, level);
        return filename;
}

void drawBlock(SDL_Renderer * renderer, cJSON * block) {
int xCoord = cJSON_GetObjectItem(block, MAP_BLOCKS_COL)->valueint;
int yCoord = cJSON_GetObjectItem(block, MAP_BLOCKS_ROW)->valueint;
int squere_w = (int) TABLE_RECT.w/GRID_W;
SDL_Rect block_place = (SDL_Rect) {TABLE_RECT.x+(xCoord-1)*squere_w, TABLE_RECT.y+(yCoord-1)*squere_w, squere_w, squere_w};
SDL_SetRenderDrawColor(renderer, 0,255,0,255);
SDL_RenderFillRect(renderer, &block_place);

}

void placeBlocks(SDL_Renderer * renderer, cJSON * blocksArr) {

    for (int i = 0; i < cJSON_GetArraySize(blocksArr); i++) {
        cJSON *block = cJSON_GetArrayItem(blocksArr, i);
        drawBlock(renderer, block);
        cJSON_Delete(block);
    }
}

void initializeGame(SDL_Renderer *renderer, int level) {
     char * filename = generateFileNameStringByLevel(level);
     cJSON * allMap = getParsedJSONContentOfFile(filename);
     if (allMap != NULL) {
      cJSON * actualMaps = cJSON_GetObjectItem(allMap, MAP_ARRAY);
      cJSON * firstMap = cJSON_GetArrayItem(actualMaps, 0);
      cJSON * firstMapBlocks = cJSON_GetObjectItem(firstMap, MAP_BLOCKS_ARRAY);
       placeBlocks(renderer, firstMapBlocks);

      printItForDebugging(firstMapBlocks);

     cJSON_Delete(firstMapBlocks);
     cJSON_Delete(firstMap);
     cJSON_Delete(actualMaps);
     cJSON_Delete(allMap);
     }
     free(filename);
}

void drawGameTable(SDL_Renderer *renderer, int level) {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDrawRect(renderer, &TABLE_RECT);
    drawGrid(renderer);
}

void drawGrid(SDL_Renderer * renderer) {
    int squere_w = (int) TABLE_RECT.w/GRID_W;
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    for (int i = 1; i < GRID_W; i++) {
        SDL_RenderDrawLine(renderer, TABLE_RECT.x+(i*squere_w), TABLE_RECT.y, TABLE_RECT.x+(i*squere_w), TABLE_RECT.y+TABLE_RECT.h);
    }
    int squere_h = (int) TABLE_RECT.h/GRID_H;
    for (int i = 1; i < GRID_H; i++) {
        SDL_RenderDrawLine(renderer, TABLE_RECT.x, TABLE_RECT.y+(i*squere_h), TABLE_RECT.x+TABLE_RECT.w, TABLE_RECT.y+(i*squere_h));
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

void startGame(SDL_Renderer *renderer) {
    cJSON * currentData = getParsedJSONContentOfFile(ACTUAL_STATUS_FILE_NAME);
    if (currentData == NULL) {
        printf("A játék nem indítható...");
        exit(1);
    }
    bool isGameInitialized = cJSON_GetObjectItem(currentData,"isGameInitialized")->valueint ? true : false;
    int level = cJSON_GetObjectItem(currentData,LEVEL_STR)->valueint;
    if (!isGameInitialized) {
        initializeGame(renderer, level);
        drawGameTable(renderer, level);
        SDL_RenderPresent(renderer);
    }
    else {
       //todo: mikor betoltjuk a megkezdett jatekot
    }

    cJSON_Delete(currentData);
}


/*
 if (isGameInitialized())
 int lvl = getLevel();
 //if (isLevelAccessable) {}
 maps = get5randomMap() {
    getAllMapId() {
        readGameFile - gameFileHandler
        malloc (int n*sizeof(int))    }
    }
    selectRandom5Item()


 }

  saveThemToActualFile(maps)


  drawFirstMap(map) {
    ....
  }

*/

