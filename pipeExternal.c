#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include "helpers.h"
#include <fcntl.h>
#include <sys/wait.h>

/*
    Function that handles piping of commands
    Uses a parsed array from parse function using | character as delimeter
    Returns 1 if successful and 2 if failure

    @param array: an array of char pointers (strings)
        note: array is created using parse function and using the | character as delimiter
    @param numOfPipes: an integer that contains the number of pipes required
        note: calculated using the parse function by taking the length of array and subtrating one
    @param backgroundFlag: an integer used to determine if processes need to be run in background or not
        background flag = 1 if yes and 0 for no
    
    @return: an integer value that determine whether the command inputted was valid or not
        1 = successful 
        0 = failure

    GRADING: all of the following can be found in this function
    Implementation of pipe [|]
    implementation of indefinite pipe commands on one commandline, 
    and Implementation of running a program in the background 
        note for background: external also has implementation of background 

*/

int pipeExternal(char **array, int numOfPipes, int backgroundFlag) {
    int pipe_external_success = -1; //return value
    //create a 2D array for the pipes and fork for background processes
    int pipes[numOfPipes][2];
    int pid = fork();
    
    if (pid < 0) {
        perror("fork");
        exit(1);
    }//end if (pid < 0)

    //child
    if (pid == 0) {
        /*
            Loop for creating and connecting pipes
            1. check to see if a pipe needs to be created and if so create it
            2. Fork for command
            3. check which pipe we are in
                Three types:
                    1. First pipe (i==0)
                    2. A middle pipe (i < numOfPipes)
                    3. Last pipe
            4. Based on which pipe, dup2 and close
            5. take param array and parse it using " " character
            6. check to see if user entered path to executable
            7. If that failed use getenv and parse to loop through and check if command is valid
        */
        for (int i = 0; i < numOfPipes+1; i++) {
            if(i != numOfPipes) {
                pipe(pipes[i]);
            }//end if(i != numOfPipes)

            //fork for each command
            int pid2 = fork();

            //grand child
            if (pid2 == 0) {
                if (i == 0) {
                    dup2(pipes[i][1], 1);
                    close(pipes[i][1]);
                }//end if (i == 0)
                else if (i < numOfPipes) {
                    dup2(pipes[i-1][0], 0);
                    dup2(pipes[i][1], 1);
                    close(pipes[i-1][0]);
                    close(pipes[i][1]);
                }//end else if (i < numOfPipes)
                else {
                    dup2(pipes[i-1][0], 0);
                    close(pipes[i-1][0]); 
                }//end else

                //prepare array for execv
                int length = 0;
                int *arrayLength = &length;
                char **myArray = parse(array[i], " \n", arrayLength);

                //check to see if user entered executable path
                int fail = execv(myArray[0], myArray);
                /* 
                if user did not, get string of executable file paths divided by a : using getenv
                parse using : and loop through the array
                attach the command to the end of the path
                see if it exists 
                loop again if not
                */
                if (fail == -1) {
                    int n = 0;
                    int *array_length2 = &n;
                    char *path = getenv("PATH");
                    char **array2 = parse(path, ":", array_length2);
                    for (int i = 0; i < *array_length2; ++i) {
                        char newPath[1000] = {0};
                        strcpy(newPath, array2[i]);
                        strcat(newPath, "/");
                        strcat(newPath, myArray[0]);
                        execv(newPath, myArray);
                    }//end for (int i = 0; i < *array_length2; ++i)
                    exit(1);
                }//end if (fail == -1)
            }//end if (pid2 == 0)
            
            //child 
            //close pipes properly
            else if (pid2 > 0) {
                if (i == 0) {
                    close(pipes[i][1]);
                }
                else if (i < numOfPipes) {
                    close(pipes[i-1][0]);
                    close(pipes[i][1]);
                }
                else {
                    close(pipes[i-1][0]); 
                }
                wait(NULL);
            }
        }
        exit(0); //exit child
    }//end if (pid == 0)

    /*
    Parent
        1. Check if background processes are needed
        2. wait and get the exit status of the child
        3. Based on exit status return 0 or 1
    */
    else if (pid > 0) {
        int status;
        if (backgroundFlag == 1) {
            waitpid(pid, &status, WNOHANG);
        }
        else {
            waitpid(pid, &status, 0);
        }

        if (WEXITSTATUS(status) == 0) {
            pipe_external_success = 1;
        }
        else {
            pipe_external_success = 0;
        }
    }
    return pipe_external_success;
}