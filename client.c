#include "networking.h"
#include "gameFunctions.h"

void stripNewLine(char* input){
  for(int x = 0; x < strlen(input); x++){
    if (strcmp(&input[x], "\n") == 0 || strcmp(&input[x], "\r") == 0){
      input[x] = 0; // removes new line
      break;
    }
  }
}

void clientLogic(int server_socket){
  char line[BUFFER_SIZE];
  printf("enter your message:\n");
  fgets(line, BUFFER_SIZE, stdin);
  stripNewLine(line);
  //printf("writing...\n");
  int b = write(server_socket, line, BUFFER_SIZE);
  if (b == -1) err(6, "client write broke");
  //printf("reading...\n");
  b = read(server_socket, line, BUFFER_SIZE);
  if (b == -1) err(6, "client read broke");
  printf("%s\n", line);
}

int main(int argc, char *argv[] ) {
  char* IP = "127.0.0.1";
  if(argc>1){
    IP=argv[1];
  }
  int server_socket = client_tcp_handshake(IP);
  printf("client connected.\n");
  //while(1){
    clientLogic(server_socket);
  //}
}

