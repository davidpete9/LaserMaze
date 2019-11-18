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

/** Létrehoz egy fájlt a paraméterként kapot néven.
 * A hívó felelőssége bezáeni a fájlt az fclose() segítségével.
 * @param const char *filename
 * @return FILE *fp
 */
FILE *createNewFile(const char *filename) {

    FILE *fp;
    fp = fopen(filename, "wt");
    if (fp == NULL) {
        perror("Fájl megnyitása sikertelen");
        exit(1);
    }

    return fp;
}

/**
 * @param cJSON * structure
 * @param FILE * fp
 * */
void printStructureIntoFileAndClose(FILE *fp, cJSON * structure) {
    fprintf(fp, cJSON_Print(structure));
    fclose(fp);
}

/** Megnyit egy fájlt a paraméteren kapott néven. Amennyiben sikertelen a megnyitás NULL pointer a visszatérési érték.
 * A hívó felelőssége bezárni a fájlt az fclose segítségével.
 * @param const char * filename
 * @return FILE *fp
 * */
FILE * openFileForRead(const char *filename) {
    FILE * fp;
    fp = fopen(filename, "rt");
    return fp;
}

/** Megnézi, hogy hány karakterből áll a kapott fájl tartalma.
 * @param FILE *fp
 * @return int len
 * */
int getFileLength(FILE * fp) {
    fseek (fp, 0, SEEK_END);
    int len = ftell (fp);
    fseek (fp, 0, SEEK_SET);
    return len;
}

/** Beolvassa a kapott fájl teljes tartalmát egy stringbe, Amennyiben az átalakítás sikertelen NULL pointerrel tér vissza.
 * A hívó felelőssége felszabadítani a string számára lefoglalt memóriát.
 * @param FILE *fp
 * @return cJSON * parsed
 * */
char * readFileContent(FILE * fp) {
    int len = getFileLength(fp);
    if (len == 0) return NULL;

    char *dataString = (char *) malloc((len+1)*sizeof(char));
    dataString[0] = '\0';

    char * buffer = (char *) malloc((len+1)*sizeof(char));

    while (fscanf(fp, "%s", buffer) == 1) strcat(dataString, buffer);
    dataString[len] = '\0';
    free(buffer);
    return dataString;
}

void printItForDebugging(cJSON *data) {
 printf("\n %s \n",cJSON_Print(data));
}

/** Beolvassa a kapott JSON fájl teljes tartalmát, és cJSON struktúrává alakítja.
 * Amennyiben az átalakítás sikertelen NULL pointerrel tér vissza. A hívó felelősség felszabadítani
 * a cJSON struktúra által foglalt memóriát a cJSON_Delete() függvény segítségével.
 * @param const char * filename
 * @return cJSON * parsed
*/
cJSON * getParsedJSONContentOfFile(const char *filename) {
 FILE *fp = openFileForRead(filename);
 char * dataString = readFileContent(fp);
 cJSON* parsed = cJSON_Parse(dataString);
 free(dataString);
 fclose(fp);
 return parsed;
}



