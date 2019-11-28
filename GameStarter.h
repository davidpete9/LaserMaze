#ifndef GAMESTARTER_H_INCLUDED
#define GAMESTARTER_H_INCLUDED
#include <stdbool.h>
#include <SDL2/SDL.h>
#include "cJSON.h"
#include "MenuEngine.h"

typedef enum GameEvent {
    leave,
    reset,
    pause,
    skip,
    fire
} GameEvent;

typedef enum Direction {
    north,
    east,
    south,
    west ,
    nowhere
} Direction;

typedef struct Cell {
    int block_id;
    bool isPlacedByUser;
    bool isRotatable;
    bool isMoveable;
    int rotation;
    bool isLeftSide;
    bool display;
    bool hasLaserTouchedIt;
} Cell;

typedef struct LaserPath {
    int startRow;
    int startCol;
    Direction dir;
    struct LaserPath * next;
    struct LaserPath * next2;
} LaserPath;

Cell **initGridStructure(const cJSON *mapdata);
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
