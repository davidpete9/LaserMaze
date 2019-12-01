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


Cell **initGridStructure(cJSON *mapdata);
char *generateFormattedStringFromNumber(int num, const char *format);
int *getArrayFromObject(cJSON *obj, char *key, int *size);
cJSON *getMapsForLevel(int level);
cJSON *saveResultToFile(int level, int elapsedTime, bool usedSkip);
int *selectRandomIndexes(int allMapsNum, int *array_length);
bool checkSolution(Cell **grid);
void freeGrid(Cell **grid);
void freeTree(LaserPath *r);
int getInitializedGameLevelIfExists();
void incrementTime(cJSON *currentData, double elapsed);
void initCell(bool isLeft, int block_id, int rotation, Cell *cell);
static void initEmptyResultObj(cJSON *result, int level, int elapsedTime);
void initializeFileWithLevel(int level);
bool isArrayIncludes(int *arr, int length, int n);
void printGridForDebug(Cell **grid);
void resetGridAfterShot(Cell **grid);
bool runGame(SDL_Renderer *renderer, cJSON *currentData, cJSON *maps);
void selectAndLoadNextMap(cJSON *maps, int mapInd, Cell ***grid, cJSON **actualMap);
void setBadges(cJSON *result, bool usedSkip, int elapsedTime);
void setSelectedNumbers(cJSON *data, int *selectedMapIndexes, int size);
Page startGame(SDL_Renderer *renderer);
StringToDisplay * getGameCounterObj(SDL_Renderer *renderer, int level, int mapInd, int size, int mapId);
void writeMessageToTop(SDL_Renderer *renderer, char *text, bool isSuccess);

#endif // GAMESTARTER_H_INCLUDED
