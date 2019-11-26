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

typedef struct Cell {
    int block_id;
    bool isPlacedByUser;
    bool isRotatable;
    bool isMoveable;
    int rotation;
    bool isLeftSide;
    bool display;
} Cell;

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


/** Visszatér az adott blokk textúrájával, amit fájlból töltött be.
 * A hívó felelőssége ezt késöbb SDL_DestroyTexture()-rel törölni.
 * @param SDL_Renderer *renderer,
 * @param int blockId
 * @return SDL_Texture * blockImg
 * */
SDL_Texture *getBlockTexture(SDL_Renderer *renderer, int blockId) {
    char *filename = generateFormattedStringFromNumber(blockId, BLOCKS_TEXTURE_FILENAME_FORMAT);
    SDL_Texture *blockImg = IMG_LoadTexture(renderer, filename);
    free(filename);
    return blockImg;
}

void initCell(bool isLeft, int block_id, int rotation, Cell *cell) {
    cell->block_id = block_id;
    cell->isMoveable = !isLeft;
    cell->isPlacedByUser = false;
    cell->rotation = rotation;
    cell->isRotatable = !isLeft;
    cell->isLeftSide = isLeft;
    cell->display = true;
}

Cell **initGridStructure(cJSON *blockArr, cJSON *rightBlocks) {

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

    for (int i = 0; i < cJSON_GetArraySize(blockArr); i++) {
        cJSON *block = cJSON_GetArrayItem(blockArr, i);
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

void drawMoveing(SDL_Renderer *renderer, int x, int y, int blockId) {
    int squaer_w = getOneSquareW();
    SDL_Rect to = (SDL_Rect) {x-((int)(squaer_w/2)), y-((int)(squaer_w/2)), squaer_w, squaer_w};
    SDL_Texture *imgTexture = getBlockTexture(renderer, blockId);
    SDL_RenderCopy(renderer, imgTexture, NULL, &to);
    SDL_DestroyTexture(imgTexture);
}

void drawCurrentGrid(SDL_Renderer *renderer, Cell **grid, ButtonRect **buttons) {
    for (int i = 0; i < GRID_W; i++) {
        for (int j = 0; j <= GRID_W; j++) {
            if (grid[i][j].block_id != -1 && grid[i][j].display) {
                drawBlock(renderer, grid[i][j], i, j);
            }
        }
    }
}

//todo temp
bool isClickedOnRect(SDL_Rect *actualPlace, int x, int y) {
    return actualPlace->x <= x && actualPlace->x + actualPlace->w >= x && actualPlace->y <= y &&
           actualPlace->y + actualPlace->h >= y;
}

SDL_Rect getActualCoordsOnGrid(bool isLeftSide, int x, int y) {
    int square_w = getOneSquareW();
    SDL_Rect base = isLeftSide ? TABLE_RECT : RIGHT_SIDE_RECT;
    //todo document it
    x = isLeftSide ? x : 0;

    return (SDL_Rect) {base.x + (x * square_w), base.y + (square_w * y), square_w, square_w};
}

//todo fix... square w
Cell * getClickedOnBlock(Cell **grid, int x, int y) {

    for (int i = 0; i < GRID_W; i++) {
        for (int j = 0; j <= GRID_W; j++) {
            if (grid[i][j].block_id != -1 && grid[i][j].isMoveable) {
                SDL_Rect block_place = getActualCoordsOnGrid(grid[i][j].isLeftSide, j, i);
                if (isClickedOnBtn(&block_place, x, y)) {
                    return &grid[i][j];
                }
            }
        }
    }
    return NULL;
}

bool PlaceBlockIfCan(Cell **grid, Cell * block, int x, int y, SDL_Renderer * renderer) {
    int square_w = getOneSquareW();
    for (int i = 0; i < GRID_W; i++) {
        for (int j = 0;j < GRID_W; j++) {
            SDL_Rect current = getActualCoordsOnGrid(true, j,i);

             if (isClickedOnBtn(&current, x,y) && grid[i][j].block_id == -1) {
                initCell(true, block->block_id, block->rotation, &grid[i][j]);
                return true;
             }
        }
    }
    return false;
}

//todo mások file
Page runGameEvents(SDL_Renderer *renderer, cJSON *blocks, Cell ***grid, ButtonRect **buttons) {
    SDL_Event event;
    bool isDragBlock = false;
    Cell * clickedOn;

    int squaer_w = getOneSquareW();
    while (SDL_WaitEvent(&event) && event.type != SDL_QUIT) {

        switch (event.type) {
            /* eger kattintas */
            case SDL_MOUSEBUTTONDOWN:
                if (event.button.button == SDL_BUTTON_LEFT) {

                    int clickedOnBtnId = getClickedButtonIdIfExists(buttons, inGame, event.button.x, event.button.y);
                    if (clickedOnBtnId != -1) {
                        Page nextPage = handleBtnClickAndGetNextPageIfShould(clickedOnBtnId, inGame);
                        if (nextPage != -1) return nextPage;
                    }

                    if (!isDragBlock) {
                        clickedOn = getClickedOnBlock(*grid, event.button.x, event.button.y);
                        clickedOn->display = false;
                        if (clickedOn != NULL) isDragBlock = true;
                    }
                }
                break;
            case SDL_MOUSEBUTTONUP:

                if (event.button.button == SDL_BUTTON_LEFT) {
                    if (isDragBlock) {
                        if (!PlaceBlockIfCan(*grid, clickedOn, event.button.x, event.button.y, renderer)) {
                            clickedOn->block_id = -1;
                        }
                        drawFullMap(renderer, *grid, buttons);
                        SDL_RenderPresent(renderer);
                        isDragBlock = false;
                        }
                    }
                break;
            case SDL_MOUSEMOTION:
                if (isDragBlock && clickedOn != NULL) {
                    drawFullMap(renderer, *grid, buttons);
                    drawMoveing(renderer, event.motion.x, event.motion.y, clickedOn->block_id);
                    SDL_RenderPresent(renderer);
                }
        }

    }
    return -1;
}
//todo fix square_W
void drawBlock(SDL_Renderer *renderer, Cell cell, int y, int x) {
    SDL_Rect block_place = getActualCoordsOnGrid(cell.isLeftSide,x,y);
    SDL_Texture *imgTexture = getBlockTexture(renderer, cell.block_id);
    SDL_RenderCopyEx(renderer, imgTexture, NULL, &block_place, cell.rotation * 90, NULL, SDL_FLIP_NONE);
    SDL_DestroyTexture(imgTexture);
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
    cJSON_Delete(allMap);
    return selectedMaps;
}

/** Kirajzolja a játéktábla alapjait (négyzetrácsok, jobb oldali sáv)
 * @param SDL_Renderer *renderer
 * @pararm int level
 * */
void drawGameTable(SDL_Renderer *renderer) {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDrawRect(renderer, &TABLE_RECT);
    SDL_RenderDrawRect(renderer, &RIGHT_SIDE_RECT);
    drawGrid(renderer);
}

/** Kirajzolja a négyzetrácsokat.
 * @pararm SDL_Renderer * renderer
 * */
void drawGrid(SDL_Renderer *renderer) {
    int squere_w = getOneSquareW();
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    for (int i = 1; i < GRID_W; i++) {
        SDL_RenderDrawLine(renderer, TABLE_RECT.x + (i * squere_w), TABLE_RECT.y, TABLE_RECT.x + (i * squere_w),
                           TABLE_RECT.y + TABLE_RECT.h);
        SDL_RenderDrawLine(renderer, TABLE_RECT.x, TABLE_RECT.y + (i * squere_w), TABLE_RECT.x + TABLE_RECT.w,
                           TABLE_RECT.y + (i * squere_w));

        SDL_RenderDrawLine(renderer, RIGHT_SIDE_RECT.x, RIGHT_SIDE_RECT.y + (i * squere_w),
                           RIGHT_SIDE_RECT.x + RIGHT_SIDE_RECT.w, RIGHT_SIDE_RECT.y + (i * squere_w));
    }
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

void drawFullMap(SDL_Renderer *renderer, Cell **grid, ButtonRect **buttons) {
    SDL_SetRenderDrawColor(renderer,0,0,0,255);
    SDL_RenderClear(renderer);
    drawAllCurrentButtons(renderer, buttons, inGame);
    drawGameTable(renderer);
    drawCurrentGrid(renderer, grid, buttons);
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
        cJSON *firstMapBlockArr = cJSON_GetObjectItem(firstMap, MAP_BLOCKS_ARRAY);
        cJSON *rightSideBlockArr = cJSON_GetObjectItem(firstMap, PLACEABLE_BLOCK_ARRAY);
        Cell **grid = initGridStructure(firstMapBlockArr, rightSideBlockArr);

        ButtonRect **buttons = createButtonsForCurrentPage(inGame);

        drawFullMap(renderer, grid, buttons);
        SDL_RenderPresent(renderer);


        Page next = runGameEvents(renderer, firstMapBlockArr, &grid, buttons);
        if (next != -1) {
            resetScreenAndFreeButtonsArray(renderer, buttons, inGame);
            return next;
        }

        cJSON_Delete(rightSideBlockArr);
        cJSON_Delete(firstMap);
        cJSON_Delete(firstMapBlockArr);

    } else {
        /*
        Ebben az esetben fogja a rendszer betolteni az előző mentést.
        */
    }
    cJSON_Delete(currentData);

    return gameMenu;
}

