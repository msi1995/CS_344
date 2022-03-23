#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

int main(int argc, char* argv[]){

    // Seed random
    srand(time(NULL));

    // Make str into int
    int keylength = atoi(argv[1]);
    int random, i;
    char* key;
    char toCopy[2];

    // allocate memory for key
    key = (char*) malloc(keylength * sizeof(char));

    for(i = 0; i < keylength; i++){
        // Random value 0-26, add 65 to have valid ascii values
        random = rand() % 27 + 65;

        // If ascii is 91 (past Z), it should be space (32).
        if(random == 91){
            random = 32;
        }

        toCopy[0] = random;
        toCopy[1] = '\0';

        //Build the key 1 index at a time
        strcat(key, toCopy);
    }

    //add newline at end
    strcat(key, "\n");

    // This is the key
    printf("%s", key);

    return 0;

}