#ifndef GAMEBOARDEVENTS_H_INCLUDED
#define GAMEBOARDEVENTS_H_INCLUDED
#include "GameStarter.h"

void *createLaserPathTree(Cell **grid, LaserPath **root);
LaserPath *createRoot(Cell **grid);
SDL_Texture *getBlockTexture(SDL_Renderer *renderer, int blockId);
Cell *getClickedOnBlock(Cell **grid, int x, int y);
LaserPath *initLaserPath(int fromRow, int fromCol, Cell **grid, Direction laserDir);
LaserPath *runLaser(SDL_Renderer *renderer, Cell **grid);
bool canBeNext(Cell *cell, Direction from);
void drawBlock(SDL_Renderer *renderer, Cell *cell, SDL_Rect *block_place);
void drawCurrentGrid(SDL_Renderer *renderer, Cell **grid, ButtonRect **buttons);
void drawFullMap(SDL_Renderer *renderer, Cell **grid, ButtonRect **buttons, StringToDisplay *statusStrObj);
void drawGameTable(SDL_Renderer *renderer);
void drawGrid(SDL_Renderer *renderer);
void drawLaser(SDL_Renderer *renderer, LaserPath *r);
void drawMoveing(SDL_Renderer *renderer, Cell *cell, int x, int y);
void freeTree(LaserPath *r);
SDL_Rect getActualCoordsOnGrid(bool isLeftSide, int x, int y);
Direction getOppositeDir(Direction dir);
int getTheNextTouchedBlockRowOrCol(LaserPath *line, Cell **grid);
GameEvent handlButtonsClicks(int btnId);
bool PlaceBlockIfCan(SDL_Renderer *renderer, Cell **grid, Cell *block, int cursorX, int cursorY);
void printTree(LaserPath *root);
GameEvent runGameEvents(SDL_Renderer *renderer, Cell **grid, ButtonRect **buttons, StringToDisplay *statusStrObj);


#endif // GAMEBOARDEVENTS_H_INCLUDED
