#ifndef ACTUALSTATEFILEHANDLER_H_INCLUDED
#define ACTUALSTATEFILEHANDLER_H_INCLUDED
#include "cJSON.h"
#include <stdio.h>

/** Létrehoz egy fájlt a paraméterként kapot néven.
* A hívó felelőssége bezáeni a fájlt az fclose() segítségével.
* @param const char *filename
* @return FILE *fp
*/
FILE *createNewFile(const char *filename);
/** Megnézi, hogy hány karakterből áll a kapott fájl tartalma.
* @param FILE *fp
* @return int len
* */
int getFileLength(FILE * fp);
/** Beolvassa a kapott JSON fájl teljes tartalmát, és cJSON struktúrává alakítja.
* Amennyiben az átalakítás sikertelen NULL pointerrel tér vissza. A hívó felelősség felszabadítani
* a cJSON struktúra által foglalt memóriát a cJSON_Delete() függvény segítségével.
* @param const char * filename
* @return cJSON * parsed
*/
cJSON * getParsedJSONContentOfFile(const char *filename);
/** Megnyit egy fájlt a paraméteren kapott néven. Amennyiben sikertelen a megnyitás NULL pointer a visszatérési érték.
* A hívó felelőssége bezárni a fájlt az fclose segítségével.
* @param const char * filename
* @return FILE *fp
* */
FILE * openFileForRead(const char *filename);
void printItForDebugging(cJSON *data);
void printStructureIntoFileAndClose(FILE *fp, cJSON * structure);
/** Beolvassa a kapott fájl teljes tartalmát egy stringbe, Amennyiben az átalakítás sikertelen NULL pointerrel tér vissza.
* A hívó felelőssége felszabadítani a string számára lefoglalt memóriát.
* @param FILE *fp
* @return cJSON * parsed
* */
char * readFileContent(FILE * fp);

#endif // ACTUALSTATEFILEHANDLER_H_INCLUDED
