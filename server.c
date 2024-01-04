#include "networking.h"
#include "gameFunctions.h"

void rot13(char* line){
  for(int x = 0; x < strlen(line); x++){
    if (line[x] < 'A');                           // do nothing
    else if (line[x] < 'A' + 13) line[x] += 13;   // A-N
    else if (line[x] < 'a') line[x] -= 13;        // O-Z
    else if (line[x] < 'a' + 13) line[x] += 13;   // a-n
    else if (line[x] < 'z' + 1) line[x] -= 13;    // n-z
  }
}

void subserver_logic(int client_socket){
  char line[BUFFER_SIZE];
  //printf("reading...\n");
  int b = read(client_socket, line, BUFFER_SIZE);
  if (b == -1) err(13, "server read broke");
  //printf("processing...\n");
  rot13(line);
  //printf("writing...\n");
  b = write(client_socket, line, BUFFER_SIZE);
  if (b == -1) err(16, "server write broke");
}

int main(int argc, char *argv[] ) { 

  int listen_socket = server_setup();

  while (1){
    int client_socket = server_tcp_handshake(listen_socket);
    //printf("client connected.\n");

    int f = fork();
    if (f == 0){
      while(1) {
        subserver_logic(client_socket);
      }
    }
        
  }
}

