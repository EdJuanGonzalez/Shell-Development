# Project-1-S24
## Project 1: Shell
Note: Copy of my Temple University Lab Repository.

In this homework, you’ll be building a shell, similar to the tsch shell you use on your CIS Linux2 system. When you open a terminal window on your computer, you are running a shell program. The purpose of a shell is to provide an interface for users to access an operating system’s services, which include file and process management, or execute programs. sh (Bourne shell) is the original Unix shell, and there are many different flavors of shells available. Some other examples include ksh (Korn shell), bash (GNU Bourne again shell), and zsh (Z shell). Shells can be interactive or non-interactive.

For instance, you are using tsch non-interactively when you run a tsch script (series of commnds). tsch is interactive when invoked without arguments, or when the -i flag is explicitly provided. The operating system kernel provides well-documented interfaces for building shells. By building your own, you’ll become more familiar with these interfaces, and you’ll probably learn more about other shells as well.

The overall project is organized in steps to help guide your design/implementation.

## Deliverable 1
help: outputs a text file containing information about the built in functions, what they do, and how to use them
    Note: the txt file in question is in same directory as the program
cd: changes current working directory
pwd: displays the current working directory path
shell_exit: exits shell (ending program)
pseudocode: a text file with the same name has also been provided
help.txt: a file containing the information displayed by help

## Deliverable 2
external: a function that handles redirection and any command that is not built in
    external uses multiple parameters to determine which features are required for each input
    external function also handles situations where user enters path to executable of command
    the array containing the command and its inputs must end with NULL which can be achieved using parse function
    fork() and execv() are used for the external commands
    dup2() along with mutiple parameters are used for redirection

main: main was updated to check for redirection type and to format the array and other parameters properly to send to external
    main determines which redirection type is necessary and external executes said redirection based on formatted parameters (formatting done in main)
NOTE: the parse array was altered to have one more parameter being an integer pointer that stores the length of the array
NOTE: the redirection flag that is used to determine which type of redirection is needed uses the parse function

## Deliverable 3
wait: a function that forces program to wait for all background processes to finish
pipeExternal: the function that handles piping of both two and indefinite amount
    also allows for background processes
implementation of background processes
    edits to external were made to implement this feature
main: main was updated to check if piping was necessary. Also main was updated to ensure that redirection and piping do not work together
NOTE: the parse function is used to determine if piping is required by using the character | as a delimiter

## Disclaimers
### Know Issues
There is an issue when involving working with txt files. For some counting values (numerical values), the numbers are slightly different
For example, if you use a txt file for input for wc, it will say there are 1 less rows and ls -l -a will show 4 less if using txt files
After testing, this is not an issue with the code or redirection, but rather an issue with txt files. 


## Compilation
One must cd into proper folder: shell-development-EdgardoJ-Gonzalez-main
May be different per person based on how file is downloaded

A make file has been created for easy compilation.
Type make then enter followed by ./shell for compilation

## Testing
1. Test help by typing help
2. Test cd by typing cd ..
3. Test pwd and check to see pwd worked
4. Check to see if help still works even if in different directory
5. Test cd again by going back into previous folder: shell-development-EdgardoJ-Gonzalez-main or another valid folder
6. check to see if help still works even if in different directory
7. Type pwd to see if it worked
8. Test for external commands by typing ls
9. Test for arguments for external commands by typing ls -l -a
10. Test for input redirection by typing wc < input.txt
11. Test again using cat < input.txt
12. Test for output redirection by typing ls -l -a > output.txt
13. Type cat < output.txt to check
14. Test for user entering path to executable: /bin/ls
15. Test for path with arguments: /bin/ls -l -a
16. Test for both input and output redirection: wc < input.txt > output.txt
17. Test for both input and output redirection and full path: /usr/bin/wc < input.txt > output.txt
18. Test background process: sleep 10 &
19. Test for wait and check to see background process is working: wait
20. Repeate 18. and type ps to check
21. Test for full path and background process: /bin/ls -l -a > output.txt &
22. Type ps to check
23. Test pipe: ls | wc
23. Test full path & pipe: /bin/ls | wc
24. Test arguments and pipe: /bin/ls -l -a | wc
25. Test multiple pipes: ls | wc | wc
26. Test to ensure redirection and pipes result in invalid input: ls | wc > output.txt
27. Test for no/invalid command (random input): hello
28. Test for cd error check: cd 
29. Test exit
