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

int main(int argc, char *argv[] ) { 

    int listen_socket = server_setup();
    int max_sd = listen_socket; // max socket descriptor, will change as clients connect
    //int clients[MAX_CLIENTS]; // store client socket file descriptors
    //for (int x = 0; x < MAX_CLIENTS; x++) clients[x] = 0; // initialize
    
    struct player** players = calloc(MAX_CLIENTS, sizeof(struct player*));
    
    fd_set read_fds;
    //fd_set cur_player_only;

    struct timeval timeout;
    timeout.tv_sec = 10;
    timeout.tv_usec = 0;

    int temp_game_status = 0; //track for next loop
    int next_player_i = 0;
    int game_status = 0; //changes once per loop
    int cur_player_index = 0;
    char *prompt = malloc(10);

    while (1){
        
        FD_ZERO(&read_fds);
        //FD_ZERO(&cur_player_only);
        FD_SET(listen_socket,&read_fds); 
        char buff[BUFFER_SIZE]="";

        for (int x = 0; x < MAX_CLIENTS; x++){
            if (players[x] != NULL){
                struct player* player = players[x];
                FD_SET(player->sd, &read_fds); // add clients to fds
                if (player->sd > max_sd) max_sd = player->sd; // update max socket descriptor
            }
        }
        int cur_clients = cur_players(players);
        printf("%d clients connected.\n", cur_clients);
        if (cur_clients == 10) exit(0);
        struct player *player_turn;

        if (game_status == 0 && temp_game_status == 1){ //game just started, reset the timeout
            timeout.tv_sec = 10;
            timeout.tv_usec = 0;
            strcpy(prompt, randPrompt());
            player_turn = players[cur_player_index];
            char buff1[] = "|| Game is starting.\n";
            char buff2[200] = "";
            snprintf(buff2, 200, "|| It is %s's turn.\n|| The prompt is: %s\n", player_turn->name, prompt);
            snprintf(buff, BUFFER_SIZE, "%s%s", buff1, buff2);
            write_all(players, buff);
        }
        game_status = temp_game_status; // ONLY UPDATES HERE to start logic from beginning
        cur_player_index = next_player_i;
        player_turn = players[cur_player_index];
        printf("gamestatus: %d\ncurrent player: %s\n", game_status, players[cur_player_index]->name);
        if (game_status == 1) { //game logic
            if (cur_clients == 1){ //check for end game condition
                temp_game_status = 0;
                struct player * winner;
                for (int x = 0; x <MAX_CLIENTS; x++){
                    if (players[x] != NULL) winner = players[x];
                }
                sprintf(buff, "|| Game is ending.\n|| The winner is: %s!\n|| Congratulations!\n", winner->name);
                write_all(players, buff);
            }
        }

        int i = select(max_sd+1, &read_fds, NULL, NULL, &timeout);
        if (i == 0) { //timeout occured
            if (game_status == 1){
                //char buff[300] = "";
                snprintf(buff, 300, "|| The bomb exploded! %s lost a life.\n", player_turn->name);
                (player_turn->lives)--;
                if (player_turn->lives == 0){
                    char buff2[300]="";
                    snprintf(buff2, 300, "|| %s has died! %d players remaining.\n", player_turn->name, cur_players(players)-1);
                    strcat(buff, buff2);
                    //write_all(players, buff2);
                    for(int x = 0; x < MAX_CLIENTS; x++){ //remove from player list and disconnect
                        if (players[x]!=NULL){
                            if (player_turn->sd == players[x]->sd) players[x] = 0;
                        }
                    }
                    close(player_turn->sd);
                }
                if (cur_players(players) == 1){ //check for end game condition
                    struct player * winner;
                    for (int x = 0; x <MAX_CLIENTS; x++){
                        if (players[x] != NULL) winner = players[x];
                    }
                    char buff2[300]="";
                    snprintf(buff2, 300, "|| Game is ending.\n|| The winner is: %s!\n|| Congratulations!\n", winner->name);
                    strcat(buff, buff2);
                    temp_game_status = 0;
                }
                else{
                    next_player_i = next_player_index(cur_player_index, players);
                    strcpy(prompt, randPrompt());
                    char buff2[300] = "";
                    snprintf(buff2, 300, "|| It is %s's turn.\n|| The prompt is: %s\n", players[next_player_i]->name, prompt);
                    strcat(buff, buff2);
                }
                write_all(players, buff);
            }
            timeout.tv_sec = 10;
            timeout.tv_usec = 0;
        }

        // if listen_socket, accept connection and add to client sockets array
        if (FD_ISSET(listen_socket, &read_fds)) {
            //accept the connection
            int client_socket = server_tcp_handshake(listen_socket);
            //printf("client socket: %d", client_socket);
            if (client_socket == -1) err(98, "server handshake failed");
            char name[NAME_SIZE] = "";
            read(client_socket, buff, BUFFER_SIZE); //read for username
            if (game_status == 1) {
                sprintf(buff, "|| Game in progress, unable to join.\n");
                write(client_socket, buff, strlen(buff)+1);
                close(client_socket);
            }
            else{
                stripNewLine(buff);
                snprintf(name, NAME_SIZE, "%.90s", buff);
                //printf("name: %s\n", name);
                struct player * p = create_player(name, client_socket);
                //printf("client socket: %d\n", p->sd);
                //printf("lives: %d\n", p->lives);

                for (int x = 0; x < MAX_CLIENTS; x++){
                    //printf("%d\n", clients[x]);
                    if (players[x] == NULL){
                        players[x] = p;
                        printf("%d sd %d added to client list.\n", x, p->sd);
                        break; //to add once
                    }
                }
                sprintf(buff, "|| Welcome to Word Bomb, %s!\n", p->name);
                strcat(buff, "|| Type \"/help\" for all commands.\n");
                write(p->sd, buff, strlen(buff)+1);
                printf("Connected, waiting for data.\n");
                char buff1[BUFFER_SIZE] = "";
                sprintf(buff1, "|| %s joined the game.\n", p->name);
                write_all(players, buff1);

                //subserver_logic(client_socket);
                
                //close(client_socket);
            }
        }

        //if existing client
        for (int x = 0; x < MAX_CLIENTS; x++){
            int sd;
            if (players[x] != NULL) { //player exists
            
                sd = players[x]->sd;
                if (FD_ISSET(sd, &read_fds)){

                    //check if client exited
                    int bytes = read(sd, buff, BUFFER_SIZE);
                    char *temp = calloc(1,strlen(buff)+1);
                    snprintf(temp, strlen(buff)+1, "%s", buff);
                    printf("bytes: %d\n strlen: %ld\n", bytes, strlen(temp));
                    if (bytes == -1) err(80, "check client exit failed\n");
                    if (bytes == 0){
                        printf("client %d has disconnected.\n", x);
                        if (game_status == 1){
                            char buff1[BUFFER_SIZE]="";
                            sprintf(buff1, "%s left the game. %d players remaining.\n", players[x]->name, cur_players(players)-1);
                            write_all(players, buff1);
                            if (cur_players(players) == 1){ //check for end game condition
                                temp_game_status = 0;
                                struct player * winner;
                                for (int x = 0; x <MAX_CLIENTS; x++){
                                    if (players[x] != NULL) winner = players[x];
                                }
                                sprintf(buff, "|| Game is ending.\n|| The winner is: %s!\n|| Congratulations!\n", winner->name);
                                write_all(players, buff);
                            }
                            else{
                                next_player_i = next_player_index(cur_player_index, players);
                                timeout.tv_sec = 10;
                                timeout.tv_usec = 0;
                                snprintf(buff, BUFFER_SIZE, "|| It is %s's turn.\n|| The prompt is: %s\n", players[next_player_i]->name, prompt);
                                write_all(players, buff);
                            }
                        }
                        else{
                            char buff1[BUFFER_SIZE]="";
                            sprintf(buff1, "%s left the game.", players[x]->name);
                            write_all(players, buff1);
                        }
                        players[x] = 0;
                    }
                    else{
                        stripNewLine(temp);
                        printf("Recieved from client %s '%s'\n", players[x]->name, temp);
                        //prioritize commands, to game inputs, to chat
                        if(temp[0] == '/') command_logic(players, players[x], temp, &temp_game_status, &game_status);
                        else if (game_status == 1) { 
                            // char temp[BUFFER_SIZE] = "";
                            // strcat(temp, buff);
                            chat_logic(players, players[x], temp, &game_status);
                            if (sd == player_turn->sd){ //if input is from current player's turn
                                check_logic(players, players[x], &next_player_i, &cur_player_index, temp, &temp_game_status, &timeout, &prompt);
                            }
                        }
                        else chat_logic(players, players[x], temp, &game_status);
                    }
                }
            }

        }




    }
}

