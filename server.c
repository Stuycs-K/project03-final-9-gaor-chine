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

void processing_logic(int client_socket, char* line){
    //char line[BUFFER_SIZE];
    //printf("reading...\n");
    //int b = read(client_socket, line, BUFFER_SIZE);
    //if (b == -1) err(13, "server read broke");
    printf("\nRecieved from client '%s'\n",line);
    //printf("processing...\n");
    rot13(line);
    //printf("writing...\n");
    int b = write(client_socket, line, BUFFER_SIZE);
    if (b == -1) err(16, "server write broke");
}

int main(int argc, char *argv[] ) { 

    int listen_socket = server_setup();
    int max_sd = listen_socket; // max socket descriptor, will change as clients connect
    int MAX_CLIENTS = 10;
    int clients[MAX_CLIENTS]; // store client socket file descriptors
    for (int x = 0; x < MAX_CLIENTS; x++) clients[x] = 0; // initialize
    fd_set read_fds;
    char buff[BUFFER_SIZE]="";

    while (1){
        
        int cur_clients = 0;
        FD_ZERO(&read_fds);
        FD_SET(listen_socket,&read_fds);
        for (int x = 0; x < MAX_CLIENTS; x++){
            if (clients[x] > 0){
                FD_SET(clients[x], &read_fds); // add clients to fds
                cur_clients += 1;
            }
            if (clients[x] > max_sd) max_sd = clients[x]; // update max socket descriptor

        }
        printf("%d clients connected.\n", cur_clients);
        int i = select(max_sd+1, &read_fds, NULL, NULL, NULL);

        //if standard in, use fgets
        if (FD_ISSET(STDIN_FILENO, &read_fds)) {
            fgets(buff, sizeof(buff), stdin);
            stripNewLine(buff);
            write(server_socket, buff, strlen(buff));
            //printf("Recieved from terminal: '%s'\n",buff);
        }

        // if listen_socket, accept connection and add to client sockets array
        if (FD_ISSET(listen_socket, &read_fds)) {
            //accept the connection
            int client_socket = server_tcp_handshake(listen_socket);
            for (int x = 0; x < MAX_CLIENTS; x++){
                //printf("%d\n", clients[x]);
                if (clients[x] == 0){
                    clients[x] = client_socket;
                    printf("%d sd %d added to client list.\n", x, clients[x]);
                    break; //to add once
                }
            }
            printf("Connected, waiting for data.\n");

            //subserver_logic(client_socket);
            
            //close(client_socket);
        }

        //if existing client
        for (int x = 0; x < MAX_CLIENTS; x++){

            int sd = clients[x];
            if (FD_ISSET(sd, &read_fds)){

                //check if client exited
                int bytes = read(sd, buff, BUFFER_SIZE);
                if (bytes == -1) err(80, "check client exit failed\n");
                if (bytes == 0){
                    printf("client %d has disconnected.\n", x);
                    clients[x] = 0;
                }
                else{
                    processing_logic(sd, buff);
                }
            }

        }




    }
}

