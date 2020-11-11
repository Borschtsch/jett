## What is jett?
**jett** stands for JSON Embedded Tree Traversal.

## You know, I've seen a lot...
**jett** is not just another JSON parser written in C. It is a JSON tree traversal tool specifically designed for microcontroller systems that require sigle-pass JSON parsing on one side and have strict memory requirements on another side. As an example, on for Cortex-M4 core the code produced is around 1K in size and RAM consumption is just 12 bytes.

## Why do I need a tree traversal instead of a normal parser?
Any JSON parser requires memory allocation. They need memory to store either parsed values or metadata that describes the JSON file so you can access these values faster at any time. But do you need to access JSON document more than once during its lifetime? Even if this document is downloaded from Internet you transform it into application parameters and then it can be discarded.
Another problem is the bigger the JSON file, the higher parser's memory consumption. This is an overhead you do not want to pay for. For the specific example you can take a look at [AWS IoT Embedded SDK](https://github.com/aws/aws-iot-device-sdk-embedded-C/tree/v2.1.1) which uses [Jsmn](https://github.com/zserge/jsmn) parser to read the information from device Shadow. Depending on the compiler the Jsmn parser will require from 7 to 16 bytes of RAM for each JSON key, object or object member, array or array element. If device Shadow has a hundred of elements then the minimum RAM requirement to use Jsmn starts from 700-1600 bytes while **jett** requires only 12 bytes regardless of the size of the Shadow.
And the most important, the application still has to walk the parser data structure to fetch specific keys, arrays and objects or the metadata that describes them and then somehow process this data based on its own rules.
When designed, the **jett** had such application responsibilities in mind, it avoids intermediate data storage and allows the application to deal with JSON data directly in its own way. It does not need extra memory allocation, because traversal is done in a single-pass and is fully defined with an application logic and current cursor position.

## Isn't it too many responsibilities for the application?
Almost any embedded microcontroller application is predefined by design, so if your system expects a JSON file with specific fields and data type then it is already aware of all application-specific JSON key/format pairs. It is an application responsibility to know how to fill in strings, numericals, arrays or structures based on the keys. **jett** is just removing a man in the middle and avoids creating any metadata for JSON document that can be directly accessed.

## How do I use it to walk through the JSON document?
Your software knows the idea about document structure anyways. Think of your JSON document as a library API - you always link your software against specific version of library it can work with. Traversal could be a manual walk-through or it could be fully automated by defining what keys you expect to find and what their data types, and a way to describe arrays and structure layouts.
The very first thing you should do - initialize the traversal with jett_init() and open the main object or array by issuing jett_collectionBegin().
If root element is an object then request next key by jett_findKey(). If it is certainly an array of primitive values - use jett_getValue() to extract primitive value.
If the key you requested is an object key, then you have to call jett_collectionBegin() again.

The library supports multithreaded mode when the state of the traversal library is stored in the tiny state structure and this is up to developer to provide allocation for it. State structure contains the pointer to the JSON string, cursor position and string length - when you walk down the JSON tree using some automation functions just pass a pointer to this structure to bring the traverse state where its needed.

## Is the library error free?
Any feedback is warmly welcomed :) Also there is no official release yet and I need to do more testing, primarily test the malformed document. Based on my experience the malformed JSON may feed the application with incorrect data. But regardless of that, the application has to deal with a raw data and it takes extra caution to make sure there is no data corruption or buffer overflow when parsing data directly from JSON. I didn't do a lot of exceptions testing so I encourage you to strictly rely on your application which defines the type and size of each value, array or structure fields that are used in the system.

## Let me take a closer look!
Below is a simple example on how to use the API. This is a non-automated way of accessing the data, it is a key position dependent, easy to make error, so only works for small files.
Using this API one can build its own parser that could define a way to describe primitive types, arrays and structure layouts to automate feeding application parameters directly from JSON text string.

```
#include "jett.h"

char jsonFile[] = "{ \"players\": [ { \"name\" : \"Flinn\", \"integer\" : 1 }, { \"name\" : \"Tr0n\", \"integer\" : 2 } ] }";

void traverse(void)
{
   /* This example omits the use of response code as it is not practical */
   jett_init(jsonFile, strlen(jsonFile));

   int begin;
   int end;

   /* Open the object first and get 'players' key */
   jett_collectionBegin();
   jett_findKey(&begin, &end);
   /* A sanity check would be verifying that size of key and 'players' is the same before strncmp */
   if (strncmp(jsonFile[begin], "players", end - begin) == 0)
   {
      /* Open array */
      jett_collectionBegin();
      /* Skip Flinn's object */
      jett_collectionBegin(); 
      jett_collectionEnd();

      /* Open Tr0n's object */
      jett_collectionBegin();
      /* Find 'name' key */
      jett_findKey(&begin, &end);
      if (strncmp(jsonFile[begin], "name", end - begin) == 0)
      {
         jett_getValue(&begin, &end);

         /* Scanning the value */
         char someValue[10];

         /* This one is dangerous, what if JSON value is bigger then 'somevalue'? Application should always check the size. */
         strncpy(&name[0], &pJson[begin], end - begin);
         someValue[end - begin] = '\0';
      }
      
      /* Read the 'integer' value */
      jett_findKey(&begin, &end);
      /* A sanity check would be verifying that size of key and 'integer' is the same before strncmp */
      if (strncmp(jsonFile[begin], "integer", end - begin) == 0)
      {
         jett_getValue(&begin, &end);
         
         unsigned int someNumber;
         /* sscanf is good here, it will check the string for number correctness, 
          * but a check could be also made that number is not negative.
          */
         sscanf(&pJson[begin], "%u", &someNumber);
      }
      
      /* No need to close array, since we finished processing here */
   }
}
```
