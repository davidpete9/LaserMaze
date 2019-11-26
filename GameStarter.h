#ifndef GAMESTARTER_H_INCLUDED
#define GAMESTARTER_H_INCLUDED
#include "cJSON.h"

typedef struct Cell {
    int block_id;
    bool isPlacedByUser;
    bool isRotatable;
    bool isMoveable;
    int rotation;
    bool isLeftSide;
    bool display;
} Cell;

Cell **initGridStructure(cJSON *blockArr, cJSON *rightBlocks);
char *generateFormattedStringFromNumber(int num, const char *format);
cJSON *selectMapsForLevel(int level);
cJSON *selectRandomMaps(cJSON *allMap);
void initCell(bool isLeft, int block_id, int rotation, Cell *cell);
void initializeFileWithLevel(int level);
void printGridForDebug(Cell **grid);
void setGameInitalizationStatus(cJSON *structure, bool val);
void setLevel(cJSON *structure, int level);
Page startGame(SDL_Renderer *renderer);

#endif // GAMESTARTER_H_INCLUDED
