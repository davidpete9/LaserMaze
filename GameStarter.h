#ifndef GAMESTARTER_H_INCLUDED
#define GAMESTARTER_H_INCLUDED
#include "cJSON.h"

void initializeFileWithLevel(int level);
void initializeGame(cJSON * data);
void setGameInitalizationStatus(cJSON *structure, bool val);
void setLevel(cJSON *structure, int level);
void startGame(SDL_Renderer *renderer);

#endif // GAMESTARTER_H_INCLUDED
