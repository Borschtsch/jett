#ifndef JETT_H
#define JETT_H

#include <stddef.h>
#include <stdbool.h>

#ifndef MULTITHREAD_ENABLED
/*
 * Init is only needed to set initial values
 */
void jett_init(char *pJsonDocument, int size);
#else
typedef struct
{
   int pos;
   int length;
   char* pJson;
} sJettState;
#endif

/*
 * Find key withing the object and if successful return its start and end positions
 */
bool jett_findKey(
#ifdef MULTITHREAD_ENABLED
   sJettState* pState,
#endif
   int* pBegin, int* pEnd);

/*
 * Enters collection of items
 */
bool jett_collectionBegin(
#ifdef MULTITHREAD_ENABLED
   sJettState* pState
#else
   void
#endif
);

/*
 * Exits collection of items
 */
bool jett_collectionEnd(
#ifdef MULTITHREAD_ENABLED
   sJettState* pState
#else
   void
#endif
);

/*
 * Get primitive type value
 */
bool jett_getValue(
#ifdef MULTITHREAD_ENABLED
   sJettState* pState,
#endif
   int* pBegin, int* pEnd);

#endif//JETT_H
