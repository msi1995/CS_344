#ifndef IMPLEMENTATION_H
#define IMPLEMENTATION_H

#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <regex.h>
#include <signal.h>
#include <stdbool.h>

/***************************************************************************************
Function: CATCH_SIGINT(int);
Purpose: Catchtes the INTERRUPT signal, Ctrl+C and prints a message. When this signal
    handler is attached to a process, it will ignore SIGINT.
Params: int
Return: None
****************************************************************************************/
void CATCH_SIGINT(int);



/***************************************************************************************
Function: CATCH_SIGTSTP();
Purpose: Catches the SIGTSTP signal and toggles the allowBackground boolean. Ctrl+Z 
    effectively functions as a toggle-able boolean allowing or disallowing the running
    of background processes. The "&" background operator is ignored when background is 
    disallowed.
Params: None
Return: None
****************************************************************************************/
void CATCH_SIGTSTP();



/***************************************************************************************
Function: checkForExpansion();
Purpose: This function scans the tokenized command line arguments and checks for the 
    presence of '$$' in any arguments. '$$' will then be expanded to the PID of the 
    shell, and all arguments will have the PID substituted in place of
    '$$'.
Params: char*[], int, struct sigaction, struct sigaction
Return: None
****************************************************************************************/
void checkForExpansion(char*[], int, struct sigaction, struct sigaction);



/***************************************************************************************
Function: getRawCommand();
Purpose: This function uses fgets() to take the raw user input from the command line.
    The function checks for invalid characters at position 0 such as terminators, 
    spaces, or the comment indicator, "#". Strips newlines off of the input if valid.
Params: struct sigaction, struct sigaction
Return: None
****************************************************************************************/
void getRawCommand(struct sigaction, struct sigaction);



/***************************************************************************************
Function: tokenizeCommands();
Purpose: This function uses strtok_r to break the command line input into arguments using
    space as a delimiter, and store them into an array. This allows for the arguments
    and parameters to be easily indexed.
Params: char*, struct sigaction, struct sigaction
Return: None
****************************************************************************************/
void tokenizeCommands(char*, struct sigaction, struct sigaction);



/***************************************************************************************
Function: processCmd();
Purpose: This function handles built-in commands (exit, cd, status) or passes the array
    of commands to executeCmd if the command is not a built-in.
Params: char* [], int, char*, char*, char*, struct sigaction, struct sigaction
Return: None
****************************************************************************************/
void processCmd(char* [], int, char*, char*, char*, struct sigaction, struct sigaction);



/***************************************************************************************
Function: executeCmd();
Purpose: This function is way too large and handles too much. Checks for the presence of
    a background command indicator (&) and sets background value if requested. Calls fork()
    and creates child process. Handles input/output stream assignment with dup2(). 
    Sets signal handlers to child process. Calls the actual exec() family with proper
    arguments. Reports killed processes if a process exited.
Params: char* [], int, char*, char*, char*, struct sigaction, struct sigaction
Return: None
****************************************************************************************/
void executeCmd(char* [], int, char*, char*, char*, struct sigaction, struct sigaction);



/***************************************************************************************
Function: changeDir();
Purpose: Changes to specified directory using chdir(), or changes to HOME directory using
    environment variable if no directory was specified by the user.
Params: char* [], int
Return: None
****************************************************************************************/
void changeDir(char* [], int);


#endif