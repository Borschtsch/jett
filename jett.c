#include "jett.h"

#ifndef MULTITHREAD_ENABLED
static int pos = 0;
static int length = 0;
static char* pJson = NULL;

//Just initialize the pointer to a JSON text, its size and cursor position
void jett_init(char *pJsonDocument, int size)
{
   pos = 0;
   pJson = pJsonDocument;
   length = size;
}
#endif

bool jett_findKey(
#ifdef MULTITHREAD_ENABLED
   sJettState* pState,
#endif
   int* pBegin, int* pEnd)
{
#ifdef MULTITHREAD_ENABLED
   /* Initialize local variables we will be using to traverse the JSON */
   int pos = pState->pos;
   int length = pState->length;
   char* pJson = pState->pJson;
#endif

   *pBegin = -1;
   *pEnd = -1;
   int depth = 0;
   bool bResult = false;
   for (; (pJson[pos] != '\0') && (pos < length); pos++)
   {
      /* When looking-up for the key, skip anything related to the objects or arrays */
      if ((pJson[pos] == '{') || (pJson[pos]=='['))
      {
         /* Only increase the depth of search if key is not found */
         if ((*pBegin == -1) && (*pEnd == -1))
         {
            depth++;
         }
         /* Otherwise the JSON is somewhat broken and we cannot deal with it */
         else
         {
            *pBegin = -1;
            *pEnd = -1;
            break;
         }
      }
      else if ((pJson[pos] == '}') || (pJson[pos] == ']'))
      {
         /* Only go to the upper level of search if key is not found */
         if ((*pBegin == -1) && (*pEnd == -1))
         {
            depth--;
         }
         /* Otherwise the JSON is somewhat broken and we cannot deal with it */
         else
         {
            *pBegin = -1;
            *pEnd = -1;
            break;
         }
      }
      /* Only look-up for keys on the same level where we started */
      else if (depth == 0)
      {
         /* If candidate for a key is found, continue looking for its end */
         if ((pJson[pos] == '"') && (*pBegin == -1))
         {
            *pBegin = pos + 1;
            *pEnd = -1;
         }
         /* If candidate key end is found, mark it and continue to look for key-value separator */
         else if ((pJson[pos] == '"') && (*pEnd == -1))
         {
            *pEnd = pos - 1;
         }
         /* If key is found and separator is found, then we stop */
         else if ((*pBegin != -1) && (*pEnd != -1) && pJson[pos] == ':')
         {
            bResult = true;
            pos++;/* Cursor is set to the character next to colon, which is a beginning of value */
            break;
         }
      }
   }

#ifdef MULTITHREAD_ENABLED
   pState->pos = pos;
#endif

   return bResult;
}

bool jett_collectionBegin(
#ifdef MULTITHREAD_ENABLED
   sJettState* pState
#else
   void
#endif
)
{
#ifdef MULTITHREAD_ENABLED
   /* Initialize local variables we will be using to traverse the JSON */
   int pos = pState->pos;
   int length = pState->length;
   char* pJson = pState->pJson;
#endif

   bool bResult = false;
   for (; (pJson[pos] != '\0') && (pos < length); pos++)
   {
      /* Look for an array or object start */
      if ((pJson[pos] == '[') || (pJson[pos] == '{'))
      {
         pos++;//Set pointer to the next character
         bResult = true;
         break;
      }
      /* Skip any whitespace character */
      else if ((pJson[pos] == ' ') || (pJson[pos] == '\r') || (pJson[pos] == '\t') || (pJson[pos] == '\n'))
      {
         continue;
      }
      /* No collection begin found, aborting */
      else
      {
         break;
      }
   }

#ifdef MULTITHREAD_ENABLED
   pState->pos = pos;
#endif

   return bResult;
}

bool jett_collectionEnd(
#ifdef MULTITHREAD_ENABLED
   sJettState* pState
#else
   void
#endif
)
{
#ifdef MULTITHREAD_ENABLED
   /* Initialize local variables we will be using to traverse the JSON */
   int pos = pState->pos;
   int length = pState->length;
   char* pJson = pState->pJson;
#endif

   bool bResult = false;
   for (; (pJson[pos] != '\0') && (pos < length); pos++)
   {
      /* When we found the end of collection, we need to find end of nesting collection or next element in the array */
      if (bResult)
      {
         if ((pJson[pos] == ']') || (pJson[pos] == '}') || (pJson[pos] == ','))
         {
            //Skip over comma
            if (pJson[pos] == ',')
            {
               pos++;
            }
            break;
         }
      }
      /* Look for collection end */
      else if ((pJson[pos] == ']') || (pJson[pos] == '}'))
      {
         bResult = true;
         /* Continue looking Look for the actual end of the element */
      }
      /* Skip any whitespace character */
      else if ((pJson[pos] == ' ') || (pJson[pos] == '\r') || (pJson[pos] == '\t') || (pJson[pos] == '\n'))
      {
         continue;
      }
      /* No collection end found, aborting */
      else
      {
         break;
      }
   }

#ifdef MULTITHREAD_ENABLED
   pState->pos = pos;
#endif

   return bResult;
}

bool jett_getValue(
#ifdef MULTITHREAD_ENABLED
   sJettState* pState,
#endif
   int* pBegin, int* pEnd)
{
#ifdef MULTITHREAD_ENABLED
   /* Initialize local variables we will be using to traverse the JSON */
   int pos = pState->pos;
   int length = pState->length;
   char* pJson = pState->pJson;
#endif

   *pBegin = -1;
   *pEnd = -1;
   bool bResult = false;
   bool bStringFound = false;
   for (; (pJson[pos] != '\0') && (pos < length); pos++)
   {
      /* If we found an end of the value in object, array or found next element then stop traversing, regardless of success or failure */
      if ((pJson[pos] == '}') || (pJson[pos] == ']') || (pJson[pos] == ','))
      {
         /* If we only found beginning of value and it is not a string, then our search is over */
         if (!bResult && !bStringFound && (*pBegin != -1))
         {
            *pEnd = pos - 1;
            bResult = true;
         }

         if (pJson[pos] == ',')
         {
            /* Skip to next value */
            pos++;
         }

         /* If open string was found, then we just break with failure */
         break;
      }
      /* If value position is not found, continue the search */
      else if (!bResult)
      {
         /* A string candidate found */
         if (pJson[pos] == '"')
         {
            if (!bStringFound)
            {
               pos++;
               *pBegin = pos;
               bStringFound = true;
            }
            else
            {
               *pEnd = pos - 1;
               pos++;
               bResult = true;
               /* We do not break here, keep looking for the end symbol */
            }
         }
         /* Look for anything else */
         else if (!bStringFound)
         {
            /* Use the whitespace to find begin and end of the primitive value */
            if ((pJson[pos] == ' ') || (pJson[pos] == '\r') || (pJson[pos] == '\t') || (pJson[pos] == '\n'))
            {
               if (*pBegin != -1)
               {
                  *pEnd = pos - 1;
                  bResult = true;
               }
            }
            else if (*pBegin == -1)
            {
               *pBegin = pos;
            }
         }
      }
   }

#ifdef MULTITHREAD_ENABLED
   pState->pos = pos;
#endif

   return bResult;
}
