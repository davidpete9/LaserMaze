#ifndef GAMESTARTER_H_INCLUDED
#define GAMESTARTER_H_INCLUDED
#include "cJSON.h"

void drawBlock(SDL_Renderer * renderer, cJSON * block);
void drawGameTable(SDL_Renderer *renderer, int level);
void drawGrid(SDL_Renderer * renderer);
char * generateFormattedStringFromNumber(int num, const char * format);
SDL_Texture * getBlockTexture(SDL_Renderer *renderer, int blockId);
void initializeFileWithLevel(int level);
void placeBlocksToGrid(SDL_Renderer * renderer, cJSON * blocksArr);
cJSON * selectMapsForLevel(SDL_Renderer *renderer, int level);
cJSON * selectRandomMaps(cJSON * allMap, int * selectedNum);
void setGameInitalizationStatus(cJSON *structure, bool val);
void setLevel(cJSON *structure, int level);
bool startGame(SDL_Renderer *renderer);

#endif // GAMESTARTER_H_INCLUDED
