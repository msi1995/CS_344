#include "implementations.h"

int exit_status = 0;
bool allowBackground = true;


void CATCH_SIGINT(int signo){
    char* alert = ("Terminated any foreground child processes.\n");
    write(1,alert,44);
}

void CATCH_SIGTSTP(){
    char* alert = "\nEntering foreground-only mode (& is now ignored)\n";
    char* alert2 = "\nExiting foreground-only mode. (& is valid)\n";

    if(allowBackground == true){
        allowBackground = false;
        //write to stdout (1)
        write(1, alert, 50);
        fflush(stdout);
    }
    else{
        allowBackground = true;
        //write to stdout (1)
        write(1, alert2, 44);
        fflush(stdout);
    }
}

void getRawCommand(struct sigaction sigint, struct sigaction sigtstp){
    int i;
    int flag = 1;
    char command[2048];
    printf(": ");
    fgets(command, 2048, stdin);

    //Detect first character is a nonsense or comment character
    if(command[0] == '\n' || command[0] == ' ' || command[0] == '#'){
        return;
    }    

    //Remove newline/sterilize
    while(command[i] != '\0'){
        if(command[i] == '\n'){
            command[i] = '\0';
            break;
        }
        i++;
    }
    tokenizeCommands(command, sigint, sigtstp);
}


void tokenizeCommands(char* command, struct sigaction sigint, struct sigaction sigtstp){
    int i = 0;
    int j = 0;
    char* cmdArray[512];
    char* saveptr = command;
    char* token;

    while((token = strtok_r(saveptr, " ", &saveptr))){
        char* tempString = strdup(token);
        cmdArray[i] = tempString;
        i++;
        }

    int numCommands = i;
    checkForExpansion(cmdArray, numCommands, sigint, sigtstp);
}


void checkForExpansion(char** cmdArray, int numCommands, struct sigaction sigint, struct sigaction sigtstp){
    pid_t pid = getpid();
    char input_fn[256];
    char output_fn[256];
    char shell_pid[10];
    strcpy(input_fn,"");
    strcpy(output_fn,"");
    sprintf(shell_pid, "%d", pid);
    int flag = 0;
    int i,j,k, matchCounter, matchCount;
    char expandedArgument[100];

    for(i = 0; i < numCommands; i++){
        if(strcmp(cmdArray[i], "<") == 0){
            strcpy(input_fn,cmdArray[i+1]);
            // printf("input file is: %s", input_fn);
        }
        if(strcmp(cmdArray[i], ">") == 0){
        strcpy(output_fn,cmdArray[i+1]);
        // printf("output file is: %s", output_fn);
        }
    }
    

    while(flag == 0){
        for(i = 0; i < numCommands; i++){
            for(j = 0; j < strlen(cmdArray[i]); j++){
                if(cmdArray[i][j] == '$'){
                    if(cmdArray[i][j-1] == '$'){
                        cmdArray[i][j-1] = '\0';
                        strcat(cmdArray[i],shell_pid);
                        flag = 1;
                        break;
                    }
                }
            }
        }
        flag = 1;
    }


    processCmd(cmdArray, numCommands, shell_pid, input_fn, output_fn, sigint, sigtstp);
}


// handles built-ins and calls execCmd
void processCmd(char** cmdArray, int numCommands, char* shell_pid, char* input_fn, char* output_fn, struct sigaction sigint, struct sigaction sigtstp){

    char* baseCmd = cmdArray[0];

    if(strcmp(baseCmd, "cd") == 0){
        changeDir(cmdArray, numCommands);
        }

    else if(strcmp(baseCmd, "status") == 0){
        printf("%d\n", exit_status);
        fflush(stdout);
    }

    else if(strcmp(baseCmd, "exit") == 0){
        exit(0);
    }

    else{
        executeCmd(cmdArray, numCommands, shell_pid, input_fn, output_fn, sigint, sigtstp);
    }

}


