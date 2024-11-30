#include "interface.h"

int CreateAndFormat_Partition()
{
    FILE *f = fopen("partition.txt","w");
    if(!f)
    {
        printf("Partition file didnt open!!!!\n");
        return -1; // file error
    }

    // 64 MB partition - 1 byte already fixed for name
    // 1MB  = 1024 KB and 1KB = 1024 Bytes
    for(int i=0;i<(64*1024*1024)-1;i++)
    {
        fprintf(f,"%d",0);
    }

    fclose(f);

    return 0; // success    
}



int main()
{
    // 1st of all we will create and format a text file which will be our whole partition
    // any 1 charater in the text file is 1 byte + 1 byte fixed for name
    if(CreateAndFormat_Partition()==0)
    {
        printf("partition Created Successfully\n");
    }
    else
    {
        printf("Partition Creation Failed\n");
    }
    // now initializing virtual directory table
    DynamicArray virt_DirectoryTable;
    virt_DirectoryTable.size = sizeof(virt_DirectoryTable.arr);

    // initiliazing virtual FAT
    int virtualFAT [500];  // 500 Blocks of memory

    // initializing virtual Free List
    int FreeList [500];
    
    //return the command we want to perform
    char* complete_command=interface(); 

    cJSON* json_str=commandParser(complete_command);
    cJSON *item = cJSON_GetObjectItemCaseSensitive(json_str, "command");
    //printf("%s\n", item->valuestring); 

    // Command Conditions
    if(item->valuestring == "touch")
    {
        // for making a file
    }
    else if(item->valuestring == "mkdir")
    {
        // for making a directory

    }
}






// int main()
// {
//     int x=7;
//     //printf("%p\n", (void *)&x);
//     printf("%d\n", x);
//     for (int i=0;i<=7;i++)
//     {
//         printf("%d ",x>>i && 1);
//     }
//     return 0;
// }




