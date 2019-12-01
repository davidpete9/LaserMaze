#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <time.h>
#include "MenuEngine.h"
#include "Constanses.h"
#include "GameStarter.h"
#include "cJSON.h"
#include "MirrorBounce.h"
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

/** Mikor a játékos egy blokkot mozgat, az adott blokknak az egérkurzorhoz történő kirajzolását végzi a
 * függvény. A A blokk középpontját illeszti mindig a kurzorhoz a program.
 * @param SDL_Renderer * renderer,
 * @param Cell * cell,
 * @param int x
 * @param int y
 * */
void drawMoveing(SDL_Renderer *renderer, Cell *cell, int x, int y) {
    SDL_Rect to = (SDL_Rect) {x - floor(SQUARE_W / 2.0), y - floor(SQUARE_W / 2.0), SQUARE_W, SQUARE_W};
    drawBlock(renderer, cell, &to);
}

/** Visszatér a kapott koordinátájú négyzet tényleges elhelyezkedésével a képernyőn.
 * Meg kell adni, hogy a bal oldali négyzetrács, vagy a jobb oldali szerint keresse ki a koordinátákat.
 * @param bool isLeftSide
 * @param int x
 * @param int y
 * @return SDL_Rect
 * */
SDL_Rect getActualCoordsOnGrid(bool isLeftSide, int x, int y) {
    SDL_Rect base = isLeftSide ? TABLE_RECT : RIGHT_SIDE_RECT;
    //Jobb oszlop esetén lenullázzom az x koordináta értékét, mivel ott csak egy oszlop van.
    x = isLeftSide ? x : 0;
    return (SDL_Rect) {base.x + (x * SQUARE_W), base.y + (SQUARE_W * y), SQUARE_W, SQUARE_W};
}

/** Lerajzolja a képenyőre a kapott adatok akapján az aktuális pályát.
 * A grid változóban van az eltárolva, hogy melyik négyzetrácsban milyen blokk hogyan helyezkedik el.
 * @param SDL_Renderer *renderer
 * @param Cell **grid
 * @param ButtonRect *buttons
 * */
void drawCurrentGrid(SDL_Renderer *renderer, Cell **grid, ButtonRect **buttons) {
    for (int i = 0; i < GRID_W; i++) {
        for (int j = 0; j <= GRID_W; j++) {
            if (grid[i][j].block_id != -1 && grid[i][j].display) {
                SDL_Rect block_place = getActualCoordsOnGrid(grid[i][j].isLeftSide, j, i);
                drawBlock(renderer, &grid[i][j], &block_place);
            }
        }
    }
}

/** Lerajzolja az adott blokkot a megadott helyre. A cell változüó tárolja el, hogy milyen blokkot kell rajzolni, melyen formában.
 * A block_place megmondja a pontos koordinátákat, hogy hova.
 * @param SDL_Renderer *renderer
 * @param Cell *cell
 * @param SDL_Rect * block_place
 * */
