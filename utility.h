// Command: gcc file_system.c -o file_system -I./cJSON -L./cJSON -lcjson

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cjson/cJSON.h> 
#ifndef UTILITY_H
#define UTILITY_H
typedef struct 
{
    char* name; // 1 character = 1byte // total 64 bytes
    __uint16_t parent;
    __uint16_t size; // 0 for directory/folder
    __uint16_t start_Block;  
    char is_free; // 0 or 1
    //int myindex; // this will be only in virtual Dir Table for fast deletion
}DirTable_Struct;


// Dynamic
typedef struct {
    DirTable_Struct arr[256];
    size_t size;
}DIR_Table_arr;

typedef struct 
{
    __uint8_t FATarr [500];  // 500 Blocks of memory
    int size;
}virtFAT;

typedef struct 
{
    __uint8_t FLarr [500];  // 500 Blocks of memory       
    int size;
}virtFreeList;


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


// Function to split a path string into components and return as a cJSON array
cJSON* splitPathToJSON(char* path) {
    if (path == NULL || strlen(path) == 0) {
        fprintf(stderr, "Error: Empty or NULL path string\n");
        return NULL;
    }

    // Create a cJSON array
    cJSON* jsonArray = cJSON_CreateArray();
    if (!jsonArray) {
        fprintf(stderr, "Error: Failed to create JSON array\n");
        return NULL;
    }

    // Copy the path to avoid modifying the original string
    char* pathCopy = strdup(path);
    if (!pathCopy) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        cJSON_Delete(jsonArray);
        return NULL;
    }

    // Tokenize the path string based on "/"
    char* token = strtok(pathCopy, "/");
    while (token != NULL) {
        // Add each token to the JSON array
        cJSON_AddItemToArray(jsonArray, cJSON_CreateString(token));
        token = strtok(NULL, "/");
    }

    // Free the copied path string
    free(pathCopy);

    return jsonArray;
}


// Function to print a whole cJSON object
void print_cjson_object(cJSON *json_object) {
    // Access the resulting JSON array in reverse order
    if (json_object != NULL) {
        int size = cJSON_GetArraySize(json_object);
        for (int i = size - 1; i >= 0; i--) {
            cJSON* item = cJSON_GetArrayItem(json_object, i);
            printf("Component %d: %s\n", i, item->valuestring);
        }

        // Clean up the cJSON object
        cJSON_Delete(json_object);
    }
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




#endif // UTILITY_H

