#ifndef ACTUALSTATEFILEHANDLER_H_INCLUDED
#define ACTUALSTATEFILEHANDLER_H_INCLUDED
#include "cJSON.h"
#include <stdio.h>

FILE *openOrCreateFile(const char *filename);
int getFileLength(FILE * fp);
cJSON * getParsedJSONContentOfFile(const char *filename);
FILE * openFileForRead(const char *filename);
void printItForDebugging(cJSON *data);
void printStructureIntoFileAndClose(const char *filename, cJSON * structure);
char * readFileContent(FILE * fp);

#endif // ACTUALSTATEFILEHANDLER_H_INCLUDED
