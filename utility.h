#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cjson/cJSON.h> 
typedef struct 
{
    char name[64]; // 1 character = 1byte // total 64 bytes
    __uint16_t parent;
    __uint16_t size; // 0 for directory/folder
    __uint16_t start_Block;  

}DirTable_Struct;


// Dynamic
typedef struct {
    DirTable_Struct arr[256];
    size_t size;
} DynamicArray;

cJSON* commandParser(char* command)
{
    cJSON *json = cJSON_CreateObject();
    char * token = strtok(command, " ");
    cJSON_AddStringToObject(json, "command", token); 

    token = strtok(NULL, " ");
    cJSON_AddStringToObject(json, "path", token); 
    
    // char *json_str = cJSON_Print(json);
    // cJSON_free(json);
    return json;
}

// void initArray(DynamicArray *array, int initialCapacity) {
//     array->arr = (DirTable_Struct *)malloc(initialCapacity * sizeof(DirTable_Struct));
//     array->capacity = initialCapacity;
//     array->size = 0;
// }

// void push(DynamicArray *array, DirTable_Struct value) {
//     if (array->size == array->capacity) {
//         // Double the capacity
//         array->capacity *= 2;
//         array->arr = (DirTable_Struct *)realloc(array->arr, array->capacity * sizeof(int));
//     }
//     array->arr[array->size++] = value;
// }






