#include "interface.h"
#include "PartitionReadWrite.h"
#include "PathValidation.h"


int CreateAndFormat_Partition()
{
    FILE *f = fopen(Partition,"w");
    if(!f)
    {
        printf("Partition file didnt open!!!!\n");
        return -1; // file error
    }

    // 64 MB partition - 1 byte already fixed for name
    // 1MB  = 1024 KB and 1KB = 1024 Bytes
    
    for (int i = 0; i < (64 * 1024*1024); i++) {
        fwrite("0", 1, 1, f);
    }

    fclose(f);
    return 0; // success    
}



int main()
{
    // 1st of all we will create and format a text file which will be our whole partition
    // any 1 charater in the text file is 1 byte + 1 byte fixed for name
    int ISsuceess= CreateAndFormat_Partition();
    if(ISsuceess!=-1)
    {   
        printf("partition Created Successfully\n");
    }
    else
    {
        printf("Partition Creation Failed\n");
    }

    // now initializing virtual directory table
    DIR_Table_arr virt_DirectoryTable;
    // initializing all blocks as free in the start
    for(int i=0;i<256;i++)
    {
        virt_DirectoryTable.arr[i].is_free=1;
    }
     virt_DirectoryTable.size = 0; //1st already taken for root directory
    DIR_Table_Writer(&virt_DirectoryTable,"C",0,0,-1,1); // giving 0th index entry to root directory

    // initiliazing virtual and Partition FAT
    virtFAT virtualFAT;
    virtualFAT.size = 0;
    FILE* FAT_file = fopen(Partition, "r+b");
    if (FAT_file == NULL) {
        perror("Error opening file");
        return -1;
    }

    for (int i = 0; i < 500; i++)
    {
        int initialVal = -1;  // Initially all are available
        virtualFAT.FATarr[i] = initialVal; // Update the virtual free list
        virtualFAT.size += 1;

        // Seek to the correct position in the partition
        if (fseek(FAT_file, FL_Start + i, SEEK_SET) != 0)
        {
            perror("Error while seeking to initialize Free list in Partition\n");
            fclose(FAT_file);
            return -1;
        }

        // Use fprintf to write the integer to the partition in a human-readable format
        if (fprintf(FAT_file, "%d", initialVal) < 0)
        {
            perror("Error while writing to the Free list in partition\n");
            fclose(FAT_file);
            return -1;
        }
    }
    fclose(FAT_file);


    // initializing virtual and Partition Free List
    virtFreeList virtFL;
    virtFL.size=0;
    FILE* Free_Ls_file = fopen(Partition, "r+b");
    if (Free_Ls_file == NULL) {
        perror("Error opening file");
        return -1;
    }

    for (int i = 0; i < 500; i++)
    {
        int available = 1;  // Initially all are available
        virtFL.FLarr[i] = available; // Update the virtual free list
        virtFL.size += 1;

        // Seek to the correct position in the partition
        if (fseek(Free_Ls_file, FL_Start + i, SEEK_SET) != 0)
        {
            perror("Error while seeking to initialize Free list in Partition\n");
            fclose(Free_Ls_file);
            return -1;
        }

        // Use fprintf to write the integer to the partition in a human-readable format
        if (fprintf(Free_Ls_file, "%d", available) < 0)
        {
            perror("Error while writing to the Free list in partition\n");
            fclose(Free_Ls_file);
            return -1;
        }
    }
    fclose(Free_Ls_file);
    while(1)
    {
            //command control
        char* complete_command=interface(); 
        cJSON* json_str=commandParser(complete_command);
        cJSON *command = cJSON_GetObjectItemCaseSensitive(json_str, "command");
        cJSON *path= cJSON_GetObjectItemCaseSensitive(json_str, "path");
        //printf("%s\n", item->valuestring); 

        // Command Conditions
        if (strcmp(command->valuestring, "touch") == 0)
        {
            // TODO: check the path, is it correct!!
            cJSON* reversed_path= splitPathToJSON(path->valuestring);
            if(verify_path(&virt_DirectoryTable,reversed_path)==0)
            {
                // now create the file
                cJSON* filename = cJSON_GetArrayItem(reversed_path, 0);
                cJSON* itsParent = cJSON_GetArrayItem(reversed_path, 1);
                createFile_and_DIR(&virt_DirectoryTable,&virtFL,filename->valuestring,itsParent->valuestring,0);
            }
        }
        else if(strcmp(command->valuestring, "mkdir") == 0)
        {
            // for making a directory
            cJSON* reversed_path= splitPathToJSON(path->valuestring);
            if(verify_path(&virt_DirectoryTable,reversed_path)==0)
            {
                // now create the file
                cJSON* DIR_to_create = cJSON_GetArrayItem(reversed_path, 0);
                cJSON* itsParent = cJSON_GetArrayItem(reversed_path, 1);
                createFile_and_DIR(&virt_DirectoryTable,&virtFL,DIR_to_create->valuestring,itsParent->valuestring,1);
            }
        }
        else if(strcmp(command->valuestring, "ls") == 0)
        {
            // list all files and directories
            for(int i=0;i< virt_DirectoryTable.size;i++)
            {
                if(virt_DirectoryTable.arr[i].start_Block==-1)// directory has no size
                {
                    printf("Directory %s with 0 size, parent %d \n",virt_DirectoryTable.arr[i].name,virt_DirectoryTable.arr[i].parent);
                }
                else{
                    printf("File %s with %d size, parent %d \n",virt_DirectoryTable.arr[i].name,virt_DirectoryTable.arr[i].size,virt_DirectoryTable.arr[i].parent);
                }
            }
        }
        else if(strcmp(command->valuestring, "dldir") == 0)
        {
            cJSON* reversed_path= splitPathToJSON(path->valuestring);
        
            if(verify_path(&virt_DirectoryTable,reversed_path)==0)
            {
                // now create the file
                cJSON* DIR_to_delete = cJSON_GetArrayItem(reversed_path, 0);
                delete_file(&virt_DirectoryTable,&virtualFAT,&virtFL,DIR_to_delete->valuestring);
            }
        }
        else if(strcmp(command->valuestring, "exit") == 0)
        {
            printf("Exiting Terminal!!!\n");
            break;
        }
    }
    
}










