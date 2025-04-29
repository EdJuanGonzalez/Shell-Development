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
    Function used to handle any input that is not a built in command.
    Uses specifically parsed information gotten from parsing first in main
    Returns 1 or 0 depending on if the command inputed is a valid command

    @param array: an array of strings containing the command, any inputs for the command, and lastly NULL
    @param redirection_flag: an integer value that is 0, 1, 2, or -1 depending on what type of redirection is necessary
        0 = input redirection
        1 = output redirection
        2 = both
        -1 = nothing
    @param outputFile: a string containing the path or name of the file used in output redirection
    @param inputFile: a string that contains the name or path of the file used for input redirection
    @param array_length: an integer value containing the size of the parameter array (length of the array)

    @return: an integer value that determine whether the command inputted was valid or not
        1 = successful 
        0 = failure

    NOTE: ALL GRADED PARTS of Deliverable B can be found in the external function
    NOTE: Based on what redirection is necessary (determined in main function), the parameters of external are set to different values
    GRADING: Implementation of running a program in the background can be found at the bottom of the external function
*/

int external(char **array, int redirection_flag, char *outputFile, char *inputFile, int *array_length, int backgroundFlag) {
    //the return integer value used for if statements in main
    int external_success = -1;

    //fork the process and checks to see if fork was successful
    int pid = fork();
    if (pid < 0) {
        perror("fork");
        exit(1);
    }

    //the child process
    else if (pid == 0) {
        //check for input redirection using parameter redirection_flag
        //open proper file with proper clearances, use dup2 with proper integer value, and close the file once complete
        if (redirection_flag == 0) {
            int file = open(inputFile, O_RDONLY, 0777);
            dup2(file, 0);
            close(file);
        }
        else if (redirection_flag == 1) {
            int file = open(outputFile, O_WRONLY | O_CREAT | O_TRUNC, 0777);
            dup2(file, 1);
            close(file);
        }
        else if (redirection_flag == 2) {
            int fileInput = open(inputFile, O_RDONLY, 0777);
            dup2(fileInput, 0);
            close(fileInput);
            int fileOutput = open(outputFile, O_WRONLY | O_CREAT | O_TRUNC, 0777);
            dup2(fileOutput, 1);
            close(fileOutput);
        }

        //check to see if user entered executable path
        int fail = execv(array[0], array);

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
                strcat(newPath, array[0]);
                execv(newPath, array);
            }
            exit(1);
        }
    /*
    Parent
    1. Check if background processes are needed
        2. wait and get the exit status of the child
        3. Based on exit status return 0 or 1
    */
    } else if (pid > 0) {
        int status;
        if (backgroundFlag == 1) {
            waitpid(pid, &status, WNOHANG);
        }
        else {
            waitpid(pid, &status, 0);
        }
        if (WEXITSTATUS(status) == 0) {
            external_success = 1;
        }
        else {
            external_success = 0;
        }
    }
    //return value
    return external_success;
}