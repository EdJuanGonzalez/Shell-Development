#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include "helpers.h"
#include "external.h"
#include "pipeExternal.h"
#include <sys/wait.h>

/*
Shell: main program that holds built ins and does proper parsing for other functions
GRADING: the following grading portions can be found in the following functions
-Create core shell for built-ins: shell.c
-help & exit built-in commands: help(char *helpdirectory) & shell_Exit() respectively
-pwd built-in: pwd()
-cd built-in: cd(char *path)
-proper use of supplied parser code: all throughout main()
-implementation of a wait built-in command: waitForBackground()
*/

/*
    Function that displays help information from a text file.
    Uses absolute path to ensure the file is correctly read.
    Gets the initial current working directory path and concatenates it with the name of the help.txt file
    
    @param helpdirectory: a string that holds the initial current working directory
*/

void help(char *helpdirectory) {
    char newPath[1000] = {0};
    strcpy(newPath, helpdirectory);
    strcat(newPath, "/help.txt");
    FILE *myFile = fopen(newPath, "r");
    if (myFile == NULL) {
        perror("Could not find file.");
        exit(1);
    }
    char character;
    while ((character = fgetc(myFile)) != EOF) {
        putchar(character);
    }
    fclose(myFile);
}

/*
    Function that exits the shell
*/

void shell_Exit() {
    exit(0);
}
/*
    Function that displays the full path of the current working directory
    uses getcwd to get said path
*/

void pwd() {
    char path[1000];
    getcwd(path, 1000);
    printf("%s\n", path); 
}

/*
    Function to change the current working directory
    uses chdir to change directory

    @param path: a string that holds the path to the desired directory 
*/
void cd(char *path) {
    if (chdir(path)!= 0) {
        perror("chdir() error()");
    }
}

/*
    Built in wait function that causes program to wait for all background processes to finish
*/
void waitForBackground() {
    int status;
    pid_t pid;
    if ((pid = waitpid(-1, &status, 0)) > 0) {
    }
}
/*
    Mimics shell using while(1)
    gets user input
    checks to see if command exists
    if so call the correct function
    if not displat no such command

    Note: input must be parsed with space and \n before working
    This is implemented
*/

