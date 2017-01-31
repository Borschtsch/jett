# jett
JSON Embedded Tree Traversal

## What is jett?
**jett** is not just another JSON parser written in C. It is a JSON tree traversal tool specifically designed for embedded applications that require JSON parsing while having strict memory constraints.

## Why do I need a tree traversal instead of a parser?
Any JSON parser requires memory allocation. They either need memory to store parsed values or they need memory allocation to store metadata that describes the JSON file. And here is a simple rule - the bigger the JSON file, the higher parser's memory consumption. This is an overhead you do not want to pay for. And the most important, the application still needs to walk the parser data structure to fetch specific keys, arrays and objects or the metadata that describes them and then somehow process this data. For the specific example you can take a look at [AWS IoT Embedded SDK](https://github.com/aws/aws-iot-device-sdk-embedded-C/tree/v2.1.1).

**jett** has such application responsibilities in mind and was developed to avoid intermediate data storage and allow the application to deal with JSON data directly in its own way. It does not need extra memory allocation, because its state is fully defined with an application logic and current cursor position.

## Isn't it too many responsibilities for the application?
Almost any embedded microcontroller application is predefined by design, so if your system expects a JSON file then it is already aware of any JSON key possible and values format. It is an application responsibility to know how to fill in strings, numericals, arrays or structures. **jett** is just removing a man in the middle.

## How stable is the library?
First of all, any feedback is warmly welcomed. Based on my experience the malformed JSON may feed the application with incorrect data. But regardless of that the application has to deal with a raw data and it takes extra caution to make sure there is no data corruption or buffer overflow when parsing data directly from JSON. I didn't do many exceptions testing and I encourage you to strictly rely on your application which defines the type and size of each value, array or structure fields that are used in the system.

## Well... how much memory it uses?
On Cortex-M4 code is 1K and RAM is 12 bytes. In multi-threaded mode RAM could be allocated on stack or per thread.

## Example
Below is a simple example on how to use the API. Using this API one can build its own parser that will define a way to describe primitive types, arrays and structure layouts to automate feeding application parameters directly from JSON file.

```
#include "jett.h"

char jsonFile[] = "{ \"items\": [ { \"index\": 1}, { \"name\" : \"Glenn\", \"bool\" : true } ] }";

void traverse(void)
{
   /* This example omits the use of response code as it is not practical */
   jett_init(jsonFile, strlen(jsonFile));

   int begin;
   int end;

   /* Open the object first and get 'items' key */
   jett_collectionBegin();
   jett_findKey(&begin, &end);
   if (strncmp(jsonFile[begin], "items", (end - begin) + 1) == 0)
   {
      /* Open array and object */
      jett_collectionBegin();
      jett_collectionBegin();
      /* Find 'index' key */
      jett_findKey(&begin, &end);
      if (strncmp(jsonFile[begin], "index", (end - begin) + 1) == 0)
      {
         jett_getValue(&begin, &end);

         /* Scanning the value */
         int index;
         sscanf(pJson[begin], "%u", &index);
      }
      
      /* No need to close collections, since we found the key we need */
   }
}
```
