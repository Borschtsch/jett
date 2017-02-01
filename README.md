## What is jett?
**jett** stands for JSON Embedded Tree Traversal.

## You know, I've seen a lot...
**jett** is not just another JSON parser written in C. It is a JSON tree traversal tool specifically designed for embedded applications that require JSON parsing on one side and have strict memory requirements on another.

## Why do I need a tree traversal instead of a parser?
Any JSON parser requires memory allocation. They either need memory to store parsed values or they need memory allocation to store metadata that describes the JSON file. And here is a simple rule - the bigger the JSON file, the higher parser's memory consumption. This is an overhead you do not want to pay for. And the most important, the application still needs to walk the parser data structure to fetch specific keys, arrays and objects or the metadata that describes them and then somehow process this data. For the specific example you can take a look at [AWS IoT Embedded SDK](https://github.com/aws/aws-iot-device-sdk-embedded-C/tree/v2.1.1).

When designed, the **jett** had such application responsibilities in mind and is developed to avoid intermediate data storage and allow the application to deal with JSON data directly in its own way. It does not need extra memory allocation, because its state is fully defined with an application logic and current cursor position.

## Isn't it too many responsibilities for the application?
Almost any embedded microcontroller application is predefined by design, so if your system expects a JSON file then it is already aware of any JSON key and value format possible. It is an application responsibility to know how to fill in strings, numericals, arrays or structures. **jett** is just removing a man in the middle.

## Is the library error free?
Any feedback is warmly welcomed :) Also there is no official release yet and I need to do more testing.
Based on my experience the malformed JSON may feed the application with incorrect data. But regardless of that, the application has to deal with a raw data and it takes extra caution to make sure there is no data corruption or buffer overflow when parsing data directly from JSON. I didn't do many exceptions testing and I encourage you to strictly rely on your application which defines the type and size of each value, array or structure fields that are used in the system.

## Well... and how much memory it uses?
On Cortex-M4 the code is 1K and RAM is 12 bytes. In multi-threaded mode RAM could be allocated on stack or per thread.

## Let me take a closer look!
Below is a simple example on how to use the API. This is a non-automated way of accessing the data, it is position dependent, easy to make error, but works for small files.
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

   /* Open the object first and get 'items' key */
   jett_collectionBegin();
   jett_findKey(&begin, &end);
   /* A sanity check would be verifying that size of key and 'players' is the same before strncmp */
   if (strncmp(jsonFile[begin], "players", (end - begin) + 1) == 0)
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
      if (strncmp(jsonFile[begin], "name", (end - begin) + 1) == 0)
      {
         jett_getValue(&begin, &end);

         /* Scanning the value */
         char someValue[10];

         /* This one is dangerous, what if JSON value is bigger then 'somevalue'? Application should always check the size. */
         strncpy(&name[0], &pJson[begin], (end - begin) + 1);
         someValue[(end - begin) + 1] = '\0';
      }
      
      /* Read the 'integer' value */
      jett_findKey(&begin, &end);
      /* A sanity check would be verifying that size of key and 'integer' is the same before strncmp */
      if (strncmp(jsonFile[begin], "integer", (end - begin) + 1) == 0)
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
