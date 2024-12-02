#include "utility.h"

#define Partition "partition.txt"
        // for DIR table + for FAT + 1  
#define FL_Start 256*sizeof(DirTable_Struct)+500+1
#define FAT_Start 256*sizeof(DirTable_Struct)+1
#define PartitionBLock sizeof(DirTable_Struct)
#define NameRsrSpace 64 // 64 bytes reserved for name


long Calc_DirTable_Offset(DIR_Table_arr* vir_DIRtable)
{
    int i;
    for (i=0;i<vir_DIRtable->size && i<256;i++)
    {
        if(vir_DIRtable->arr[i].is_free==1)
        {
            return i*PartitionBLock; // found free block in between
        }
    }
    // if DIR Table is full and no block is free in between
    if (i == 256) 
    {
        perror("DIR limit is full, no more directories or files can be added!!!\n");
        return -1;
    }
    else{ // if we havent reached end then allocate new memory
        return i*PartitionBLock;
    }
}


// Writes entries on DIR Table
int DIR_Table_Writer(DIR_Table_arr* vir_DIRtable,char* name, __uint16_t Parent,__uint16_t size,__uint16_t strt_block,int isDirectory)
{
    // name cannot exceed 64 bytes
    if(strlen(name) < NameRsrSpace)
    {
        long Free_BlockOffset = Calc_DirTable_Offset(vir_DIRtable);
        if (Free_BlockOffset == -1)
        {
            perror("No space in vir_DIRtable");
            return -1;
        }

        FILE* file = fopen(Partition,"r+b");
        if (file == NULL) {
            perror("Error opening file");
            return -1;
        }

        // 1 name
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
        
        // 2 Parent 
        if (fseek(file, Free_BlockOffset+NameRsrSpace, SEEK_SET) != 0) {
            perror("Error while seeking to write file/DIR parent");
            fclose(file);
            return -1;
        }
        if (fwrite(&Parent, sizeof(Parent), 1, file)!=1) {
            perror("Error writing parent of file/DIR to partition\n");
            fclose(file); // Always close before returning
            return -1;
        }

        // 3 size
        if (fseek(file, Free_BlockOffset+NameRsrSpace+sizeof(__uint16_t), SEEK_SET) != 0) {
            perror("Error while seeking to write file size");
            fclose(file);
            return -1;
        }
        if(isDirectory)
        {
            size=0;
            strt_block=0; // start block pointing to zero 
        }
        if (fwrite(&size,sizeof(size),1,file)!=1) {
            perror("Error writing size of file/DIR to partition\n");
            fclose(file); // Always close before returning
            return -1;
        }

        // 4 Start Block
        if (fseek(file, Free_BlockOffset+NameRsrSpace+2*sizeof(__uint16_t), SEEK_SET) != 0) {
            perror("Error while seeking to write File start block");
            fclose(file);
            return -1;
        }
        if (fwrite(&strt_block, sizeof(strt_block), 1, file)!=1) {
            perror("Error while writing start block of file\n");
            fclose(file); // Always close before returning
            return -1;
        }
        
        // 5
        if (fseek(file, Free_BlockOffset+NameRsrSpace+3*sizeof(__uint16_t), SEEK_SET) != 1) {
            perror("Error while seeking to write DIR block unavailable int");
            fclose(file);
            return -1;
        }
        int is_available=0;
        size_t len2 = fwrite(&is_available, sizeof(is_available), 1, file);
        if (len2 < sizeof(is_available)) {
            perror("Error writing block unavailable int\n");
            fclose(file); // Always close before returning
            return -1;
        }
        
        // Close the file
        fclose(file);

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
        vir_DIRtable->arr[vir_DIRtable->size] = newlyCreated;
        vir_DIRtable->size++; // Increment the size of the virtual table
        return 0;
    }
    else
    {
        perror("File/Directory name exceeds 63 characters (64 bytes reserved including null terminator).\n");
        return -1;
    }
}

