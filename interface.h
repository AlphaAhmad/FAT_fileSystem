#include "utility.h"

#define MAX_CMD_LEN 100

void clear_screen() {
    // Clear the screen based on the operating system
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void print_help() {
    printf("Available commands:\n");
    printf("  clear   - Clear the screen\n");
    printf("  help    - Show available commands\n");
    printf("  exit    - Exit the interface\n");
    printf("  echo    - Repeat the input b else ack\n");
}

char* interface() {
    char command[MAX_CMD_LEN];
    char input[MAX_CMD_LEN];

    // printf("Welcome to the C Terminal Interface!\n");
    // printf("Type 'help' for a list of commands.\n\n");

    while (1) {
        printf("C-Terminal> "); // Command prompt
        fgets(command, MAX_CMD_LEN, stdin); // Read user input
        command[strcspn(command, "\n")] = 0; // Remove trailing newline

        // if (strcmp(command, "exit") == 0) {
        //     printf("Exiting... Goodbye!\n");
        //     return NULL;}
        if (strcmp(command, "clear") == 0) {
            clear_screen();
        } else if (strcmp(command, "help") == 0) {
            print_help();
        } else if (strncmp(command, "echo", 4) == 0) {
            // Echo command logic
            if (strlen(command) > 5) {
                strncpy(input, command + 5, MAX_CMD_LEN - 5);
                printf("%s\n", input);
            } else {
                printf("Nothing to echo.\n");
            }
        } else {
            // Allocate memory for unrecognized command and return it
            char* unrecognized_command = (char*)malloc(strlen(command) + 1);
            if (unrecognized_command == NULL) {
                perror("Failed to allocate memory");
                return NULL;
            }
            strcpy(unrecognized_command, command);
            return unrecognized_command;
        }
    }
}
