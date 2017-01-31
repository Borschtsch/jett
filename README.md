# jett
JSON Embedded Tree Traversal

## About
**jett** is not just another JSON parser written in C. It is a JSON tree traversal tool specifically designed for embedded applications that require JSON parsing and have memory constraints.

## Why do I need a tree traversal instead of a parser?
Any JSON parser requires memory allocation. They either need memory to store parsed values or they need memory allocation to store metadata that describes the JSON file. And here is a simple rule - the bigger the JSON file, the higher parser's memory consumption. This is an overhead you do not want to pay for. And the most important, the application still needs to walk the parser data structure to fetch specific keys, arrays and objects or the metadata that describes them and then somehow process this data. For the specific example you can take a look at [AWS IoT Embedded SDK](https://github.com/aws/aws-iot-device-sdk-embedded-C/tree/v2.1.1).

**jett** has such application responsibilities in mind and was developed to avoid intermediate data storage and allow the application to deal with JSON data directly in its own way. It does not need any memory allocation, and its state is fully described with a current cursor position.

## Isn't it too many responsibilities for application?
Almost any embedded microcontroller application is predefined by design, so if your system expects a JSON file then it is already aware of any JSON key possible and their values format. It is an application responsibility to know how to fill strings, numericals, arrays or structures. **jett** is just removing a middleman.
