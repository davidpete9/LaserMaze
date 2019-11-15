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
#include "cJSON.c"
#include "debugmalloc.h"


const char * IS_GAME_INITIALIZED = "isGameInitialized";
const char * LEVEL_STR = "currentLevel";

FILE *createNewFile() {

    FILE *fp;

    fp = fopen("actual.json", "wt");
    if (fp == NULL) {
        perror("Fájl megnyitása sikertelen");
        exit(1);
    }

    return fp;
}

cJSON * initalizeEmptyJSON() {
    cJSON * structure;
    structure = (cJSON *) malloc(sizeof(cJSON));
    structure = cJSON_CreateObject();
    return structure;
}

//todo error handling
void printStructureIntoFileAndClose(FILE *fp, cJSON * structure) {
    fprintf(fp, cJSON_Print(structure));
    fclose(fp);
}

void setGameInitalizationStatus(cJSON *structure, bool val) {
    cJSON_AddItemToObject(structure, IS_GAME_INITIALIZED, cJSON_CreateBool(val));
}

void setLevel(cJSON *structure, int level) {
    cJSON_AddItemToObject(structure, LEVEL_STR, cJSON_CreateNumber((int)level));
}

void initializeFileWithLevel(int level) {
    FILE *fp = createNewFile();
    cJSON * structure = initalizeEmptyJSON();

    setGameInitalizationStatus(structure, false);
    setLevel(structure, level);

    printStructureIntoFileAndClose(fp, structure);
}


FILE * openFileForRead() {
    FILE * fp;
    fp = fopen("actual.json", "rt");
    if (fp == NULL) {
        perror("Fájl megnyitása sikertelen");
        exit(1);
    }
    return fp;
}

int getFileLength(FILE * fp) {
fseek (fp, 0, SEEK_END);
long length = ftell (fp);
printf("-------------\n %d  \n------------", length);
fseek (fp, 0, SEEK_SET);


}

char * readFileIntoString(FILE * fp) {
    getFileLength(fp);
}

bool isGameInitialized() {

 FILE *fp = openFileForRead();
 readFileIntoString(fp);
 fclose(fp);
}






