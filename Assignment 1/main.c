// Author: Doug Lloyd
// Class: CS344 - Operating Systems I
// References:
    // - https://replit.com/@cs344/studentsc#student_info1.txt
    // - https://www.tutorialgateway.org/c-program-to-remove-all-occurrences-of-a-character-in-a-string/
    // - https://www.educative.io/edpresso/splitting-a-string-using-strtok-in-c


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>


//create the struct
struct movie{

char* title;
char* year;
char* languages;
char* ratingValue;
struct movie* next;
};


// This function is called from inside the readFile function. uses tokens to break up the lines
// from the text file, and set the variables inside the struct to the proper values.
struct movie* createMovie(char* currentLine){
    char* saveptr;
    struct movie *currentMovie = malloc(sizeof(struct movie));

    char* token = strtok_r(currentLine, ",", &saveptr);
    currentMovie->title = calloc(strlen(token) + 1, sizeof(char));
    strcpy(currentMovie->title, token);

    token = strtok_r(NULL, ",", &saveptr);
    currentMovie->year = calloc(strlen(token) + 1, sizeof(double));
    strcpy(currentMovie->year, token);

    token = strtok_r(NULL, ",", &saveptr);
    currentMovie->languages = calloc(strlen(token) + 1, sizeof(char));
    strcpy(currentMovie->languages, token);

    token = strtok_r(NULL, "\n", &saveptr);
    currentMovie->ratingValue = calloc(strlen(token) + 1, sizeof(char));
    strcpy(currentMovie->ratingValue, token);

    currentMovie->next = NULL;

    return currentMovie;
}


// Open the csv, call getline on it, and then create individual nodes which are returned by the createMovie function.
// Count is to keep track of how many movies were in the file. Finally, insert the node into the linked list.
struct movie* readFile(char* filepath, int* numLines){
    FILE* movieFile = fopen(filepath, "r");
    if(movieFile == NULL){
        printf("\nThe file you supplied doesn't seem to exist. You should start the program by typing './movies <filename>'\n");
        printf("Exiting...\n\n");
        exit(0);
    }
    char* currentLine = NULL;
    size_t length = 0;
    double nread;
    char* token;
    int count = -1;

    struct movie* head = NULL;
    struct movie* tail = NULL;

    while((nread = getline(&currentLine, &length, movieFile)) != -1){
            struct movie* generatedNode = createMovie(currentLine);
            count++;
            if (head == NULL){
                head = generatedNode;
                tail = generatedNode;
            }
            else {
                tail->next = generatedNode;
                tail = generatedNode;
            }
        }
    free(currentLine);
    fclose(movieFile);
    *numLines = count;

    // get rid of the first line that populated the head. I couldn't filter this out nicely in the getline area, so I just
    // dump the head of the linked list since it has garbage in it.
    struct movie* temp = head->next;
    free(head);
    head = temp;
    return head;
}


// print the choices to the user
void displayChoices(){
    printf("1. Show movies released in the specified year\n"
    "2. Show highest rated movie for each year\n" 
    "3. Show the title and year of release of all movies in a specific language\n"
    "4. Exit from the program\n\n");
 }

 void printPretty(){
     printf("=================================================================\n");
 }

//print all information about a single movie
void printMovie(struct movie* movie){
    printf("%s\n %s\n %s\n %s\n\n", movie->title,
    movie->year,
    movie->languages,
    movie->ratingValue);
}

// Print the entire linked list.
void printMovieList(struct movie* node){
    while(node != NULL){
        printMovie(node);
        node = node->next;
    }
}


// Show all of the movies within an entered year.
void showMoviesInYear(struct movie* node){
    char* movieYear;
    int count = 0;
    bool validEntry = false;
    size_t stringSize = 5;
    movieYear = (char*) malloc(stringSize);
    char** string_pointer = &movieYear;

    printf("Search for movies released in the year: ");
    getline(string_pointer, &stringSize, stdin);

    printf("\n");
    printPretty();
    while(node != NULL){
        if(atoi(node->year) == atoi(movieYear)){
            printf("%s\n",node->title);
            count++;
        }

        node = node->next;
    }
    if(count == 0){
        printf("There don't seem to be any movies released in %s",movieYear);
    }
    printPretty();
}


