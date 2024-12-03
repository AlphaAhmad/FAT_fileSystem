#include "utility.h"


// Function to find the directory by name using linear search
DirTable_Struct* find_directory_by_name(DIR_Table_arr* dir_table, const char* name) {
    for (size_t i = 0; i < dir_table->size; ++i) {
        if (dir_table->arr[i].name != NULL && strcmp(dir_table->arr[i].name, name) == 0) {
            return &dir_table->arr[i];
        }
    }
    return NULL; // Directory not found
}

// Function to verify the path by checking the parent-child relationships
int verify_path(DIR_Table_arr* dir_table, cJSON* json_obj) {
    // Extract the path from the JSON object
    cJSON* path_array = json_obj;
    if (!cJSON_IsArray(path_array)) {
        printf("Error: JSON path is not an array.\n");
        return -1;
    }

    // Get the number of directories in the path
    size_t path_size = cJSON_GetArraySize(path_array);
    if (path_size < 2) {
        printf("Error: Path should contain at least the file and one parent directory.\n");
        return -1;
    }

    // Start with the second element (parent directory)
    cJSON* first_dir_item = cJSON_GetArrayItem(path_array, 1);
    if (!cJSON_IsString(first_dir_item)) {
        printf("Error: The first directory in the path is not a valid string.\n");
        return -1;
    }

    const char* first_dir_name = first_dir_item->valuestring;

    // If the first directory is the base directory 'C', return immediately
    if (strcmp(first_dir_name, "C") == 0) {
        printf("Path is valid: Starting from base directory 'C'.\n");
        return 0;
    }

    // Find the first directory in the table
    DirTable_Struct* current_dir = find_directory_by_name(dir_table, first_dir_name);
    if (!current_dir) {
        printf("Path is invalid: Directory '%s' not found.\n", first_dir_name);
        return -1;
    }

    // Traverse upwards towards the root directory
    for (size_t i = 2; i < path_size; ++i) {
        cJSON* parent_dir_item = cJSON_GetArrayItem(path_array, i);
        if (!cJSON_IsString(parent_dir_item)) {
            printf("Error: Path contains an invalid directory name.\n");
            return -1;
        }

        const char* expected_parent_name = parent_dir_item->valuestring;

        // Check if the parent of the current directory matches the expected directory in the json_path
        if (current_dir->parent >= dir_table->size) {
            printf("Invalid parent index for directory '%s'. Parent index exceeds table size.\n", current_dir->name);
            return -1;
        }

        // Find the parent directory using the parent index
        DirTable_Struct* parent_dir = &dir_table->arr[current_dir->parent];
        if (parent_dir == NULL || strcmp(parent_dir->name, expected_parent_name) != 0) {
            // Mismatch happened, print the name of the directory causing the issue
            printf("Mismatch happened at directory '%s'. Expected parent: '%s', found parent: '%s'.\n", 
                   current_dir->name, expected_parent_name, parent_dir->name);
            return -1;
        }

        // Move to the parent directory for the next iteration
        current_dir = parent_dir;
    }

    // If we reach here, the path is valid
    printf("The path is valid.\n");
    return 0;
}