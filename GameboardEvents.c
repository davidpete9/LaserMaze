#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include "MenuEngine.h"
#include "Constanses.h"
#include "GameStarter.h"
#include "cJSON.h"
#include "debugmalloc.h"


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

void drawMoveing(SDL_Renderer *renderer, Cell * cell, int x, int y) {
    SDL_Rect to = (SDL_Rect) {x-floor(SQUARE_W/2.0), y-floor(SQUARE_W/2.0), SQUARE_W, SQUARE_W};
    drawBlock(renderer, cell, &to);
}

SDL_Rect getActualCoordsOnGrid(bool isLeftSide, int x, int y) {
    SDL_Rect base = isLeftSide ? TABLE_RECT : RIGHT_SIDE_RECT;
    //todo document it
    x = isLeftSide ? x : 0;
    return (SDL_Rect) {base.x + (x * SQUARE_W), base.y + (SQUARE_W * y), SQUARE_W, SQUARE_W};
}

void drawCurrentGrid(SDL_Renderer *renderer, Cell **grid, ButtonRect **buttons) {
    for (int i = 0; i < GRID_W; i++) {
        for (int j = 0; j <= GRID_W; j++) {
            if (grid[i][j].block_id != -1 && grid[i][j].display) {
                SDL_Rect block_place = getActualCoordsOnGrid(grid[i][j].isLeftSide,j,i);
                drawBlock(renderer, &grid[i][j], &block_place);
            }
        }
    }
}

void drawBlock(SDL_Renderer *renderer, Cell * cell, SDL_Rect * block_place) {
    SDL_Texture *imgTexture = getBlockTexture(renderer, cell->block_id);
    SDL_RenderCopyEx(renderer, imgTexture, NULL, block_place, cell->rotation * 90, NULL, SDL_FLIP_NONE);
    SDL_DestroyTexture(imgTexture);
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
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    for (int i = 1; i < GRID_W; i++) {
        SDL_RenderDrawLine(renderer, TABLE_RECT.x + (i * SQUARE_W), TABLE_RECT.y, TABLE_RECT.x + (i * SQUARE_W),
                           TABLE_RECT.y + TABLE_RECT.h);
        SDL_RenderDrawLine(renderer, TABLE_RECT.x, TABLE_RECT.y + (i * SQUARE_W), TABLE_RECT.x + TABLE_RECT.w,
                           TABLE_RECT.y + (i * SQUARE_W));

        SDL_RenderDrawLine(renderer, RIGHT_SIDE_RECT.x, RIGHT_SIDE_RECT.y + (i * SQUARE_W),
                           RIGHT_SIDE_RECT.x + RIGHT_SIDE_RECT.w, RIGHT_SIDE_RECT.y + (i * SQUARE_W));
    }
}


void drawFullMap(SDL_Renderer *renderer, Cell **grid, ButtonRect **buttons) {
    SDL_SetRenderDrawColor(renderer,0,0,0,255);
    SDL_RenderClear(renderer);
    drawAllCurrentButtons(renderer, buttons, inGame);
    drawGameTable(renderer);
    drawCurrentGrid(renderer, grid, buttons);
}

Cell * getClickedOnBlock(Cell **grid, int x, int y) {

    for (int i = 0; i < GRID_W; i++) {
        for (int j = 0; j <= GRID_W; j++) {
            if (grid[i][j].block_id != -1 && grid[i][j].isMoveable && grid[i][j].display) {
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
    for (int i = 0; i < GRID_W; i++) {
        for (int j = 0;j < GRID_W; j++) {
            SDL_Rect current = getActualCoordsOnGrid(true, j,i);
             if (isClickedOnBtn(&current, x,y) && grid[i][j].block_id == -1) {
                initCell(true, block->block_id, block->rotation, &grid[i][j]);
                grid[i][j].isPlacedByUser = true;
                grid[i][j].isMoveable = true;
                return true;
             }
        }
    }
    return false;
}

Page runGameEvents(SDL_Renderer *renderer, cJSON *blocks, Cell ***grid, ButtonRect **buttons) {
    SDL_Event event;
    bool isDragBlock = false;
    Cell * clickedOn;
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
                        if (clickedOn != NULL) {
                        clickedOn->display = false;
                        isDragBlock = true;
                        }
                    }
                }
                break;
            case SDL_MOUSEBUTTONUP:

                if (event.button.button == SDL_BUTTON_LEFT) {
                    if (isDragBlock) {
                        if (!PlaceBlockIfCan(*grid, clickedOn, event.button.x, event.button.y, renderer)) {
                            clickedOn->display = true;
                        }
                        else {
                            clickedOn->block_id = -1;
                        }
                        isDragBlock = false;
                        drawFullMap(renderer, *grid, buttons);
                        SDL_RenderPresent(renderer);
                        }
                    }
                break;
            case SDL_MOUSEMOTION:
                if (isDragBlock && clickedOn != NULL) {
                    drawFullMap(renderer, *grid, buttons);
                    drawMoveing(renderer, clickedOn, event.motion.x, event.motion.y);
                    SDL_RenderPresent(renderer);
                }
        }

    }
    return -1;
}