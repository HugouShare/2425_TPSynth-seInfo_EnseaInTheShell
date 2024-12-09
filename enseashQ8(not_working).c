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

        int redirection_in = 0;
        int redirection_out = 0;
        int redirection_pipe = 0;

        
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

            redirection_out = (strchr(command_line, '>') != NULL);
            redirection_in = (strchr(command_line, '<') != NULL);
            redirection_pipe = (strchr(command_line, '|') != NULL);
            

            // if it has one, extract redirection
            char * attached_file;
            char * command_call1 = command_line;
            char * command_call2 = command_call1;

            if (redirection_out){
                command_call1 = strtok_r(command_line, ">", &attached_file);
                attached_file ++; // removing the ' ' at the beginning of the string
            }
            if (redirection_in){
                command_call1 = strtok_r(command_line, "<", &attached_file);
                attached_file ++; // removing the ' ' at the beginning of the string
            }
            if (redirection_pipe){
                command_call1 = strtok_r(command_line, "|", &command_call2);
                command_call2 ++; // removing the ' ' at the beginning of the string
            }

            ////////////////////////////////////////////////////////////////

            // Extracting each command and its arguments
            char *command1;
            int argc1 = 0;
            char *argv1[11];

            char *token;
            char *rest = command_call1;

            token = strtok_r(rest, " ", &rest); // returns a string before the first space in command_line
            command1 = token;
            argv1[0] = token;    // Warning :the name of the function is included into the arglist
            argc1 ++;

            if (token != NULL) {
                while ((token = strtok_r(NULL, " ", &rest)) != NULL) { // we don't need to respecify the source string, we only put NULL
                    argv1[argc1] = token;
                    if (token != NULL){
                        argc1++;
                    }
                    
                }
            }

            argv1[argc1]= (char *)NULL;

            char *command2;
            int argc2 = 0;
            char *argv2[11];
            rest = command_call2;

            token = strtok_r(rest, " ", &rest); // returns a string before the first space in command_line
            command2 = token;
            argv2[0] = token;    // Warning :the name of the function is included into the arglist
            argc2 ++;

            if (token != NULL) {
                while ((token = strtok_r(NULL, " ", &rest)) != NULL) { // we don't need to respecify the source string, we only put NULL
                    argv2[argc2] = token;
                    if (token != NULL){
                        argc2++;
                    }
                    
                }
            }

            argv2[argc2]= (char *)NULL;

            ////////////////////////////////////////////////////////////////

            
            // executing the user's command
    
            pid_t pid;
            int status;

            // Initializing the pipe for interprocess communication
            int pipefd[2];
            pipe(pipefd);

            // Initializing the timestamps
            struct timespec t_start;
            struct timespec t_end;
            clock_gettime(CLOCK_REALTIME, &t_start);


            if ((pid = fork())==-1){
                perror("fork"); 
                exit(EXIT_FAILURE);
            };

            if (pid == 0){

                int fd;
                
                if (redirection_out) {
                    if ((fd = open(attached_file, O_CREAT|O_WRONLY, S_IRUSR|S_IWUSR|S_IXUSR|S_IRGRP|S_IWGRP|S_IXGRP))==-1){
                        perror("open");
                        exit(EXIT_FAILURE);
                    }
                    dup2(fd, STDOUT_FILENO);
                }
                if (redirection_in) {
                    if ((fd = open(attached_file, O_RDONLY, S_IRUSR|S_IWUSR))==-1){
                        perror("open"); 
                        exit(EXIT_FAILURE);
                    }
                    dup2(fd, STDIN_FILENO);
                }
                if (redirection_pipe) {
                    dup2(pipefd[1], STDOUT_FILENO);
                    close(pipefd[0]);
                }
                if (execvp(command1, argv1) == -1){
                    perror("Unknown command :");
                    exit(EXIT_FAILURE);
                }

                if (redirection_in|redirection_out) {
                    close(fd);
                }
                
            
                exit(EXIT_SUCCESS);
            }

            if (redirection_pipe) {
                printf("test");
                if ((pid = fork())==-1){
                perror("fork"); 
                exit(EXIT_FAILURE);
                };

                if (pid == 0){

                    dup2(STDIN_FILENO, pipefd[0]);
                    close(pipefd[1]);
                    
                    if (execvp(command2, argv2) == -1){
                        perror("Unknown command :");
                        exit(EXIT_FAILURE);
                    }
                    
                }
            }

            
            // WaitSTDOUT_FILENOing for the subprocess to finish
            wait(&status);
            //////////////////////////////////////////////////////////

            // Getting the time elapsed since the fork call

            clock_gettime(CLOCK_REALTIME, &t_end);
            int elapsed_time = ((double)(t_end.tv_nsec - t_start.tv_nsec))/(1000000);
            
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