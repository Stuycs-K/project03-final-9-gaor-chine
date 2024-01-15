#include "networking.h"
#include "gameFunctions.h"

void clientLogic(int server_socket){
    char line[BUFFER_SIZE];
    printf("enter your message:\n");
    fgets(line, BUFFER_SIZE, stdin);
    stripNewLine(line);
    //printf("writing...\n");
    int b = write(server_socket, line, BUFFER_SIZE);
    if (b == -1) err(6, "client write broke");
    //printf("reading...\n");
    //b = read(server_socket, line, BUFFER_SIZE);
    //if (b == -1) err(6, "client read broke");
    //printf("%s\n", line);
}

int main(int argc, char *argv[] ) {
    char* IP = "127.0.0.1";
    if(argc>1){
        IP=argv[1];
        //printf("%s", IP);
    }
    int server_socket = client_tcp_handshake(IP);
    if (server_socket == -1) err(24, "client handshake failed");
    printf("client connected.\n");
    
    printf("|| Enter your username: ");
    char name[NAME_SIZE] = "";
    fgets(name, NAME_SIZE, stdin);
    write(server_socket, name, strlen(name)+1);
    //read(server_socket, buff, BUFFER_SIZE); //welcome msg
    //printf("%s\n", buff);

    int max_sd = server_socket;
    fd_set read_fds;

    while(1){

        FD_ZERO(&read_fds);
        FD_SET(server_socket,&read_fds);
        FD_SET(STDIN_FILENO,&read_fds);
        char buff[BUFFER_SIZE] = "";
        int i = select(max_sd+1, &read_fds, NULL, NULL, NULL);

        //if standard in, use fgets, writes to server
        if (FD_ISSET(STDIN_FILENO, &read_fds)) {
            fgets(buff, BUFFER_SIZE, stdin);
            int b = write(server_socket, buff, strlen(buff)+1);
            if (b==-1) err(46, "writing stdin failed\n");
            //printf("bytes sent: %d\n", b);
            
        }

        if (FD_ISSET(server_socket, &read_fds)) { //listening from server
            int bytes = read(server_socket, buff, BUFFER_SIZE);
            //printf("bytes from server: %d\n", bytes);
            stripNewLine(buff);
            if (bytes == -1) err(80, "check server exit failed\n");
            if (bytes == 0){ //server disconnect
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

