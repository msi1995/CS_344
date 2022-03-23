#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define LINES 50
#define LENGTH 1001
int numLines = 0;
int stopFlag = 0;
int indexOffset = 0;
char input[1001];
char line_temp_input[1001];
char line_temp_buf1[1001];
char line_temp_buf2[1001];
char line_temp_buf3[50000];
int counter = 0;
int idx = 0;

//BUFFER 1. Shared between input and line separator.
char buffer1[LINES][LENGTH];
int items_in_buf1 = 0;
//Index where the input thread will put line in buffer
int producer_idx_1 = 0;
//Index where the line separator thread will pick up line from buffer
int consumer_idx_1 = 0;
//mutex and condition initialization
pthread_mutex_t mutex_1 = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t full_1 = PTHREAD_COND_INITIALIZER;


//BUFFER 2. Shared between line separator and plus modifier.
char buffer2[LINES][LENGTH];
int items_in_buf2 = 0;
//Index where the line separator will put line in buffer
int producer_idx_2 = 0;
//Index where the plus modifier will pick up line from buffer
int consumer_idx_2 = 0;
//mutex and condition initialization
pthread_mutex_t mutex_2 = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t full_2 = PTHREAD_COND_INITIALIZER;


//BUFFER 3. Shared between plus modifier and output writer.
char buffer3[LINES][LENGTH];

char outputBuffer[50000];

int items_in_buf3 = 0;
//Index where the plus modifier will put line in buffer
int producer_idx_3 = 0;
//Index where the output writer will pick up line from buffer
int consumer_idx_3 = 0;
//mutex and condition initialization
pthread_mutex_t mutex_3 = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t full_3 = PTHREAD_COND_INITIALIZER;



void put_buffer_1(char* line){
    pthread_mutex_lock(&mutex_1);
    strcpy(buffer1[producer_idx_1],line);
    producer_idx_1++;
    items_in_buf1++;
    //signal to the consumer (line separator) that the buffer isn't empty
    pthread_cond_signal(&full_1);
    //unlock mutex
    pthread_mutex_unlock(&mutex_1);
}

void put_buffer_2(char* line){
    pthread_mutex_lock(&mutex_2);
    strcpy(buffer2[producer_idx_2],line);
    producer_idx_2++;
    items_in_buf2++;
    //signal to the consumer (line separator) that the buffer isn't empty
    pthread_cond_signal(&full_2);
    //unlock mutex
    pthread_mutex_unlock(&mutex_2);
}

void put_buffer_3(char* line){
    pthread_mutex_lock(&mutex_3);
    strcpy(buffer3[producer_idx_3],line);
    producer_idx_3++;
    items_in_buf3++;
    //signal to the consumer (line separator) that the buffer isn't empty
    pthread_cond_signal(&full_3);
    //unlock mutex
    pthread_mutex_unlock(&mutex_3);
}

char* get_buffer_1_item(){
    //lock mutex
    pthread_mutex_lock(&mutex_1);
    //no items in buffer. Wait for producer to signal that there are items.
    while (items_in_buf1 == 0){
        pthread_cond_wait(&full_1, &mutex_1);
    }
    strcpy(line_temp_buf1, buffer1[consumer_idx_1]);
    consumer_idx_1++;
    items_in_buf1--;
    //unlock mutex
    pthread_mutex_unlock(&mutex_1);
    //return item
    return line_temp_buf1;
}

char* get_buffer_2_item(){
    //lock mutex
    char* temp = malloc(50000);
    pthread_mutex_lock(&mutex_2);
    //no items in buffer. Wait for producer to signal that there are items.
    while (items_in_buf2 == 0){
        pthread_cond_wait(&full_2, &mutex_2);
    }
    strcpy(temp, buffer2[consumer_idx_2]);
    consumer_idx_2++;
    items_in_buf2--;
    //unlock mutex
    pthread_mutex_unlock(&mutex_2);
    //return item
    return temp;
}


char* get_buffer_3_item(){
    //lock mutex
    pthread_mutex_lock(&mutex_3);
    //no items in buffer. Wait for producer to signal that there are items.
    while (items_in_buf3 == 0 ){
        pthread_cond_wait(&full_3, &mutex_3);
    }
    strcpy(line_temp_buf3, buffer3[consumer_idx_3]);
    consumer_idx_3++;
    items_in_buf3--;
    //unlock mutex
    pthread_mutex_unlock(&mutex_3);
    //return item
    return line_temp_buf3;
}

// get user input
char* get_input(){
    size_t size = 1001;
    char* input;
    input = (char*) malloc (size);
    char** input_ptr = &input;
    getline(input_ptr, &size, stdin);
    // fgets(input, LENGTH, stdin);
    // printf("DEBUG-- input: %s", input);
    return input;
}


//inputGetter thread, Thread #1
void* inputGetter(void* args){
    int i;
    for(i = 0; i < LINES; i++){
        char* line_temp_input = get_input();

        if(!strcmp(line_temp_input, "STOP\n")){
            put_buffer_1(line_temp_input);
            return NULL;
        }

        put_buffer_1(line_temp_input);
    }
    return NULL;
}

//lineSeparator thread, Thread #2
void* lineSeparator(void* args){
    int i, j;
    char* line;
    for(i = 0; i < LINES; i++){
        line = get_buffer_1_item();

        if(!strcmp(line, "STOP\n")){
            put_buffer_2(line);
            return NULL;
        }

        for(j = 0; j < strlen(line); j++){
            if(line[j] == '\n'){
                line[j] = ' ';
            }
        }
        put_buffer_2(line);
    }
    return NULL;
}

//plusSignModifier thread, Thread #3
void* plusSignModifier(void* args){
    int i, j;
    char* line;
    for(i = 0; i < LINES; i++){
        line = get_buffer_2_item();

        if(!strcmp(line, "STOP\n")){
            put_buffer_3(line);
            return NULL;
        }

        for(j = 0; j < strlen(line); j++){
            if(line[j] == '\n'){
                line[j] = ' ';
            }
        }
        for(j = 0; j < strlen(line); j++){
            if(line[j] == '+'){
                if(line[j+1] == '+'){
                    line[j] = '^';
                    memmove(line+j+1, line+j+2, strlen(line)-j+1);
                }
            }
        }
        put_buffer_3(line);
    } 
    return NULL;
}

//outputWriter thread, Thread #4
void* outputWriter(void* args){
    char output_line[82];
    int i,j;
    char* temp;

    for (i = 0; i < LINES; i++)
    {
        temp = get_buffer_3_item();
        counter += strlen(temp);

        if(!strcmp(temp, "STOP\n"))
            return NULL;
        
        strcat(outputBuffer, temp);
        if(counter > 79)
            do
            {
                strncpy(output_line, outputBuffer+idx, 80);
                output_line[80] = '\n';
                printf("%s", output_line);
                idx += 80;
                counter -= 80;

            }while(counter > 79);
    
    }


    return NULL;
}

int main(){
    pthread_t inputGetter_t, lineSeparator_t, plusModifier_t, outputWriter_t;
    // Create the threads
    pthread_create(&inputGetter_t, NULL, inputGetter, NULL);
    pthread_create(&lineSeparator_t, NULL, lineSeparator, NULL);
    pthread_create(&plusModifier_t, NULL, plusSignModifier, NULL);
    pthread_create(&outputWriter_t, NULL, outputWriter, NULL);

    //wait for termination
    pthread_join(inputGetter_t, NULL);
    pthread_join(lineSeparator_t, NULL);
    pthread_join(plusModifier_t, NULL);
    pthread_join(outputWriter_t, NULL);
    return EXIT_SUCCESS;
}