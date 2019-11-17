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
#include "cJSON.h"
#include "debugmalloc.h"

FILE *createNewFile(const char *filename) {

    FILE *fp;

    fp = fopen(filename, "wt");
    if (fp == NULL) {
        perror("Fájl megnyitása sikertelen");
        exit(1);
    }

    return fp;
}

//todo error handling
void printStructureIntoFileAndClose(FILE *fp, cJSON * structure) {
    fprintf(fp, cJSON_Print(structure));
    fclose(fp);
}


FILE * openFileForRead(const char *filename) {
    FILE * fp;
    fp = fopen(filename, "rt");
    if (fp == NULL) {
        perror("Fájl megnyitása sikertelen");
        exit(1);
    }
    return fp;
}

int getFileLength(FILE * fp) {
    fseek (fp, 0, SEEK_END);
    int len = ftell (fp);
    fseek (fp, 0, SEEK_SET);
    return len;
}

cJSON * readFileAndParseJson(FILE * fp) {
    int len = getFileLength(fp);
    if (len == 0) return NULL;
    char *dataString = (char *) malloc((len+1)*sizeof(char));
    dataString[0] = '\0';
    char * buffer = (char *) malloc((len+1)*sizeof(char));
    while (fscanf(fp, "%s", buffer) == 1) strcat(dataString, buffer);
    dataString[len] = '\0';
    cJSON* parsed = cJSON_Parse(dataString);
    free(dataString);
    free(buffer);
    return parsed;
}

void printItForDebugging(cJSON *data) {
 printf("\n Az actual.json parsolva: \n  %s \n",cJSON_Print(data));
}

/**
cJSON_DELETE(data) to free
*/
cJSON * getParsedJSONContentOfFile(char *filename) {
 FILE *fp = openFileForRead(filename);
 cJSON * data = readFileAndParseJson(fp);
 printItForDebugging(data);
 fclose(fp);
 return data;
}