// Print the highest movie in each year that has at least 1 movie.
void showHighestMovieInEachYear(struct movie* node, int numLines){
    int* yearsToCheck;
    yearsToCheck = (int*)malloc(numLines * sizeof(int));
    int count = 0;
    int i;
    bool flag = false;
    struct movie* list_head = node;
    struct movie* hold;

    printPretty();
    while(node != NULL){
        // check if value is already in the array. Don't add it to the yearsToCheck 
        // array if it is already in it. (no duplicates)
        for(i = 0; i < numLines; i++){
            if(yearsToCheck[i] == atoi(node->year)){
                flag = true;
            }
        }
        // add it to the array if it was not already present.
        if(flag == false){
            yearsToCheck[count] = atoi(node->year);
            count++;
            }

        flag = false;
        node = node->next;
    }

    float highestRating;
    node = list_head;
    count = 0;

    // Get the highest rating for each year in the yearsToCheck array.
    while(count < numLines){
        flag = false;
        highestRating = 0.0;
        while(node != NULL){
            if(atoi(node->year) == yearsToCheck[count]){
                if(atof(node->ratingValue) > highestRating){
                    highestRating = atof(node->ratingValue);
                    hold = node;
                    flag = true;
                }
            }
            node = node->next;
        }

        count++;
        node = list_head;

        if(flag == true){

            printf("%s %.1f %s \n", hold->year, atof(hold->ratingValue), hold->title);
        }
    }
    printPretty();
}



// 
void showMovieByLanguage(struct movie* node){
    struct movie* movieListHead = node;
    char* saveptr;
    char* movieLanguage;
    int i, j, result;
    bool flag = false;
    size_t stringSize = 10;
    char** string_pointer = &movieLanguage;
    movieLanguage = (char*) malloc(sizeof(char) * stringSize);

    // user input
    printf("Enter the language for which you want to see movies: ");
    int chars = getline(string_pointer, &stringSize, stdin);

    // strip the newline that getline sticks on the end. Causes strcmp to fail otherwise.
    if(movieLanguage[chars-1] == '\n'){
        movieLanguage[chars-1] = '\0';
        chars--;
    }

    // traverse linked list looking for matches
    printPretty();
    while(node != NULL){
        char* tempString = malloc(sizeof(char*) * strlen(node->languages));
        strcpy(tempString, node->languages);


        // strip the brackets before pulling tokens for comparison.
        int string_length = strlen(tempString);
        for(i=0; i < string_length; i++){
            if(tempString[i] == '[' || tempString[i] == ']'){
                for(j=i; j < string_length; j++){
                    tempString[j] = tempString[j+1];
                }
                string_length--;
                i--;
            }
        }

        // get individual languages within a node one at a time per loop using tokens, splitting
        // on ';' character and then compare them with the searched language
        char* token = strtok_r(tempString, ";", &saveptr);

        while(token != NULL){
            result = strcmp(movieLanguage, token);
            if(result == 0){
                printf("%s %s \n", node->year, node->title);
                flag = true;
                break;
            }
            token = strtok_r(NULL, ";", &saveptr);
        }

        node = node->next;
    }

    if(flag == false){
        printf("\nThere doesn't seem to be any movies in the '%s' language.\n", movieLanguage);
    }
    printPretty();
    free(movieLanguage);
}


// call displayChoices, take the user's choice, and then call the correct function (or terminate program)
void doUserChoice(struct movie* node, int numLines){

    char* userString;
    bool validEntry = false;
    size_t stringSize = 5;
    userString = (char*) malloc(stringSize);

    char** string_pointer = &userString;

    while(validEntry == false){
        displayChoices();
        printf("Enter a choice from 1-4: ");
        getline(string_pointer, &stringSize, stdin);
        switch(atoi(userString)){
            case 1: showMoviesInYear(node);
                    validEntry = true;
                    break;
            case 2: showHighestMovieInEachYear(node, numLines);
                    validEntry = true;
                    break;
            case 3: showMovieByLanguage(node);
                    validEntry = true;
                    break;
            case 4: printf("\nExiting.\n");
                    exit(0);

            default: printf("Invalid value. Enter 1-4.\n");
        }
    }
}



// main function call. Kinda messy, could have made it cleaner.
int main(int argc, char *argv[] ) {
    int numLines;
    if (argc < 2)
    {
        printf("You must provide the name of the file to process\n");
        printf("Example usage: a.out movies_sample_1.csv\n");
        return EXIT_FAILURE;
    }
    
    struct movie* movieListHead = readFile(argv[1], &numLines);

    printf("\nProcessed file %s"
    " and parsed data for %d movies \n\n", argv[1],numLines );

    while(1){
        printf("\n");
        doUserChoice(movieListHead, numLines);
    }


    return 0;
}