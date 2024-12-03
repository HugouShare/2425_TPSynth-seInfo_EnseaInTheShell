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
    char endMessage[BUFFER_SIZE] = "Exiting the shell...\n";
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
    int running = 1;
    while (running){
        // Reading user input
        if ((command_length =read(STDIN_FILENO, command, BUFFER_SIZE))==-1){
        perror("read"); 
        exit(EXIT_FAILURE);
        }

        ////////////////////////////////////////////////////////////////


        // Executing the user's command
        if (strncmp(command, "fortune", 7) == 0){
            // Creating a subprocess to execute the fortune command
            pid_t pid;
            if ((pid = fork())==-1){
                perror("fork"); 
                exit(EXIT_FAILURE);
            };

            if (pid == 0){
                execl("/usr/games/fortune", "fortune", NULL);
                perror("execl");
                exit(EXIT_FAILURE);
            }
            wait(NULL);
            //////////////////////////////////////////////////////////
            
            if (write(STDOUT_FILENO, commandPrefix, BUFFER_SIZE)==-1){ 
            perror("write"); 
            exit(EXIT_FAILURE);
            }
        }

        else if (strncmp(command, "date", 4) == 0){
            // Creating a subprocess to execute the date command
            pid_t pid;
            if ((pid = fork())==-1){
                perror("fork"); 
                exit(EXIT_FAILURE);
            };

            if (pid == 0){
                execl("/usr/bin/date", "date", NULL);
                perror("execl");
                exit(EXIT_FAILURE);
            }
            wait(NULL);
            //////////////////////////////////////////////////////////
            
            if (write(STDOUT_FILENO, commandPrefix, BUFFER_SIZE)==-1){ 
            perror("write"); 
            exit(EXIT_FAILURE);
            }
        }

        // Checking for the 'exit' command
        if (strncmp(command, "exit", 4) == 0){
            if (write(STDOUT_FILENO, endMessage, BUFFER_SIZE)==-1){ 
            perror("write"); 
            exit(EXIT_FAILURE);
            }
            running = 0;
        }
        
        // Handling an unknown command
        else {
            
            char message[BUFFER_SIZE] = "Unknown command.\n";
            if (write(STDOUT_FILENO, message, BUFFER_SIZE)==-1){ 
            perror("write"); 
            exit(EXIT_FAILURE);
            }
            if (write(STDOUT_FILENO, commandPrefix, BUFFER_SIZE)==-1){ 
            perror("write"); 
            exit(EXIT_FAILURE);
            }
        }

        /////////////////////////////////////////////////////////////
        

    }
    exit(EXIT_SUCCESS);
    
}