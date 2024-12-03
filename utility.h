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
    int parent;
    int size; // 0 for directory/folder
    int start_Block;  
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
    int FATarr [500];  // 500 Blocks of memory
    int size;
}virtFAT;

typedef struct 
{
    int FLarr [500];  // 500 Blocks of memory       
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


// Function to split a path string into components and return as a cJSON array in reverse order
cJSON* splitPathToJSON(char* path) {
    // Error handling: Check if the path is NULL or empty
    if (path == NULL || strlen(path) == 0) {
        fprintf(stderr, "Error: Empty or NULL path string\n");
        return NULL;
    }

    // Create a cJSON array to hold the path components
    cJSON* jsonArray = cJSON_CreateArray();
    if (!jsonArray) {
        fprintf(stderr, "Error: Failed to create JSON array\n");
        return NULL;
    }

    // Copy the path to avoid modifying the original string
    char* pathCopy = strdup(path);
    if (!pathCopy) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        cJSON_Delete(jsonArray); // Clean up the created JSON array
        return NULL;
    }

    // Tokenize the path string based on "/"
    char* token = strtok(pathCopy, "/");
    
    // Use a temporary list to store tokens in reverse order
    char* tokens[256];  // Assume no path is longer than 255 components
    int tokenCount = 0;

    // Tokenize the path into components
    while (token != NULL) {
        tokens[tokenCount] = strdup(token); // Store token in array
        if (!tokens[tokenCount]) {
            fprintf(stderr, "Error: Memory allocation failed for token\n");
            // Clean up previously allocated memory
            for (int i = 0; i < tokenCount; i++) {
                free(tokens[i]);
            }
            free(pathCopy);
            cJSON_Delete(jsonArray); // Clean up the JSON array
            return NULL;
        }
        tokenCount++;
        token = strtok(NULL, "/");
    }

    // Add tokens to JSON array in reverse order
    for (int i = tokenCount - 1; i >= 0; i--) {
        cJSON_AddItemToArray(jsonArray, cJSON_CreateString(tokens[i]));
        free(tokens[i]);  // Free the memory allocated for each token
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


// Function to delete a file and free its blocks
int delete_file(DIR_Table_arr* dir_table, virtFAT* fat, virtFreeList* free_list, const char* file_name) {
    if (!dir_table || !fat || !free_list || !file_name) {
        fprintf(stderr, "Error: Invalid arguments to delete_file.\n");
        return -1;
    }

    // Search for the file in the directory table
    for (size_t i = 0; i < dir_table->size; ++i) {
        DirTable_Struct* entry = &dir_table->arr[i];
        if (entry->is_free == 0 && strcmp(entry->name, file_name) == 0) {
            int current_block = entry->start_Block;

            // Traverse the FAT and free the blocks
            while (current_block != -1) {
                int next_block = fat->FATarr[current_block];  // Get the next block
                fat->FATarr[current_block] = -1;             // Reset the FAT entry
                free_list->FLarr[current_block] = 1;         // Mark the block as free
                current_block = next_block;                 // Move to the next block
            }

            // Clear the directory entry
            free(entry->name);           // Free the name
            entry->name = NULL;          // Reset the name
            entry->parent = -1;          // Reset parent
            entry->size = 0;             // Reset size
            entry->start_Block = -1;     // Reset starting block
            entry->is_free = 1;          // Mark as free

            printf("File '%s' deleted, and its blocks have been freed.\n", file_name);
            return 0; // Success
        }
    }

    // File not found
    printf("File '%s' not found in the directory table.\n", file_name);
    return -1; // Failure
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

