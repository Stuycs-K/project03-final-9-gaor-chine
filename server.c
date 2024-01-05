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
    printf("\nRecieved from client '%s'\n",line);
    //printf("processing...\n");
    rot13(line);
    //printf("writing...\n");
    b = write(client_socket, line, BUFFER_SIZE);
    if (b == -1) err(16, "server write broke");
}

int main(int argc, char *argv[] ) { 

    int listen_socket = server_setup();
    fd_set read_fds;

    char buff[1025]="";

    while (1){

        FD_ZERO(&read_fds);
        FD_SET(STDIN_FILENO, &read_fds);
        FD_SET(listen_socket,&read_fds);
        int i = select(listen_socket+1, &read_fds, NULL, NULL, NULL);

        // if socket
        if (FD_ISSET(listen_socket, &read_fds)) {
            //accept the connection
            int client_socket = server_tcp_handshake(listen_socket);
            printf("Connected, waiting for data.\n");

            //while(1){
                subserver_logic(client_socket);
            //}

            //close(client_socket);
        }
    }
}

