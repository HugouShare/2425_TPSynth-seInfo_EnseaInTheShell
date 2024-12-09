#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>
#include<sys/types.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

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

    while (1){

        
        // Reading user input
        if ((command_length =read(STDIN_FILENO, command, BUFFER_SIZE))==-1){
            perror("read"); 
            exit(EXIT_FAILURE);
        }

        // Checking for the 'exit' command
        if ((strncmp(command, "exit", 4) == 0)|(command_length == 0)){
            if (write(STDOUT_FILENO, endMessage, BUFFER_SIZE)==-1){ 
                perror("write"); 
                exit(EXIT_FAILURE);
            }

            exit(EXIT_SUCCESS);
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

            // Initializing the timestamps
            struct timespec t_start;
            struct timespec t_end;
            clock_gettime(CLOCK_REALTIME, &t_start);

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

            // Getting the time elapsed since the fork call

            clock_gettime(CLOCK_REALTIME, &t_end);
            int elapsed_time = ((double)(t_end.tv_nsec - t_start.tv_nsec))/(1000000);;
            
            //////////////////////////////////////////////////////////


            // Writing the command line prefix (with return value and elapsed time)
            if (write(STDOUT_FILENO, commandPrefix, BUFFER_SIZE)==-1){ 
            perror("write"); 
            exit(EXIT_FAILURE);
            }
            char returnBuffer[15];
            
            if (WIFEXITED(status)){
                sprintf(returnBuffer,"[exit:%d|%dms]", WEXITSTATUS(status), elapsed_time);
            }

            else {
                sprintf(returnBuffer,"[sign:%d|%dms]", WTERMSIG(status), elapsed_time);
            }
            
            
            if (write(STDOUT_FILENO, returnBuffer, strlen(returnBuffer))==-1){ 
                perror("write"); 
                exit(EXIT_FAILURE);
            }

            if (write(STDOUT_FILENO, " % ", 3)==-1){ 
            perror("write"); 
            exit(EXIT_FAILURE);
            }

            //////////////////////////////////////////////////////////

        }
        
    }
    exit(EXIT_SUCCESS);
    
}