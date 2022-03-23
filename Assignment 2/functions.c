#include "functions.h"


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

struct movie* readFile(char* filepath, int* numLines){
    FILE* movieFile = fopen(filepath, "r");
    if(movieFile == NULL){
        printf("\nThe file doesn't seem to exist.\n");
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

void printMovie(struct movie* movie){
    printf("%s\n %s\n %s\n %s\n\n", movie->title,
    movie->year,
    movie->languages,
    movie->ratingValue);
}

void printMovieList(struct movie* node){
    while(node != NULL){
        printMovie(node);
        node = node->next;
    }
}

void freeList(struct movie* head)
{
   struct movie* hold;
   while (head != NULL)
    {
       hold = head;
       head = head->next;
       free(hold);
    }
}

char* processEntireDirectory(char* choice, off_t* fileSize){
  // Open the current directory, initialize variables
  DIR* currDir = opendir(".");
  struct dirent *aDir;
  off_t largestSize = 0;
  off_t smallestSize = 1000000;
  char* largestFile;
  char* smallestFile;
  largestFile = (char*) malloc(sizeof(char*) * (strlen(aDir->d_name)+1));
  smallestFile = (char*) malloc(sizeof(char*) * (strlen(aDir->d_name)+1));
  struct stat dirStat;

  // Go through all the entries and get last 4 characters in the name of each entry
  while((aDir = readdir(currDir)) != NULL){
      char* tempString = strdup(aDir->d_name);
      int fileNameLength = strlen(tempString);
      char* lastFour = &tempString[fileNameLength-4]; 
        //Only process files that start with "movies_" and end in ".csv"
      if(strncmp(PREFIX, aDir->d_name, strlen(PREFIX)) == 0 && strcmp(EXTENSION, lastFour) == 0){
        // Get meta-data for the current entry
        stat(aDir->d_name, &dirStat);  
        if(dirStat.st_size > largestSize){
            largestSize = dirStat.st_size;
            strcpy(largestFile, aDir->d_name);
            }
        if(dirStat.st_size <= smallestSize){
            smallestSize = dirStat.st_size;
            strcpy(smallestFile, aDir->d_name);
            }
       
        }
    }

  // Close the directory
  closedir(currDir);

  if(choice[0] == '1'){
    *fileSize = largestSize;
    return largestFile;
  }
  if(choice[0] == '2'){
    *fileSize = smallestSize;
    return smallestFile;
  }

}


char* takeFileName(off_t* fileSize){
    DIR* currDir = opendir(".");
    char* userString;
    bool validEntry = false;
    size_t stringSize = 5;
    userString = (char*) malloc(stringSize);
    char** string_pointer = &userString;
    struct dirent *aDir;
    struct stat dirStat;

    printf("\nEnter the filename with extension: ");
    getline(string_pointer, &stringSize, stdin);
    userString = strtok(userString, "\n");
     while((aDir = readdir(currDir)) != NULL){
      char* tempString = strdup(aDir->d_name);
      if(!strcmp(tempString, userString)){
          stat(aDir->d_name, &dirStat);  
          *fileSize = dirStat.st_size;
          return userString;
        }
     }
    printf("The file '%s' doesn't seem to exist. Try again.\n", userString);
    closedir(currDir);
    takeFileName(fileSize);
}

void populateDirectory(struct movie* node, char* directory, int numLines){
    int i, file_descriptor;
    char year[5];
    char* text = malloc(sizeof(char*) * strlen(node->title));
    struct movie* list_head = node;
    int count = 0;
    bool flag = false;
    char tempString[100];
    int* yearsToCheck;
    yearsToCheck = (int*)malloc(numLines * sizeof(int));
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

    node = list_head;
    count = 0;
    while(count < numLines){
        while(node != NULL){
            if(atoi(node->year) == yearsToCheck[count]){
                strcpy(tempString, directory);
                sprintf(year, "%d", yearsToCheck[count]);
                strcat(tempString,"/");
                strcat(tempString, year);
                strcat(tempString,".txt");
                file_descriptor = open(tempString, O_RDWR | O_CREAT | O_APPEND, 0640);
                if (file_descriptor == -1){
                    printf("open() failed on \"%s\"\n", tempString);
                    perror("Error");
                    exit(1);
                    }
                // Write string to file
                strcpy(text, node->title);
                strcat(text, "\n");
                int howMany = write(file_descriptor, text, strlen(text));
                close(file_descriptor);
            }
            node = node->next;
        }
        count++;
        node = list_head;
    }

    printf("Finished writing to the file.\n");
    free(yearsToCheck);
    freeList(list_head);
    //exit(0);
}
    


void createNewDirectory(struct movie* head, int numLines){
    srand(time(NULL));
    char directory[25] = "lloyddo.movies.";
    int randNum = rand() % (99999 + 1 - 0) + 0;
    char randomExtension[6];
    sprintf(randomExtension, "%d", randNum);
    strcat(directory, randomExtension);
    printf("\nCreated directory with the name %s\n", directory);
    mkdir(directory, 0750);
    populateDirectory(head, directory, numLines);
}

void chooseFileToProcess(){
    struct movie* head;
    char* userString;
    char* fileToProcess;
    int numLines;
    bool validEntry = false;
    size_t stringSize = 5;
    off_t fileSize;
    userString = (char*) malloc(stringSize);
    char** string_pointer = &userString;

    while(validEntry == false){
        printf("\nWhich file do you want to process?\nEnter 1 to pick the largest file\nEnter 2 to pick the smallest file\nEnter 3 to specify a file name\n\nEnter choice from 1 to 3: ");
        getline(string_pointer, &stringSize, stdin);
        switch(atoi(userString)){
            case 1: fileToProcess = processEntireDirectory(userString, &fileSize);
                    validEntry = !validEntry;
                    break;
            case 2: fileToProcess = processEntireDirectory(userString, &fileSize);
                    validEntry = !validEntry;
                    break;
            case 3: fileToProcess = takeFileName(&fileSize);
                    validEntry = !validEntry;
                    break;

            default: printf("Invalid value. Enter a value 1-3.\n\n");
        }
    }
    if(fileToProcess != NULL){
        printf("\nNow processing the chosen file named %s with filesize %lu bytes... \n\n", fileToProcess, fileSize);
        head = readFile(fileToProcess, &numLines);
        createNewDirectory(head, numLines);
    }
    free(userString);
}

void doUserChoice(){
    char* userString;
    bool validEntry = false;
    size_t stringSize = 5;
    userString = (char*) malloc(stringSize);
    char** string_pointer = &userString;

    while(validEntry == false){
        printf("1. Select file to process\n2. Exit the program\nEnter a choice, 1 or 2: ");
        getline(string_pointer, &stringSize, stdin);
        switch(atoi(userString)){
            case 1: chooseFileToProcess();
                    break;
            case 2: printf("\nExiting.\n");
                    exit(0);

            default: printf("Invalid value. Enter 1 or 2.\n\n");
        }
    }
}