void drawBlock(SDL_Renderer *renderer, Cell *cell, SDL_Rect *block_place) {
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
 * @param SDL_Renderer * renderer
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
//todo
/** Újrarajzolja a teljes térképet.
 * @param SDL_Renderer * renderer,
 * @parma Cell **grid
 * @param ButtonRect **buttons
 * @param StringToDisplay * statusStrObj
 * */
void drawFullMap(SDL_Renderer *renderer, Cell **grid, ButtonRect **buttons, StringToDisplay *statusStrObj) {

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderFillRect(renderer, &((SDL_Rect) {0, 0, S_WIDTH, S_HEIGHT}));
    drawAllCurrentButtons(renderer, buttons, IG_BTN_NUM);
    writeTextToDisplay(renderer, statusStrObj);
    drawGameTable(renderer);
    drawCurrentGrid(renderer, grid, buttons);
}

/** Megnézi, hogy a játékos melyik blokkon kattintott, és visszatér cím szerint vele.
 * Ha egyikkel sem, akkor NULL-lal tér vissza.
 * @param Cell **grid
 * @param int x
 * @param int y
 * @return Cell * block
 * * */
Cell *getClickedOnBlock(Cell **grid, int x, int y) {

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

/** Leteszi a kapott blokkot a négyzetrács azon elemére, amely fölött a kurzor van.
 * Visszetér egy logikai értékkel annak függvényében hogy ez sikerült e.
 * @oaram Cell **grid
 * @param Cell *block
 * @param int cursorX
 * @param int cursorY
 * @oaram SDL_Renderer * renderer
 * @return bool isSucceed
 *
 * */
bool PlaceBlockIfCan(SDL_Renderer *renderer, Cell **grid, Cell *block, int cursorX, int cursorY) {
    for (int i = 0; i < GRID_W; i++) {
        for (int j = 0; j < GRID_W; j++) {
            SDL_Rect current = getActualCoordsOnGrid(true, j, i);
            if (isClickedOnBtn(&current, cursorX, cursorY) && grid[i][j].block_id == -1) {
                initCell(true, block->block_id, block->rotation, &grid[i][j]);
                grid[i][j].isPlacedByUser = true;
                grid[i][j].isMoveable = true;
                return true;
            }
        }
    }
    return false;
}

/** Megmondja, hogy az adott gombra kattintva milyen játékesemény (GameEvent) következzen be.
 * @param int btnId
 * @return GameEvent next
 * */
GameEvent handlButtonsClicks(int btnId) {
    switch (btnId) {
        case IG_BACK_BTN:
            return beforeleave;
        case IG_FIRE_BTN:
            return fire;
        case IG_SKIP_BTN:
            return skip;
        case IG_PAUSE_BTN:
            return pause;
        case IG_RESET_BTN:
            return reset;
        default:
            return beforeleave;
    }
}

/** Létrehoz egy LaserPath struktúrát a kapott adatok alapján.
 * @param int fromRow
 * @param int fromCol
 * @param Cell **grid
 * @param Direction laserDir
 * @return LaserPath *item
 * */
LaserPath *initLaserPath(int fromRow, int fromCol, Cell **grid, Direction laserDir) {
    grid[fromRow][fromCol].hasLaserTouchedIt = true;
    grid[fromRow][fromCol].laserTouchedItDir = laserDir;
    LaserPath *line = (LaserPath *) malloc(sizeof(LaserPath));
    line->startRow = fromRow;
    line->startCol = fromCol;
    line->dir = nowhere;
    line->next2 = NULL;
    line->next = NULL;
    return line;
}

/**Megkeresi a lézersugár kiindulási pontját (A lézerkövő blokk pozíciója), és visszatér az azt tartalmazó
 * LaserPath * struktúrával, vagy NULL-lal, ha nincsen meg, Ez lesz a lézerfény útját leíró bináris fa gyökere.
 * @param Cell **grid
 * @return LaserPath * root
 * */
LaserPath *createRoot(Cell **grid) {
    for (int i = 0; i < GRID_W; i++) {
        for (int j = 0; j < GRID_W; j++) {
            if (grid[i][j].block_id == LASER_CANNON) {
                LaserPath *root = initLaserPath(i, j, grid, nowhere);
                root->dir = getLaserCannonDirection(grid[i][j].rotation);
                return root;
            }
        }
    }
    return NULL;
}

/**
 * Minden irányhoz visszatér a vele ellentétes irányúval.
 * @param Direction dir
 * @return Direction opposite
 * */
Direction getOppositeDir(Direction dir) {
    if (dir == north || dir == south) {
        return dir == north ? south : north;
    } else if (dir == east || dir == west) {
        return dir == west ? east : west;
    }
    return nowhere;
}

/**Eldönti egy blokknál, hogy lehet e a közekező, amit a lézerfény érint.
 * Akkor lehet, hogy érvényes blokk (nem üres),
 * és a lézerfény még nem érintette az adott irányból (a végtelen körbe-körbe pattogás elkerülés végett)
 * @param Cell *cell
 * @param Direction from
 * @return bool canBe
 * */
bool canBeNext(Cell *cell, Direction from) {
    return cell->block_id != -1 && from != getOppositeDir(cell->laserTouchedItDir);
}

//todo fura
int getTheNextTouchedBlockRowOrCol(LaserPath *line, Cell **grid) {
    int row = line->startRow;
    int col = line->startCol;
    switch (line->dir) {
        case west:
            while (col - 1 >= 0 && !canBeNext(&grid[row][col - 1], line->dir)) col--;
            return col - 1;
        case south:
            while (row + 1 < GRID_W && !canBeNext(&grid[row + 1][col], line->dir)) row++;
            return row + 1;
        case north:
            while (row - 1 >= 0 && !canBeNext(&grid[row - 1][col], line->dir)) row--;
            return row - 1;
        case east:
            while (col + 1 < GRID_W && !canBeNext(&grid[row][col + 1], line->dir)) col++;
            return col + 1;
        default:
            return 0;
    }
}


//todo fura
/**
Építési folyamat:
Megvan: az aktuális root koordinátái
az aktuális rootból kiinduló vonal iránya

Cél: toKoordináták beállítása + next nek a from, és a dir
*/
void *createLaserPathTree(Cell **grid, LaserPath **root) {

    LaserPath *r = *root;
    if (r->dir == nowhere) return;

    //temp
    int coord = getTheNextTouchedBlockRowOrCol(r, grid);
    bool hitEnd = false;
    if (coord == -1 || coord == GRID_W) hitEnd = true;
    if (coord == -1) coord = 0;
    if (coord == GRID_W) coord = GRID_W - 1;


    //todo: szepites, itt kicsit katyvaszos
    if (r->dir == west || r->dir == east) {
        r->next = initLaserPath(r->startRow, coord, grid, getOppositeDir(r->dir));
    } else {
        r->next = initLaserPath(coord, r->startCol, grid, getOppositeDir(r->dir));
    }

    if (hitEnd) {
        r->next->dir = nowhere;
        return;
    }

    //ellentete a kiindulona. pl. nyugat fel megyunk, de a blokk keletioldalara erkezik
    Direction arrivelDirection = getOppositeDir(r->dir);

    r->next->dir = findDirection(arrivelDirection, grid[r->next->startRow][r->next->startCol].block_id,
                                 grid[r->next->startRow][r->next->startCol].rotation);

    createLaserPathTree(grid, &(r->next));


    if (grid[r->next->startRow][r->next->startCol].block_id == DOUBLE_REFLECTION_MIRROR) {
        r->next2 = initLaserPath(r->next->startRow, r->next->startCol, grid, getOppositeDir(r->dir));
        //mivel innent egyenesen novabb megy, nem valtozik az irany ebben a szogben.
        r->next2->dir = r->dir;
        createLaserPathTree(grid, &(r->next2));
    }

}


void printTree(LaserPath *root) {
    if (root == NULL) {
        return;
    }

    printf("\n{from: [%d, %d] dir: %s}\n", root->startRow, root->startCol,
           root->dir == west ? "west" : root->dir == east ? "east" : root->dir == north ? "north" : root->dir == nowhere
                                                                                                    ? "sehova"
                                                                                                    : "south");
    printTree(root->next);
    printTree(root->next2);
}

//todo atir..
void drawLaser(SDL_Renderer *renderer, LaserPath *r) {
    if (r == NULL || r->dir == nowhere) return;
    int fromX = r->startCol;
    int fromY = r->startRow;

    if (r->next != NULL) {
        int toX = r->next->startCol;
        int toY = r->next->startRow;


        if (fromX == toX && fromY == toY) return;
        lineRGBA(renderer,
                 TABLE_RECT.x + (fromX * SQUARE_W) + SQUARE_W / 2,
                 TABLE_RECT.y + (fromY * SQUARE_W) + SQUARE_W / 2,
                 TABLE_RECT.x + (toX * SQUARE_W) + SQUARE_W / 2,
                 TABLE_RECT.y + (toY * SQUARE_W) + SQUARE_W / 2, 255, 0, 0, 255);
    }
    if (r->next2 != NULL) {
        int toX = r->next2->startCol;
        int toY = r->next2->startRow;


        if (fromX == toX && fromY == toY) return;
        lineRGBA(renderer,
                 TABLE_RECT.x + (fromX * SQUARE_W) + SQUARE_W / 2,
                 TABLE_RECT.y + (fromY * SQUARE_W) + SQUARE_W / 2,
                 TABLE_RECT.x + (toX * SQUARE_W) + SQUARE_W / 2,
                 TABLE_RECT.y + (toY * SQUARE_W) + SQUARE_W / 2, 255, 0, 0, 255);
    }

    drawLaser(renderer, r->next);
    drawLaser(renderer, r->next2);

}


/** A lézerfény ellövését kezeli, kiszémolja a sugér útját.
 * És a kiszámolt bináris fa gyökerével tér vissza (amely leírja, hogy honnan hogy pattan a lézer);
 * Kirajzol a képernyőre a sugár útját.
 * @param SDL_Renderer *renderer
 * @param Cell **grid
 * @return LaserPath *root
 * */
LaserPath *runLaser(SDL_Renderer *renderer, Cell **grid) {

    LaserPath *root = createRoot(grid);
    if (!root) return NULL;
    createLaserPathTree(grid, &root);
    printTree(root);
    drawLaser(renderer, root);
    SDL_RenderPresent(renderer);
    return root;
}

/** Visszatér a soron következő játékeseménnyel (GameEvent). Lekezeli a játékos kattintásait, amint valamelyik gombra kattintott
 * visszatér a megfelelő eseménnyel. Továbbá, lekezeli az, hogy a mozgatható blokkokat tudja az egér bel gombjának
 * a nyomvatartásával mozgatni, és lehelyezni a megfelelő helyre. (Ehhez mindig újrarajzolja a képernyőt mozgatás közben)
 *  Lehetővé teszi, hogy a játékos forgassa jobb gombbal az adott blokkot.
 * @param SDL_Renderer *renderer
 * @param Cell **grid
 * @param ButtonRect **button
 * @param StringToDisplay statusStrObj
 * @return GameEvent next
 * */
GameEvent runGameEvents(SDL_Renderer *renderer, Cell **grid, ButtonRect **buttons, StringToDisplay *statusStrObj) {
    drawFullMap(renderer, grid, buttons, statusStrObj);
    SDL_RenderPresent(renderer);
    SDL_Event event;
    bool isDragBlock = false;
    Cell *clickedOn;
    while (SDL_WaitEvent(&event) && event.type != SDL_QUIT) {

        switch (event.type) {
            case SDL_MOUSEBUTTONDOWN:
                if (event.button.button == SDL_BUTTON_LEFT) {

                    int clickedOnBtnId = getClickedButtonIdIfExists(buttons, inGame, event.button.x, event.button.y);
                    if (clickedOnBtnId != -1) {
                        return handlButtonsClicks(clickedOnBtnId);
                    }

                    if (!isDragBlock) {
                        clickedOn = getClickedOnBlock(grid, event.button.x, event.button.y);
                        if (clickedOn != NULL) {
                            clickedOn->display = false;
                            isDragBlock = true;
                        }
                    }
                } else if (event.button.button == SDL_BUTTON_RIGHT) {
                    //a blokk forgatása.
                    Cell *actualBlock = getClickedOnBlock(grid, event.button.x, event.button.y);
                    if (actualBlock != NULL) {
                        actualBlock->rotation = (actualBlock->rotation + 1) % 4;
                        drawFullMap(renderer, grid, buttons, statusStrObj);
                        SDL_RenderPresent(renderer);
                    }
                }
                break;
            case SDL_MOUSEBUTTONUP:

                if (event.button.button == SDL_BUTTON_LEFT) {
                    if (isDragBlock) {
                        //A bal egérgomb elengedésekor megpróbája lehelyezni a blokkot.
                        // Amenyiben nem sikerül, visszahelyezi az előző pozícióba.

                        if (!PlaceBlockIfCan(renderer, grid, clickedOn, event.button.x, event.button.y)) {
                            clickedOn->display = true;
                        } else {
                            clickedOn->block_id = -1;
                        }
                        isDragBlock = false;
                        drawFullMap(renderer, grid, buttons, statusStrObj);
                        SDL_RenderPresent(renderer);
                    }
                }
                break;
            case SDL_MOUSEMOTION:
                if (isDragBlock && clickedOn != NULL) {
                    //A blokk mozgatása: először a háttér kirajzolása, majd a blokké, minden kurzormozgásra.
                    drawFullMap(renderer, grid, buttons, statusStrObj);
                    drawMoveing(renderer, clickedOn, event.motion.x, event.motion.y);
                    SDL_RenderPresent(renderer);
                }
        }
    }
    return leave;
}
