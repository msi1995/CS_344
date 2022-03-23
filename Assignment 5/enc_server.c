#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <math.h>

// Error function used for reporting issues
void error(const char *msg) {
  perror(msg);
  exit(1);
} 

// Set up the address struct for the server socket
void setupAddressStruct(struct sockaddr_in* address, 
                        int portNumber){
 
  // Clear out the address struct
  memset((char*) address, '\0', sizeof(*address)); 

  // The address should be network capable
  address->sin_family = AF_INET;
  // Store the port number
  address->sin_port = htons(portNumber);
  // Allow a client at any address to connect to this server
  address->sin_addr.s_addr = INADDR_ANY;
}

int main(int argc, char *argv[]){
  int connectionSocket, charsRead, i, status;
  int totalChars = 0;
  char plaintext_buffer[70000];
  char key_buffer[70000];
  char ciphertext_buffer[70000];
  pid_t pid;
  struct sockaddr_in serverAddress, clientAddress;
  socklen_t sizeOfClientInfo = sizeof(clientAddress);

  // Check usage & args
  if (argc < 2) { 
    fprintf(stderr,"USAGE: %s port\n", argv[0]); 
    exit(1);
  } 
  
  // Create the socket that will listen for connections
  int listenSocket = socket(AF_INET, SOCK_STREAM, 0);
  if (listenSocket < 0) {
    error("ERROR opening socket");
  }

  // Set up the address struct for the server socket
  setupAddressStruct(&serverAddress, atoi(argv[1]));

  // Associate the socket to the port
  if (bind(listenSocket, 
          (struct sockaddr *)&serverAddress, 
          sizeof(serverAddress)) < 0){
    error("ERROR on binding");
  }

  // Start listening for connetions. Allow up to 5 connections to queue up
  listen(listenSocket, 5); 
  
  // Accept a connection, blocking if one is not available until one connects
  while(1){
    // Accept the connection request which creates a connection socket
    connectionSocket = accept(listenSocket, 
                (struct sockaddr *)&clientAddress, 
                &sizeOfClientInfo); 
    if (connectionSocket < 0){
      error("ERROR on accept");
    }

    // printf("SERVER: Connected to client running at host %d port %d\n", 
    //                       ntohs(clientAddress.sin_addr.s_addr),
    //                       ntohs(clientAddress.sin_port));

    // Get the message from the client and display it

    pid = fork();
    switch(pid){
        case -1:{
            error("Busted. Couldn't fork\n");
        }
        case 0:{
            memset(plaintext_buffer, '\0', 70000);
            // Read the plaintext from the socket
                charsRead = recv(connectionSocket, plaintext_buffer, sizeof(plaintext_buffer), 0); 
                if (charsRead < 0){
                error("ERROR reading from socket");
                }

            // printf("SERVER: I received this plaintext: \"%s\"\n", plaintext_buffer);

            // Send a Success message back to the client
            charsRead = send(connectionSocket, 
                            "I am the server, and I got your message", 39, 0); 
            if (charsRead < 0){
            error("ERROR writing to socket");
            }

            // Get the message from the client and display it
            memset(key_buffer, '\0', 70000);
            // Read the key from the socket
                charsRead = recv(connectionSocket, key_buffer, sizeof(key_buffer), 0); 
                if (charsRead < 0){
                error("ERROR reading from socket");
                }
            // printf("SERVER: I received this key: \"%s\"\n", key_buffer);

            for(i = 0; i < strlen(plaintext_buffer); i++){
                int offsetAmount = key_buffer[i];
                int plaintext_char = plaintext_buffer[i];
                plaintext_char -= 65;
                offsetAmount -= 65;
                if(offsetAmount == -33){
                    offsetAmount = 26;
                }
                if(plaintext_char == -33){
                    plaintext_char = 26;
                }
                int ciphertext_ascii = (plaintext_char + offsetAmount) % 27;
                ciphertext_ascii += 65;
                if(ciphertext_ascii == 91){
                    ciphertext_ascii = 32;
                }
                ciphertext_buffer[i] = ciphertext_ascii;
                }


            ciphertext_buffer[i] = '\0';
            // printf("I'm sending back: %s\n", ciphertext_buffer);
            // Send processed ciphertext back to the user.
            charsRead = send(connectionSocket, 
                            ciphertext_buffer, sizeof(ciphertext_buffer), 0); 
            if (charsRead < 0){
            error("ERROR writing to socket");
            }

            exit(0);
        }

        default:{
            pid_t actualpid = waitpid(pid, &status, WNOHANG);
        }
    }



    // Close the connection socket for this client
    close(connectionSocket); 
  }
  // Close the listening socket
  close(listenSocket); 
  return 0;
}