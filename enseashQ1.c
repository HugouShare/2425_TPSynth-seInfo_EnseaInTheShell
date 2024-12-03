#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>
#include<sys/types.h>
#include <unistd.h>

#define BUFFER_SIZE 4096 // Prefered buffer size for I/O operations

int main()
{
    char message[BUFFER_SIZE] = "Bienvenue dans le Shell ENSEA.\nPour quitter, tapez 'exit'.\nenseash % ";

    if (write(STDOUT_FILENO, message, BUFFER_SIZE)==-1){ // STDOUT_FILENO is the file descriptor for stdout (1)
        perror("write"); 
        exit(EXIT_FAILURE);
    }
    
}