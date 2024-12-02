#include "interface.h"
#include "PartitionReadWrite.h"
                


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
    unsigned char zero_block[1024] = {0}; // A block of 1 KB filled with zeros
    for (int i = 0; i < (64 * 1024); i++) {
        fwrite(zero_block, 1, 1024, f);
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
    virt_DirectoryTable.size = 0; //++++++++++++++++++++++++++++++++++++++++++++

    // initiliazing virtual FAT
    virtFAT virtualFAT;
    virtualFAT.size = 0;
    
    // initializing Free List
    virtFreeList virtFL;
    virtFL.size=0;
    FILE* Free_Ls_file= fopen(Partition,"r+b");
    for(int i=0;i<500;i++)
    {
        __uint8_t available = 1;
        virtFL.FLarr[i]=available; // initially all are available 
        virtFL.size+=1;

        // writing these value on the partition
        if(fseek(Free_Ls_file,FL_Start+i, SEEK_SET)!= 0)
        {
            perror("Error while seeking to initializing Free list in Partition\n");
            fclose(Free_Ls_file);
            return -1;
        }
        fwrite(&available,sizeof(available),1,Free_Ls_file);
    }
    fclose(Free_Ls_file);

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
        //printf("%s\n", path->valuestring); 
        cJSON* ti= splitPathToJSON(path->valuestring);
        print_cjson_object(ti);
        
        printf("File creation command (touch) received\n");
    }
    else if(strcmp(command->valuestring, "mkdir") == 0)
    {
        // for making a directory
        printf("Directory creation command (mkdir) received\n");
    }
}










