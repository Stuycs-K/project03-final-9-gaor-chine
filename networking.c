#include "networking.h"
#include "gameFunctions.h"
//ip: 149.89.161.100

/*Connect to the server
 *return the to_server socket descriptor
 *blocks until connection is made.*/
int client_tcp_handshake(char * server_address) {

  //getaddrinfo
  struct addrinfo *hints, *results;
  hints = calloc(1,sizeof(struct addrinfo));
  hints->ai_family = AF_INET;
  hints->ai_socktype = SOCK_STREAM; //TCP socket
  getaddrinfo(server_address, PORT, hints, &results);
  
  int serverd;
  //create the socket
  serverd = socket(AF_INET, SOCK_STREAM, 0);

  //connect to the server
  connect(serverd, results->ai_addr, results->ai_addrlen);

  free(hints);
  freeaddrinfo(results);

  return serverd;
}

/*Accept a connection from a client
 *return the to_client socket descriptor
 *blocks until connection is made.
 */
int server_tcp_handshake(int listen_socket){
    int client_socket;

    //accept the client connection
    socklen_t sock_size;
    struct sockaddr_storage client_address;
    sock_size = sizeof(client_address);
    client_socket = accept(listen_socket,(struct sockaddr *)&client_address, &sock_size);
    if (client_socket == -1) err(41, "server_tcp_handshake couldn't accept");
  
    return client_socket;
}

/*Create and bind a socket.
* Place the socket in a listening state.
*/
int server_setup() {
  //setup structs for getaddrinfo
  struct addrinfo * hints, * results;
  hints = calloc(1,sizeof(struct addrinfo));
  hints->ai_family = AF_INET;
  hints->ai_socktype = SOCK_STREAM; //TCP socket
  hints->ai_flags = AI_PASSIVE; //only needed on server
  getaddrinfo(NULL, PORT, hints, &results);  //Server sets node to NULL

  //create the socket
  int clientd = socket(results->ai_family, results->ai_socktype, results->ai_protocol);
  
  //this code should get around the address in use error
  int yes = 1;
  int sockOpt =  setsockopt(clientd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));
  err(sockOpt,"sockopt  error");
  
  //bind the socket to address and port
  bind(clientd, results->ai_addr, results->ai_addrlen);

  //set socket to listen state
  listen(clientd, 10);

  //free the structs used by getaddrinfo
  free(hints);
  freeaddrinfo(results);
  
  return clientd;
}

void stripNewLine(char* input){
    for(int x = 0; x < strlen(input); x++){
        if (strcmp(&input[x], "\n") == 0 || strcmp(&input[x], "\r") == 0){
            input[x] = 0; // removes new line
            break;
        }
    } 
}

void err(int i, char*message){
  if(i < 0){
	  printf("Error: %s - %s\n",message, strerror(errno));
  	exit(1);
  }
}

void chat_logic(struct player** ps, struct player* p, char* line, int* game_status){
    //rot13(line);

    //printf("writing...\n");
    //printf("%d", (int)(sizeof(ps)/sizeof(struct player)));
    char buff[2* BUFFER_SIZE] = "";
    //printf("%s", line);
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
            //printf("%s", buff);
            int b = write(ps[x]->sd, buff, 2* BUFFER_SIZE);
            if (b == -1) err(16, "server write broke");
        }
    }
}