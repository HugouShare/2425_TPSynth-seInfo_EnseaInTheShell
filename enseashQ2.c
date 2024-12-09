#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>
#include<sys/types.h>
#include <unistd.h>
#include <string.h>

#define BUFFER_SIZE 128 // Prefered buffer size for I/O operations

int main()
{
    char welcomeMessage[BUFFER_SIZE] = "Welcome in the ENSEA Shell.\nTo quit, type 'exit'.\n";
    char commandPrefix[BUFFER_SIZE] = "\nenseash % ";

    char command[BUFFER_SIZE];
    int command_length;

    
    // Welcome message
    if (write(STDOUT_FILENO, welcomeMessage, BUFFER_SIZE)==-1){ // STDOUT_FILENO is the file descriptor for stdout (1)
        perror("write"); 
        exit(EXIT_FAILURE);
    }

    if (write(STDOUT_FILENO, commandPrefix, BUFFER_SIZE)==-1){ // STDOUT_FILENO is the file descriptor for stdout (1)
        perror("write"); 
        exit(EXIT_FAILURE);
    }
    ///////////////////////////////////////////////////

    // Main loop for reading and executing commands

    while (1){

        // Reading user input
        if ((command_length =read(STDIN_FILENO, command, BUFFER_SIZE))==-1){
            perror("read"); 
            exit(EXIT_FAILURE);
        }

        ////////////////////////////////////////////////////////////////
    
        // executing the user's command
    
        pid_t pid;
        if ((pid = fork())==-1){
            perror("fork"); 
            exit(EXIT_FAILURE);
        };

        if (pid == 0){

            // Removing the "\n" at the end of the command
            command[strcspn(command, "\n")] = 0;

            if (execlp(command, command, (char*)NULL) == -1){
                perror("Unknown command :");
                exit(EXIT_FAILURE);
            }
            exit(EXIT_SUCCESS);
        }

        // Waiting for the subprocess to finish
        wait(NULL);
        //////////////////////////////////////////////////////////
        
        if (write(STDOUT_FILENO, commandPrefix, BUFFER_SIZE)==-1){ 
        perror("write"); 
        exit(EXIT_FAILURE);
        }
        

    }
    exit(EXIT_SUCCESS);
    
}