//cmdArray[0] is NOT cd, status, exit, blank, start w/ space, or comment if this is hit with a valid command.
void executeCmd(char** cmdArray, int numCommands, char* shell_pid, char* input_fn, char* output_fn, struct sigaction sigint, struct sigaction sigtstp){
    pid_t spawnPid = -5;
    int childExitStatus = -5;
    int redirectHit = 0;
    int background_process = 0;
    int i, input, output, devnull, hold;
    cmdArray[numCommands] = NULL;
    char* lastCmd = cmdArray[numCommands-1];

    if(strcmp(lastCmd, "&") == 0){
        cmdArray[numCommands-1] = NULL;
        if(allowBackground == true){
            background_process = 1;
        }
        else{
            background_process = 0;
        }
    }
    
    spawnPid = fork();
    switch(spawnPid){
        case -1: {
            perror("Something is very wrong. Try killing processes.\n");
            fflush(stdout);
            exit(1);
            return;
            }
        case 0: {

            if(background_process == 0){
                sigint.sa_handler = SIG_IGN;
			    sigaction(SIGINT, &sigint, NULL);
            }

            if(background_process == 1){
                sigint.sa_handler = SIG_DFL;
			    sigaction(SIGINT, &sigint, NULL);
            }

            if(strcmp(input_fn,"") == 0 && background_process == 1){
                devnull = open("/dev/null", O_RDONLY);
                hold = dup2(devnull, 0);
                close(devnull);
            }

            if(strcmp(output_fn,"") == 0 && background_process == 1){
                devnull = open("/dev/null", O_WRONLY);
                hold = dup2(devnull, 1);
                close(devnull);
            }

            //handle input stream '<'
            if(strcmp(input_fn,"") != 0){
                redirectHit = 1;
                input = open(input_fn, O_RDONLY);
                // printf("The input file: %s\n", input_fn);
                if(input == -1){
                    perror("Error opening input file\n");
                    exit(1);
                }
                hold = dup2(input, 0);
                if(hold == -1){
                    perror("Unable to assign descriptor to input");
                    exit(2);
                }
                close(input);
            }

            //handle output stream '>'
            if(strcmp(output_fn,"") != 0){
                redirectHit = 1;
                // printf("The output file: %s\n", output_fn);
                output = open(output_fn, O_WRONLY | O_CREAT | O_TRUNC, 0640);
                if(output == -1){
                    perror("Error opening output file\n");
                    exit(1);
                }
                hold = dup2(output, 1);
                if(hold == -1){
                    perror("Unable to assign descriptor to output");
                    exit(2);
                }
                close(output);
            }

            //set null terminator at < or > (if found) so that they are not passed into execvp
            if(redirectHit == 1){
                for(i = 0; i < numCommands; i++){
                    if(strcmp(cmdArray[i],">") == 0 || strcmp(cmdArray[i],"<") == 0){
                        cmdArray[i] = NULL;
                        break;
                    }
                }
            }

            //actual execution of the command here.
            if(execvp(cmdArray[0], cmdArray) < 0){
                perror("Execute failed");
                fflush(stdout);
                exit(1);
            }
            fflush(stdout);
            break;
        }

        default: {
            //run as a background process if requested and allowed
            if(background_process == 1){
                printf("Background PID is %d\n", spawnPid);
                fflush(stdout);
                pid_t actualPid = waitpid(spawnPid, &exit_status, WNOHANG);
            }
            //run as a foreground process if not requested, or disallowed
            else{
            pid_t actualPid = waitpid(spawnPid, &childExitStatus, 0);
            if(WIFEXITED(childExitStatus)){
                exit_status = WEXITSTATUS(childExitStatus);
            }
            }

        }
        //checks for any process that has ended, but only checks when this function is entered... not sure if ok
        while((spawnPid = waitpid(-1, &exit_status, WNOHANG)) > 0){
            if(allowBackground == true){
                printf("Background process child %d has terminated with exit status %d\n", spawnPid, exit_status);
            }
                }
                fflush(stdout);
    }
}

void changeDir(char** cmdArray, int numCommands){
    char currDir[100];
    getcwd(currDir, 100);
    fflush(stdout);
    if(numCommands != 1){
        char* newDir = cmdArray[1];
        chdir(newDir);
        getcwd(currDir, 100);
        // printf("New dir: %s\n", currDir);
        fflush(stdout);
    }
        //changes to home DIR if only 1 command was sent. Command must have been 'cd'.
    else{
        char* env_home = getenv("HOME");
        chdir(env_home);
        getcwd(currDir, 100);
        // printf("New dir: %s\n", currDir);
        fflush(stdout);
        }
}