int main() {

    //save original current working directory for help function
    char helpdirectory[1000];
    getcwd(helpdirectory, 1000);

    //loop for shell
    while(1) {
        printf("\n%4s", ">>> "); //prompt

        //create 4 inputs for determining flags for input/output/both redirection, piping, and background processes
        char input[1000] = {0};
        char input2[1000] = {0};
        char input3[1000] = {0};
        char input4[1000] = {0};
        int length = 0; //length of array
        int *array_length = &length; //pointer to length of array
        int builtIn_success_checker = 0; //a flag used to determine if a built in function was requested and worked successfully
        int external_success = 0; //a flag used to determine if an external function was called and properly executed
        fgets(input, sizeof(input), stdin); //get user input
        //copy input to other 3 for later use
        strcpy(input2, input);
        strcpy(input3, input);
        strcpy(input4, input);
        char **array = parse(input, " |&<>\n", array_length); //parse the input for built in functions
        
        //error check
        if (array == NULL) {
            perror("Error getting input");
            exit(1);
        }//end if (array == NULL)
    
        //check if input is a built in command
        //if successful: builtIn_success_checker = 1 to show that a built in function was asked for
        if (strcmp(array[0], "exit") == 0) {
            shell_Exit();
        }//end if (strcmp(array[0], "exit")
        else if (strcmp(array[0], "pwd") == 0) {
            pwd();
            builtIn_success_checker = 1;
        }//end else if (strcmp(array[0], "pwd") == 0)
        else if (strcmp(array[0], "cd") == 0) {
            cd(array[1]);
            builtIn_success_checker = 1;
        }//end else if (strcmp(array[0], "cd") == 0)
        else if (strcmp(array[0], "help") == 0) {
            help(helpdirectory);
            builtIn_success_checker = 1;
        }//end else if (strcmp(array[0], "help") == 0)
        else if (strcmp(array[0], "wait") == 0) {
            waitForBackground();
            builtIn_success_checker = 1;
        }//end else if (strcmp(array[0], "wait") == 0)
        
        /*
            If command was not a built in check for external command and if one of the following are required
                redirection, piping, background processes
        */
        if (!builtIn_success_checker) {
            //flags and necessary variables
            int n2 = 0;
            int *array_length2 = &n2;
            int redirection_flag = -1;
            int backgroundFlag = -1;
            int pipe_flag = -1;
            int invalidInput = 0;

            /*
                Checking for redirection using parse function and <> as delimiters
                redirection flag possible values
                0: input redirection
                1: output redirection
                2: both
                -1: none 
            */
            char **outputR = parse(input2, ">\n", array_length2);
            if (*array_length2 > 1) {
                redirection_flag = 1;
            }//end if (*array_length2 > 1)

            int n3 = 0;
            int *array_length3 = &n3;
            char **inputR = parse(outputR[0], "<\n", array_length3);
            if (*array_length3 > 1 && redirection_flag == 1) {
                redirection_flag = 2;
            }//end if (*array_length3 > 1 && redirection_flag == 1)

            else if (*array_length3 > 1 && redirection_flag != 1) {
                redirection_flag = 0;
            }//end else if (*array_length3 > 1 && redirection_flag != 1)

            /*
                Check for background processes needed
                if yes: backgroundFlag = 1
                if no: backgroundFlag = -1
            */
            int bn5 = 0;
            int *barray_length = &bn5;
            char **backgroundArray = parse(input3, " \n", barray_length);
            if (strcmp(backgroundArray[*barray_length-1], "&") == 0) {
                backgroundFlag = 1;
            }//end if (strcmp(backgroundArray[*barray_length-1], "&") == 0) 

            /*
                Check for piping
                if yes: pipe_flag = 1
                if no: pipe_flag = -1
            */
            int pn = 0;
            int *parray_length = &pn;
            char **pipeArray = parse(input4, "|\n", parray_length);
            if (*parray_length > 1) {
                pipe_flag = 1;
            }//end if (*parray_length > 1)

            /*
                Check for valid input
                redirection and piping together are not valid
            */
            if (redirection_flag != -1 && pipe_flag == 1) {
                invalidInput = 1;
            }

            /*
                Checks if input is valid
                If so, uses flags to determine what function to call with what parameters
                parameters are also properly found/created based on flags
            */
            if (invalidInput == 0) {
                int n4 = 0;
                int *array_length4 = &n4;

                /*
                    Only input redirection
                    creates correctly formatted array to send to external function
                    parses for the inputFile while removing any unnecessary characters
                    calls external
                    frees arrays once done
                */
                if (redirection_flag == 0) {
                    char **inputFile = parse(inputR[1], " &|\n", array_length4);
                    char **external_array = parse(inputR[0], " &|\n", array_length4);
                    external_success = external(external_array, redirection_flag, NULL, inputFile[0], array_length4, backgroundFlag);
                    free(inputFile);
                    free(external_array);
                }//end if (redirection_flag == 0)

                /*
                    Only output redirection
                    creates correctly formatted array to send to external function
                    parses for the outputFile while removing any unnecessary characters
                    calls external
                    frees arrays once done
                */
                else if (redirection_flag == 1) {
                    char **outputFile = parse(outputR[1], " &|\n", array_length4);
                    char **external_array = parse(outputR[0], " &|\n", array_length4);
                    external_success = external(external_array, redirection_flag, outputFile[0], NULL, array_length4, backgroundFlag);
                    free(outputFile);
                    free(external_array);
                }//end else if (redirection_flag == 1)

                /*
                    Both redirections
                    creates correctly formatted array to send to external function
                    parses for the inputFile while removing any unnecessary characters
                    parses for the outputFile while removing any unnecessary characters
                    calls external
                    frees arrays once done
                */
                else if (redirection_flag == 2) {
                    char **inputFile = parse(inputR[1], " &|\n", array_length4);
                    char **outputFile = parse(outputR[1], " &|\n", array_length4);
                    char **external_array = parse(outputR[0], " &|\n", array_length4);
                    external_success = external(external_array, redirection_flag, outputFile[0], inputFile[0], array_length4, backgroundFlag);
                    free(inputFile);
                    free(outputFile);
                    free(external_array);
                }//end else if (redirection_flag == 2)
                
                /*
                    If all redirections fail, check to see if piping is needed
                    If so calculates the number of pipes needed and calls pipeExternal instead of regular external
                
                */
                else if (pipe_flag == 1) {
                    int numOfPipes = *parray_length - 1;
                    external_success = pipeExternal(pipeArray, numOfPipes, backgroundFlag);
                    
                }//end  else if (pipe_flag == 1)

                //If all fails call external and check to see if a valid external command was inputted
                else {
                    external_success = external(array, redirection_flag, NULL, NULL, array_length, backgroundFlag);
                }//end else
            
                //free the last arrays that are no longer used for parsing
                free(outputR);
                free(inputR);
                free(pipeArray);
            }//end if (invalidInput == 0)
        }//if (!builtIn_success_checker)
        
        //prints if user input was not a built in command or a valid external command
        if (!builtIn_success_checker && !external_success) {
            puts("Invalid input");
        }
        free(array);//free the last array
    }//end while(1)
}//end main()