#ifndef ACTUALSTATEFILEHANDLER_H_INCLUDED
#define ACTUALSTATEFILEHANDLER_H_INCLUDED
#include "cJSON.h"
#include <stdio.h>

FILE *createNewFile(const char *filename);
int getFileLength(FILE * fp);
cJSON * getParsedJSONContentOfFile(char *filename);
FILE * openFileForRead(const char *filename);
void printItForDebugging(cJSON *data);
void printStructureIntoFileAndClose(FILE *fp, cJSON * structure);
char * readFileContent(FILE * fp);

#endif // ACTUALSTATEFILEHANDLER_H_INCLUDED
