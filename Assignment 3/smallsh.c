/************************************************************************************************
** Program: smallsh
** Author: Doug Lloyd
** Date: 10/25/2021
** Description: This program is an interactive shell written in C. It has built-in support for the 
        commands status, cd, and exit. It has support for other normal shell operations through the
        exec() library.
** Input: Terminal input
** Output: Various text/operations performed by the shell
** References: 
    Some portions of code is pulled almost verbatim from these canvas pages!
    https://canvas.oregonstate.edu/courses/1877314/files/90052823?wrap=1
    https://canvas.oregonstate.edu/courses/1877314/files/90016575?wrap=1
    https://canvas.oregonstate.edu/courses/1877314/pages/exploration-signal-handling-api?module_item_id=21560398
**************************************************************************************************/

#include "implementations.h"


int main(){
    
struct sigaction SIGINT_action = {0};
SIGINT_action.sa_handler = CATCH_SIGINT;
sigfillset(&SIGINT_action.sa_mask);
SIGINT_action.sa_flags = 0;
sigaction(SIGINT, &SIGINT_action, NULL);

struct sigaction SIGTSTP_action = {0};
SIGTSTP_action.sa_handler = CATCH_SIGTSTP;
sigfillset(&SIGTSTP_action.sa_mask);
SIGTSTP_action.sa_flags = 0;
sigaction(SIGTSTP, &SIGTSTP_action, NULL);


//signal handlers crash the entire shell if I don't put this here. I don't know why. Forgive me.
sleep(0);

    while(1){
        getRawCommand(SIGINT_action, SIGTSTP_action);
        fflush(stdout);
    }
}