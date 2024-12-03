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
    char commandPrefix[BUFFER_SIZE] = "\nenseash ";
    char endMessage[BUFFER_SIZE] = "Exiting the shell...\n";
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

    if (write(STDOUT_FILENO, " % ", 3)==-1){ 
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

        // Checking for the 'exit' command
        if (strncmp(command, "exit", 4) == 0){
            if (write(STDOUT_FILENO, endMessage, BUFFER_SIZE)==-1){ 
                perror("write"); 
                exit(EXIT_FAILURE);
            }

            running = 0;
        }

        ////////////////////////////////////////////////////////////////
        else {
            // executing the user's command
    
            pid_t pid;
            int status;
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
            }

            // Waiting for the subprocess to finish
            wait(&status);

            
            //////////////////////////////////////////////////////////
            
            if (write(STDOUT_FILENO, commandPrefix, BUFFER_SIZE)==-1){ 
            perror("write"); 
            exit(EXIT_FAILURE);
            }
            char returnBuffer[12];
            
            if (WIFEXITED(status)){
                sprintf(returnBuffer,"[exit:%d]", WEXITSTATUS(status));
            }

            else {
                sprintf(returnBuffer,"[sign:%d]", WTERMSIG(status));
            }
            
            
            if (write(STDOUT_FILENO, returnBuffer, 12)==-1){ 
                perror("write"); 
                exit(EXIT_FAILURE);
            }

            if (write(STDOUT_FILENO, " % ", 3)==-1){ 
            perror("write"); 
            exit(EXIT_FAILURE);
            }

        }
        
    }
    exit(EXIT_SUCCESS);
    
}