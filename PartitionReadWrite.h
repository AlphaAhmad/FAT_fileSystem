#include "utility.h"

#define Partition "partition.txt"
// for DIR table + for FAT + 1  
#define FL_Start 256 * sizeof(DirTable_Struct) + 500 + 1
#define FAT_Start 256 * sizeof(DirTable_Struct) + 1
#define PartitionBLock sizeof(DirTable_Struct)
#define NameRsrSpace 64 // 64 bytes reserved for name

long Calc_DirTable_Offset(DIR_Table_arr* vir_DIRtable)
{
    int i;
    for (i = 0; i < vir_DIRtable->size && i < 256; i++)
    {
        if (vir_DIRtable->arr[i].is_free == 1)
        {
            return i * PartitionBLock; // found free block in between
        }
    }
    // if DIR Table is full and no block is free in between
    if (i == 256)
    {
        perror("DIR limit is full, no more directories or files can be added!!!\n");
        return -1;
    }
    else { // if we haven't reached end then allocate new memory
        return i * PartitionBLock;
    }
}

// Writes entries on DIR Table
int DIR_Table_Writer(DIR_Table_arr* vir_DIRtable, char* name, int Parent, int size, int strt_block, int isDirectory)
{
    // name cannot exceed 64 bytes
    if (strlen(name) < NameRsrSpace)
    {
        long Free_BlockOffset = Calc_DirTable_Offset(vir_DIRtable);
        if (Free_BlockOffset == -1)
        {
            perror("No space in vir_DIRtable");
            return -1;
        }

        if(isDirectory==1)
        {
            size=0;
            strt_block=-1;
        }

        FILE* file = fopen(Partition, "r+"); // Open file in append mode to write normally
        if (file == NULL) {
            perror("Error opening file");
            return -1;
        }

        // 1 name: Write the name as a string
        if (fseek(file, Free_BlockOffset, SEEK_SET) != 0) {
            perror("Error while seeking to write file/DIR name to partition\n");
            fclose(file);
            return -1;
        }
        size_t len1 = fwrite(name, sizeof(char), strlen(name), file);
        if (len1 < strlen(name)) {
            perror("Error writing file/DIR name to partition\n");
            fclose(file); // Always close before returning
            return -1;
        }

        // 2 Parent: Write Parent as an integer in readable form
        if (fseek(file, Free_BlockOffset + NameRsrSpace, SEEK_SET) != 0) {
            perror("Error while seeking to write file/DIR parent");
            fclose(file);
            return -1;
        }
        char parent_str[64];
        snprintf(parent_str, sizeof(parent_str), "%d", Parent); // Convert integer to string
        if (fwrite(parent_str, sizeof(char), strlen(parent_str), file) != strlen(parent_str)) {
            perror("Error writing parent of file/DIR to partition\n");
            fclose(file); // Always close before returning
            return -1;
        }

        // 3 size: Write size as an integer in readable form
        if (fseek(file, Free_BlockOffset + NameRsrSpace + sizeof(parent_str), SEEK_SET) != 0) {
            perror("Error while seeking to write file size");
            fclose(file);
            return -1;
        }
        char size_str[64];
        snprintf(size_str, sizeof(size_str), "%d", size); // Convert integer to string
        if (fwrite(size_str, sizeof(char), strlen(size_str), file) != strlen(size_str)) {
            perror("Error writing size of file/DIR to partition\n");
            fclose(file); // Always close before returning
            return -1;
        }

        // 4 Start Block: Write start block as an integer in readable form
        if (fseek(file, Free_BlockOffset + NameRsrSpace + sizeof(parent_str) + sizeof(size_str), SEEK_SET) != 0) {
            perror("Error while seeking to write file start block");
            fclose(file);
            return -1;
        }
        char strt_block_str[64];
        snprintf(strt_block_str, sizeof(strt_block_str), "%d", strt_block); // Convert integer to string
        if (fwrite(strt_block_str, sizeof(char), strlen(strt_block_str), file) != strlen(strt_block_str)) {
            perror("Error while writing start block of file\n");
            fclose(file); // Always close before returning
            return -1;
        }

        // 5 Block Unavailable Indicator: Write as an integer in readable form
        if (fseek(file, Free_BlockOffset + NameRsrSpace + sizeof(parent_str) + sizeof(size_str) + sizeof(strt_block_str), SEEK_SET) != 0) {
            perror("Error while seeking to write DIR block unavailable int");
            fclose(file);
            return -1;
        }
        int is_available = 0;
        char is_available_str[64];
        snprintf(is_available_str, sizeof(is_available_str), "%d", is_available); // Convert integer to string
        if (fwrite(is_available_str, sizeof(char), strlen(is_available_str), file) != strlen(is_available_str)) {
            perror("Error writing block unavailable int\n");
            fclose(file); // Always close before returning
            return -1;
        }

        // Close the file
        fclose(file);

        // Free existing name memory if present
        if (vir_DIRtable->arr[Free_BlockOffset/PartitionBLock].name != NULL) {
            free(vir_DIRtable->arr[vir_DIRtable->size].name);
        }

        // Now writing it on the virtual DIR table
        DirTable_Struct newlyCreated;
        newlyCreated.name = malloc(strlen(name) + 1); // Allocate memory for the name
        if (newlyCreated.name == NULL) {
            perror("Error allocating memory for name");
            return -1;
        }
        strcpy(newlyCreated.name, name); // Copy the name into the allocated memory
        newlyCreated.parent = Parent;
        newlyCreated.size = size;
        newlyCreated.start_Block = strt_block;
        newlyCreated.is_free = 0;

        // Add to the virtual DIR table
        int x= Free_BlockOffset/PartitionBLock;
        vir_DIRtable->arr[Free_BlockOffset/PartitionBLock] = newlyCreated;
        //if((Free_BlockOffset/PartitionBLock)==size-1)
        
            vir_DIRtable->size++; // Increment the size of the virtual table
        return 0;
    }
    else
    {
        perror("File/Directory name exceeds 63 characters (64 bytes reserved including null terminator).\n");
        return -1;
    }
}

// func to find index of file or directory in DIR table
int findIndex(DIR_Table_arr* DirTable,char* toSearch)
{
    int i;
    for (i = 0; i < DirTable->size && i < 256; i++)
    {
        if (strcmp(DirTable->arr[i].name,toSearch)==0)
        {
            return i; 
        }
    }
    if (i == 256)
    {
        perror("No such file or DIR exists\n");
        return -1;
    }
}

int FindFreeBlock_And_Assign(virtFreeList* FL)
{
    for(int i=0;i<FL->size;i++)
    {
        if(FL->FLarr[i]==1) // means we have found a free block
        {
            FL->FLarr[i]=0;
            return i;
        }
    }
    return -1;
}

int createFile_and_DIR(DIR_Table_arr* DIR_table,virtFreeList* FL,char* Filename, char* itsParent,int is_directory)
{
    int parent_index=findIndex(DIR_table,itsParent);
    int freeBlockIndex = FindFreeBlock_And_Assign(FL);
    if(parent_index != -1 && freeBlockIndex!=-1)
    {
        if(DIR_Table_Writer(DIR_table,Filename,parent_index,0,freeBlockIndex,is_directory)!=-1)
        {
            return 0;
        }
    }
    perror("Something went wrong while creating file\n");   
    return -1;
}