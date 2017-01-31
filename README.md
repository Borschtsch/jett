# jett
JSON Embedded Tree Traversal

## About
**jett** is not just another JSON parser written in C. It is a JSON tree traversal tool designed for embedded applications with strict memory limitations, but sophisticated enough to require JSON parsing. 

## Why do I need a tree traversal instead of a parser?
Any JSON parser requires memory allocation. They either need memory to store parsed values or they need memory allocation to store metadata that describes the JSON file. And here is a simple rule - the bigger the JSON file, the higher parser's memory consumption. And the most important, the application still needs to walk the parser data structure to fetch specific keys, arrays and objects or the metadata that describes them and then somehow process the data.

**jett** has the application responsibilities in mind and was developed to avoid intermediate data storage and allow the application to deal with JSON data directly in its own way. It does not need any memory allocation, and its current state consumes just 12 bytes.

## Isn't it too many responsibilities for application?
Almost any embedded microcontroller application is predefined by design, so if your system expects a JSON file then it is already aware of any JSON key possible and their values format. It is an application responsibility to know how to fill in arrays or C structures. **jett** is just removing a middleman.
