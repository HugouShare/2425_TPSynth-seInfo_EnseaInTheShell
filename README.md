# 2425_TPSyntheseInfo_EnseaInTheShell
CARVALHO FONTES Hugo  
ARCHENAULT Maël  
2 G2 TD2 TP3  


## Question 1
For this question, we avoid to use "printf". Instead, we write on a special file descriptor : "STDOUT_FILENO". It's exactly the same as writing data on a file. However, with this file descriptor, the data is written on the console.

## Question 2

We add a infinite loop to our code. There are three steps executed in this order:
- Reading user input using STDIN_FILENO
- Executing the command, by creating a child process and calling "execlp" in it
- Rewriting the prefix of the shell

After that, we are able to use every one of the functions that are already installed on our PCs (if it does not need any argument)

## Question 3

We want to be able to exit the prompt when:
- writing "exit" (simple string comparison)
- OR pressing "CTRL+D" (we used the fact that this combination of keys returns a string of length 0)

## Question 4

We get the return code of the child process by giving a "status" argument to the "wait" in the parent process.
That way, we can differentiate signals and exits, as well as getting their codes (using WIFEXITED, WEXITSTATUS, WTERMSIG)
In the aftermath, we just have to print them on the console (in the desired format)

## Question 5

To get the time of execution of the child process, we use the clock_gettime function. It takes a special struct as argument to store a time.
We store the time before forking, and after waiting. We compute the difference to find the elapsed time (we can also print it in the prefix)

## Question 6 

This question aims at upgrading our code to execute function with many arguments.
We split the command line according to spaces, and put all segments into a list (argv). We can execute the function by using "execvp" (without forgetting to add a NULL element at the end of the list).

## Question 7

We start by splitting the command line in 2:
- a part with the command
- a part with the attached file

To redirect the result of a function to a file (in or out), in the child process, we link the STDOUT_FILENO (or STDIN_FILENO) with the file descriptor of the destination (or input) file. That way, everything that would normally be printed on the console, will be stored in the file (or used from the file).

## Question 8

Unfortunately, we did not have time to finish this question. However, we managed to setup the two child processes that will execute both functions of the command line. We also set up the pipe to transmit data between them. 

END


