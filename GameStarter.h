#ifndef GAMESTARTER_H_INCLUDED
#define GAMESTARTER_H_INCLUDED
#include <stdbool.h>
#include <SDL2/SDL.h>
#include "cJSON.h"
#include "MenuEngine.h"

typedef enum GameEvent {
    leave,
    stop,
    reset,
    pause,
    skip,
    fire,
    finished,
    restart,
    beforeleave
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
    Direction laserTouchedItDir;
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
cJSON *getMapsForLevel(int level);
cJSON *selectRandomMaps(cJSON *allMap);
bool checkSolution(LaserPath * root, Cell ** grid);
void freeGrid(Cell **grid);
void freeTree(LaserPath *r);
void initCell(bool isLeft, int block_id, int rotation, Cell *cell);
void initializeFileWithLevel(int level);
bool isArrayIncludes(int * arr, int length, int n);
void printGridForDebug(Cell **grid);
void resetGridAfterShot(Cell **grid);
void selectAndLoadNextMap(cJSON * maps, int mapInd, Cell ***grid, cJSON **actualMap);
int * selectRandomIndexes(int length, int * array_length);
void setGameInitalizationStatus(cJSON *structure, bool val);
void setLevel(cJSON *structure, int level);
Page startGame(SDL_Renderer *renderer);
int getInitializedGameLevelIfExists();
void writeGameCounterToDisplay(SDL_Renderer *renderer, int level, int mapInd, int size, int mapId);
void writeMessageToTop(SDL_Renderer *renderer, char * text, bool isSuccess);

#endif // GAMESTARTER_H_INCLUDED
