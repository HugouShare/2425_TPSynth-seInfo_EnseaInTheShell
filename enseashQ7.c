#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>
#include<sys/types.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>

#define BUFFER_SIZE 128 // Prefered buffer size for I/O operations

int main()
{
    char welcomeMessage[BUFFER_SIZE] = "Welcome in the ENSEA Shell.\nTo quit, type 'exit'.\n";
    char commandPrefix[BUFFER_SIZE] = "\nenseash ";
    char endMessage[BUFFER_SIZE] = "Exiting the shell...\n";
    char command_line[BUFFER_SIZE];
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
        if ((command_length =read(STDIN_FILENO, command_line, BUFFER_SIZE))==-1){
            perror("read"); 
            exit(EXIT_FAILURE);
        }

        // Checking for the 'exit' command
        if ((strncmp(command_line, "exit", 4) == 0)|(command_length == 0)){
            if (write(STDOUT_FILENO, endMessage, strlen(endMessage))==-1){ 
                perror("write"); 
                exit(EXIT_FAILURE);
            }
            exit(EXIT_SUCCESS);
            
        }

        
        else if (strncmp(command_line, "\n", 1) !=0) {

            // Removing the "\n" at the end of the command
            command_line[strcspn(command_line, "\n")] = 0;

            int redirection = (strchr(command_line, '>') != NULL);

            // if it has one, extract redirection
            char * destination;
            char * command_call = command_line;
            if (redirection){
                command_call = strtok_r(command_line, ">", &destination);
                destination ++; // removing the ' ' at the beginning of the string
                printf("command : %s, destination : %s \n", command_call, destination);
            }
            

            ////////////////////////////////////////////////////////////////

            // Extracting the command and its arguments
            char *command;
            int argc = 0;
            char *argv[11];

            char *token;
            char *rest = command_call;

            token = strtok_r(rest, " ", &rest); // returns a string before the first space in command_line
            command = token;
            argv[0] = token;    // Warning :the name of the function is included into the arglist
            argc ++;

            if (token != NULL) {
                while ((token = strtok_r(NULL, " ", &rest)) != NULL) { // we don't need to respecify the source string, we only put NULL
                    argv[argc] = token;
                    if (token != NULL){
                        argc++;
                    }
                    
                }
            }

            argv[argc]= (char *)NULL;

            ////////////////////////////////////////////////////////////////

            
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

                int fd;

                if (redirection){
                    fd = open(destination, O_WRONLY);
                    dup2(fd, STDOUT_FILENO);
                }

                if (execvp(command, argv) == -1){
                    perror("Unknown command :");
                    exit(EXIT_FAILURE);
                }

                if (redirection){
                    close(fd);
                }
                
            
                exit(EXIT_SUCCESS);
            }

            // Waiting for the subprocess to finish
            wait(&status);
            //////////////////////////////////////////////////////////

            // Getting the time elapsed since the fork call

            clock_gettime(CLOCK_REALTIME, &t_end);
            int elapsed_time = ((double)(t_end.tv_nsec - t_start.tv_nsec))/(1000000);;
            
            //////////////////////////////////////////////////////////

            // Reading what was written on the console
            // char consoleBuffer[BUFFER_SIZE];
            // if (read(STDOUT_FILENO, consoleBuffer, BUFFER_SIZE)==-1){ 
            //     perror("read"); 
            //     exit(EXIT_FAILURE);
            // }

            // if (write(STDOUT_FILENO, consoleBuffer, strlen(consoleBuffer))==-1){ 
            //     perror("write"); 
            //     exit(EXIT_FAILURE);
            // }
            


            // Writing the command line prefix (with return value and elapsed time)
            if (write(STDOUT_FILENO, commandPrefix, strlen(commandPrefix))==-1){ 
                perror("write"); 
                exit(EXIT_FAILURE);
            }
            char returnBuffer[BUFFER_SIZE];
            
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
    
    
}