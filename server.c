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

void chat_logic(struct player** ps, struct player* p, char* line, int* game_status){
    //rot13(line);

    //printf("writing...\n");
    //printf("%d", (int)(sizeof(ps)/sizeof(struct player)));
    char buff[BUFFER_SIZE] = "";
    for (int x = 0; x < MAX_CLIENTS; x++){
        if(ps[x] != NULL && ps[x] != p){ //do not write to sender
            if (*game_status == 1) {
                char num_lives[10] = "";
                if (p->lives == 1) sprintf(num_lives, "(O)(X)");
                else if (p->lives == 2) sprintf(num_lives, "(O)(O)");
                else sprintf(num_lives, "(DEAD)");
                sprintf(buff, "%s %s: %s", num_lives, p->name, line); //shows lives
            }
            else sprintf(buff, "%s: %s", p->name, line);
            
            int b = write(ps[x]->sd, buff, BUFFER_SIZE);
            if (b == -1) err(16, "server write broke");
        }
    }
}

int main(int argc, char *argv[] ) { 

    int listen_socket = server_setup();
    int max_sd = listen_socket; // max socket descriptor, will change as clients connect
    //int clients[MAX_CLIENTS]; // store client socket file descriptors
    //for (int x = 0; x < MAX_CLIENTS; x++) clients[x] = 0; // initialize
    
    struct player** players = calloc(MAX_CLIENTS, sizeof(struct player));
    
    fd_set read_fds;
    fd_set cur_player_only;
    char buff[BUFFER_SIZE]="";

    struct timeval timeout;
    timeout.tv_sec = 10;
    timeout.tv_usec = 0;

    int temp_game_status = 0;
    int game_status = 0;
    int cur_player_index = 0;
    char prompt[5] = "";

    while (1){
        
        FD_ZERO(&read_fds);
        //FD_ZERO(&cur_player_only);
        FD_SET(listen_socket,&read_fds);

        for (int x = 0; x < MAX_CLIENTS; x++){
            if (players[x] != NULL){
                struct player* player = players[x];
                FD_SET(player->sd, &read_fds); // add clients to fds
                if (player->sd > max_sd) max_sd = player->sd; // update max socket descriptor
            }
        }
        int cur_clients = cur_players(players);
        printf("%d clients connected.\n", cur_clients);
        struct player *player_turn = players[cur_player_index];

        if (game_status == 0 && temp_game_status == 1){ //game just started, reset the timeout
            timeout.tv_sec = 10;
            timeout.tv_usec = 0;
            randPrompt(prompt);
        }
        game_status = temp_game_status; // ONLY UPDATES HERE to start logic from beginning
        if (game_status == 1) { //game logic
            if (cur_clients == 1){ //check for end game condition
                temp_game_status = 0;
                write_all(players, "Game is ending.");
            }

            player_turn = players[cur_player_index];
            sprintf(buff, "|| It is %s's turn.\n|| The prompt is: %s", player_turn->name, prompt);
            write_all(players, buff);
        }

        int i = select(max_sd+1, &read_fds, NULL, NULL, &timeout);
        if (i == 0) { //timeout occured
            if (game_status == 1){
                sprintf(buff, "|| The bomb exploded! %s lost a life.", player_turn->name);
                (player_turn->lives)--;
                write_all(players, buff);
                cur_player_index = next_player_index(cur_player_index, players);
            }
            timeout.tv_sec = 10;
            timeout.tv_usec = 0;
        }

        // if listen_socket, accept connection and add to client sockets array
        if (FD_ISSET(listen_socket, &read_fds)) {
            //accept the connection
            int client_socket = server_tcp_handshake(listen_socket);
            if (client_socket == -1) err(98, "server handshake failed");
            read(client_socket, buff, BUFFER_SIZE); //read for username
            //printf("%s", buff);
            struct player * p = create_player(buff, client_socket);

            for (int x = 0; x < MAX_CLIENTS; x++){
                //printf("%d\n", clients[x]);
                if (players[x] == NULL){
                    players[x] = p;
                    printf("%d sd %d added to client list.\n", x, p->sd);
                    break; //to add once
                }
            }
            sprintf(buff, "|| Welcome to Word Bomb, %s!", p->name);
            write(p->sd, buff, BUFFER_SIZE);
            sprintf(buff, "|| Type \"/help\" for all commands.");
            write(p->sd, buff, BUFFER_SIZE);
            printf("Connected, waiting for data.\n");


            //subserver_logic(client_socket);
            
            //close(client_socket);
        }

        //if existing client
        for (int x = 0; x < MAX_CLIENTS; x++){

            //int sd = clients[x];
            int sd;
            if (players[x] != NULL) { //player exists
            
                sd = players[x]->sd;
                if (FD_ISSET(sd, &read_fds)){

                    //check if client exited
                    int bytes = read(sd, buff, BUFFER_SIZE);
                    if (bytes == -1) err(80, "check client exit failed\n");
                    if (bytes == 0){
                        printf("client %d has disconnected.\n", x);
                        // clients[x] = 0;
                        players[x] = 0;
                        if (game_status == 1){
                            cur_player_index = next_player_index(cur_player_index, players);
                            timeout.tv_sec = 10;
                            timeout.tv_usec = 0;
                        }
                    }
                    else{
                        printf("Recieved from client '%s'\n",buff);
                        //prioritize commands, to game inputs, to chat
                        if(buff[0] == '/') command_logic(players, players[x], buff, &temp_game_status);
                        else if (game_status == 1) { 
                            if (sd = player_turn->sd){ //if input is from current player's turn
                                check_logic(players, players[x], &cur_player_index, buff, &temp_game_status, &timeout, prompt);
                            }
                            chat_logic(players, players[x], buff, &temp_game_status);
                        }
                        else chat_logic(players, players[x], buff, &temp_game_status);
                    }
                }
            }

        }




    }
}

