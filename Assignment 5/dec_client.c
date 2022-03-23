#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>  // ssize_t
#include <sys/socket.h> // send(),recv()
#include <netdb.h>      // gethostbyname()

/**
* Client code
* 1. Create a socket and connect to the server specified in the command arugments.
* 2. Prompt the user for input and send that input as a message to the server.
* 3. Print the message received from the server and exit the program.
*/

// Error function used for reporting issues
void error(const char *msg) { 
  perror(msg); 
  exit(2); 
} 

// Set up the address struct
void setupAddressStruct(struct sockaddr_in* address, 
                        int portNumber, 
                        char* hostname){
 
  // Clear out the address struct
  memset((char*) address, '\0', sizeof(*address)); 

  // The address should be network capable
  address->sin_family = AF_INET;
  // Store the port number
  address->sin_port = htons(portNumber);

  // Get the DNS entry for this host name
  struct hostent* hostInfo = gethostbyname(hostname); 
  if (hostInfo == NULL) { 
    fprintf(stderr, "CLIENT: ERROR, no such host\n"); 
    exit(0); 
  }
  // Copy the first IP address from the DNS entry to sin_addr.s_addr
  memcpy((char*) &address->sin_addr.s_addr, 
        hostInfo->h_addr_list[0],
        hostInfo->h_length);
}

int main(int argc, char *argv[]) {
  int socketFD, portNumber, charsWritten, charsRead, i;
  struct sockaddr_in serverAddress;
  char ciphertext_buffer[70000];
  char key_buffer[70000];
  char decoded_buffer[70000];
  char response_buffer[1000];
  char id_buffer[10];

  // Check usage & args
  if (argc < 4) { 
    fprintf(stderr,"USAGE: %s plaintext key port\n", argv[0]); 
    exit(0); 
  } 



  // GET CIPHERTEXT FROM FILE
  FILE *fp;
  fp = fopen(argv[1], "r");

  if(fp == NULL){
      perror("Couldn't open the file. Check entry.\n");
      exit(1);
    }
  fgets(ciphertext_buffer, sizeof(ciphertext_buffer) - 1, fp);
  fclose(fp);
  // Remove the trailing \n that fgets adds
  ciphertext_buffer[strcspn(ciphertext_buffer, "\n")] = '\0'; 




  // GET KEY FROM FILE
  fp = fopen(argv[2], "r");
   if(fp == NULL){
      perror("Couldn't open the file. Check entry.\n");
      exit(1);
    }
  fgets(key_buffer, sizeof(key_buffer) - 1, fp);
  fclose(fp);
   // Remove the trailing \n that fgets adds
  key_buffer[strcspn(key_buffer, "\n")] = '\0'; 


  //check validity of ciphertext for any characters not A-Z or space.
  for(i = 0; i < strlen(ciphertext_buffer); i++){
      if(ciphertext_buffer[i] < 65 || ciphertext_buffer[i] > 90){
          if(ciphertext_buffer[i] != 32){
              fprintf(stderr, "Bad character(s) in ciphertext. Exiting\n");
              exit(1);
          }
      }
  }


  //check validity of key for any characters not A-Z or space, or shorter than plaintext.
  if(strlen(key_buffer) < strlen(ciphertext_buffer)){
      fprintf(stderr, "Key is shorter than plaintext. Exiting\n");
      exit(1);
  }
  for(i = 0; i < strlen(key_buffer); i++){
      if(key_buffer[i] < 65 || key_buffer[i] > 90){
          if(key_buffer[i] != 32){
              fprintf(stderr, "Bad character(s) in key (THIS SHOULD NEVER HAPPEN). Exiting\n");
              exit(1);
          }
      }
  }


    // Create a socket
  socketFD = socket(AF_INET, SOCK_STREAM, 0); 
  if (socketFD < 0){
    error("CLIENT: ERROR opening socket");
  }

   // Set up the server address struct
  setupAddressStruct(&serverAddress, atoi(argv[3]), "localhost");

  // Connect to enc_server
  if (connect(socketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0){
    error("CLIENT: ERROR connecting");
  }


  // Send the ciphertext to the server

  charsWritten = send(socketFD, ciphertext_buffer, sizeof(ciphertext_buffer), 0); 
  if (charsWritten < 0){
    error("CLIENT: ERROR writing to socket");
  }
  if (charsWritten < strlen(ciphertext_buffer)){
    printf("CLIENT: WARNING: Not all data written to socket!\n");
  }

    // Clear out the buffer again for reuse
  memset(decoded_buffer, '\0', sizeof(decoded_buffer));

  // Get return message from server
  // Read data from the socket, leaving \0 at end
  charsRead = recv(socketFD, response_buffer, sizeof(response_buffer) - 1, 0); 
  if (charsRead < 0){
    error("CLIENT: ERROR reading from socket");
  }
//   printf("CLIENT: I received this from the server: \"%s\"\n", response_buffer);


  charsWritten = send(socketFD, key_buffer, sizeof(key_buffer), 0); 
  if (charsWritten < 0){
    error("CLIENT: ERROR writing to socket");
  }
  if (charsWritten < strlen(key_buffer)){
    printf("CLIENT: WARNING: Not all data written to socket!\n");
  }

  
// Clear out the buffer again for reuse
  memset(key_buffer, '\0', sizeof(key_buffer));


  // Get return message from server
  // Read data from the socket, leaving \0 at end
  charsRead = recv(socketFD, decoded_buffer, sizeof(decoded_buffer) - 1, 0); 
  if (charsRead < 0){
    error("CLIENT: ERROR reading from socket");
  }

  //the decoded message
  printf("%s\n", decoded_buffer);

  // Clear out the buffer again for reuse
  memset(decoded_buffer, '\0', sizeof(decoded_buffer));

  // Close the socket
  close(socketFD); 
  return 0;
}