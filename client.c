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
    int max_sd = server_socket;
    fd_set read_fds;

    while(1){

        FD_ZERO(&read_fds);
        FD_SET(server_socket,&read_fds);
        int i = select(max_sd+1, &read_fds, NULL, NULL, NULL);

        if (FD_ISSET(server_socket, &read_fds)) { //listening from server
            char buff[BUFFER_SIZE];
            int bytes = read(server_socket, buff, BUFFER_SIZE);
            if (bytes == -1) err(80, "check server exit failed\n");
            if (bytes == 0){
                printf("server has disconnected.\n");
                exit(0);
            }
            else{
                printf("%s\n", buff);
            }
        }
        
        //clientLogic(server_socket);
    }
}

