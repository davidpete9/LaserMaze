#ifndef GAMEBOARDEVENTS_H_INCLUDED
#define GAMEBOARDEVENTS_H_INCLUDED
#include "GameStarter.h"

SDL_Texture *getBlockTexture(SDL_Renderer *renderer, int blockId);
void drawBlock(SDL_Renderer *renderer, Cell * cell, SDL_Rect * block_place);
void drawCurrentGrid(SDL_Renderer *renderer, Cell **grid, ButtonRect **buttons);
void drawFullMap(SDL_Renderer *renderer, Cell **grid, ButtonRect **buttons);
void drawGameTable(SDL_Renderer *renderer);
void drawGrid(SDL_Renderer *renderer);
void drawMoveing(SDL_Renderer *renderer, Cell * cell, int x, int y);
SDL_Rect getActualCoordsOnGrid(bool isLeftSide, int x, int y);
Cell * getClickedOnBlock(Cell **grid, int x, int y);
bool PlaceBlockIfCan(Cell **grid, Cell * block, int x, int y, SDL_Renderer * renderer);
Page runGameEvents(SDL_Renderer *renderer, cJSON *blocks, Cell ***grid, ButtonRect **buttons);

#endif // GAMEBOARDEVENTS_H_INCLUDED
