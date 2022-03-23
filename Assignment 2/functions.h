#include <fcntl.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>
#include <stdbool.h>

#define PREFIX "movies_"
#define EXTENSION ".csv"

// Create the struct
struct movie{
char* title;
char* year;
char* languages;
char* ratingValue;
struct movie* next;
};


/***************************************************************************************
Function: doUserChoice();
Purpose: Prompts user for first user input (1 to process a file, or 2 to quit program).
    Calls chooseFileToProcess() or exits the program.
Params: None
Return: None
****************************************************************************************/
void doUserChoice();



/***************************************************************************************
Function: chooseFileToProcess();
Purpose: Prompts user to input their desired file to process: smallest, largest, or 
    by providing a filename. Calls processEntireDirectory() or takeFileName() in the case
    of option 3. Verifies that the file exists when it is returned from processEntireDirectory()
    or takeFileName(), and then calls readFile on that file. Finally, calls createNewDirectory().
Params: None
Return: 
****************************************************************************************/
void chooseFileToProcess();



/***************************************************************************************
Function: takeFileName(off_t*);
Purpose: Take a specific CSV filename from the user, and searches the directory to check
    if the file exists. Re-prompts if not present. Returns the filename for later use
    in readFile if valid.
Params: off_t (filesize for tracking size in bytes)
Return: string (filename)
****************************************************************************************/
char* takeFileName(off_t*);



/***************************************************************************************
Function: processEntireDirectory(char*, off_t*);
Purpose: Traverses the directory and identifies either the smallest or largest file
    depending on the user's choice. Returns a string containing the filename and prints
    information about it.
Params: char*, off_t (The user's choice in regards to file processing, and the filesize)
Return: string (filename of largest or smallest file)
****************************************************************************************/
char* processEntireDirectory(char*, off_t*);



/***************************************************************************************
Function: readFile(char*, int*);
Purpose: Opens the csv and calls getline on individual lines. Each line is passed to
    createMovie(). Returns a pointer to a movie struct upon completion (the head of the 
    linked list).
Params: char*, int* (the filename, and numLines var by address)
Return: struct*
****************************************************************************************/
struct movie* readFile(char*, int* );



/***************************************************************************************
Function: createMovie(char*);
Purpose: Uses strtok_r to break up lines into tokens and insert the proper values into
    member variables of the movies struct. Essentially this function populates each node
    in the linked list.
Params: char* (currentLine pointer)
Return: struct*
****************************************************************************************/
struct movie* createMovie(char*);



/***************************************************************************************
Function: createNewDirectory(struct movie*);
Purpose: Generates random number and concatenates it to directory name, creates directory
    with proper permissions. Calls populateDirectory().
Params: struct*, int
Return: None
****************************************************************************************/
void createNewDirectory(struct movie*, int);



/***************************************************************************************
Function: populateDirectory(struct movie*, char*, int);
Purpose: Generates an array from the linked list which contains all years that a movie 
    is present in with no duplicates. Navigates through the linked list and creates a 
    txt file for each year in the array, populating the text file with any movies whose
    year member variable matches the year currently being searched for. Finishes when
    all indices in the year array have been searched against.
Params: struct*, char*, int ()
Return: None
****************************************************************************************/
void populateDirectory(struct movie*, char*, int);



/***************************************************************************************
Function: printMovie(struct movie*);
Purpose: Takes in a movie struct pointer (linked list node) and prints information
    about the node/movie.
Params: struct*
Return: None
****************************************************************************************/
void printMovie(struct movie*);



/***************************************************************************************
Function: printMovieList(struct movie*);
Purpose: Takes in a movie struct pointer (linked list head) and prints information
    about the entire linked list (all nodes).
Params: struct*
Return: None
****************************************************************************************/
void printMovieList(struct movie*);



/***************************************************************************************
Function: freeList(struct movie*);
Purpose: Traverses the linked list and frees each node.
Params: struct*
Return: None
****************************************************************************************/
void freeList(struct movie*);

