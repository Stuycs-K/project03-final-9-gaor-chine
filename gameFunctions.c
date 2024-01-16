#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>
#include <time.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/wait.h>
#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>
#include <ctype.h>
#include "networking.h"
#include "gameFunctions.h"

#define BUFFERSIZE 100
#define READ 0
#define WRITE 1

// void err(int i, char*message){
//   if(i < 0){
// 	  printf("Error: %s - %s\n",message, strerror(errno));
//   	exit(1);
//   }
// }

// randPrompt returns a random 2-3 letter prompt read from prompts.txt
// args:        none
// returns:     buff: the prompt
char * randPrompt(){
    char *buff = malloc(10);
    char str[100] = "";
    int rfile = open("/dev/urandom",O_RDONLY,444);
    int i;
    read(rfile, &i, sizeof(int));
    i = i % 1305;
    if(i < 0) i *= -1;
    FILE * prompts = fopen("prompts.txt", "r");
    for(int j = 0; j < i; j++){
        fgets(str,90,prompts);
    }
    fgets(buff,8,prompts);
    stripNewLine(buff);
    //printf("randomized prompt: %s\n strlen of prompt: %ld",buff, strlen(buff));
    return buff;
}
// create_player creates a new player struct
// args:        name: name to be inputted by player; sd: socket descriptor of the player
// returns:     a pointer to the player struct
struct player* create_player(char *name, int sd){
    struct player *p = malloc(sizeof(struct player));
    strcpy(p->name, name);
    p->lives = 2;
    p->sd = sd;
    return p;
}
// parse_args parses through a string and puts it into a char *[] to be exec'ed
// args:        line: line to be parsed; args:array for the processed strings to be placed into
// returns:     no return value
void parse_args(char * line, char * args[]){
    char * linePointer = line;
    char * holder;
    int counter = 0;
    while(holder = strsep(&linePointer, "@|~")){
        args[counter] = holder;
        counter ++;
    }
    args[counter] = 0;
}
// parse parses words_alpha.txt to check for words
// args:        word inputted by player
// returns:     0 if the word exists and has not been used, 1 if the word exists, but has been used, 2 if the word exists
int parse(char *word){
    char cpy[100] = "";
    strcpy(cpy, word);
    char * cmdargv[64];
    int len = strlen(word);
    printf("size: %d\n", len);
    int f = fork();
    if(f == 0){
        char front[256] = "grep@|~";
        char back[] = "@|~words_alpha.txt@|~-w";
        strcat(front,word);
        strcat(front,back);
        parse_args(front,cmdargv);
        int ff = open("checkFile", O_CREAT,0666);
        close(ff);
        int file = open("checkFile",O_RDONLY|O_WRONLY|O_TRUNC,0666);
        fflush(stdout);
        int backup_stdout = dup(STDOUT_FILENO);
        int a = dup2(file, STDOUT_FILENO);
        int b = close(file);
        int c = execvp(cmdargv[0],cmdargv);
        int d = dup2(backup_stdout,STDOUT_FILENO);
        close(file);
        exit(0);
    }else{
        char string[100]="";
        char wds [100] = "";
        int status;
        wait(&status);
        int file = open("checkFile",O_RDONLY,666);
        int s = read(file,string,len);
        printf("read from file: %s\n",string);
        int il = strcmp(string, cpy);
        if(strcmp(string, "") == 0) return 2; 
        printf("compared: %d\n",il);
        int l = open("usedWords", O_CREAT|O_EXCL,0666);
        FILE * usedWords = fopen("usedWords", "r+"); if(usedWords == NULL) printf("fopen failed\n");
        strcat(cpy,"\n");
        while(fgets(wds,100,usedWords)!= NULL){
            if(strcmp(wds, cpy) == 0) return 1;
        }
        int uWords = open("usedWords",O_APPEND|O_WRONLY,0644);
        write(uWords,cpy,strlen(cpy));
        printf("added %s to usedWords\n",cpy);
        close(uWords);
        close(file);
        close(l);
    }
    return 0;
}
// cur_players finds the current number of players connected to the server
// args:        ps: a pointer to a pointer of player
// returns:     cur_clients: the number of players connected
int cur_players(struct player **ps){
    int cur_clients = 0;
    for (int x = 0; x < MAX_CLIENTS; x++){
        if (ps[x] != NULL) {
            printf("%s, ", ps[x]->name);
            cur_clients++;
        }
        else printf("0, ");
    }
    printf("\n");
    return cur_clients;
}
// next_player index gets the index of the next player inthe player**
// args:        cur_plater_index: the index of the current player in the player**; ps: the player**
// returns:     x: the index of the next player
int next_player_index(int cur_player_index, struct player **ps){
    for (int x = cur_player_index + 1; x < MAX_CLIENTS; x++){
        if (ps[x] != NULL) return x;
    }
    for (int x = 0; x <= cur_player_index; x++){ //loop back around
        if (ps[x] != NULL) return x;
    }
}
// write_all writes to all players connected to the server
// args:        ps: the player** containing all the players; buff: the string to be written to all players
// returns:     no return value
void write_all(struct player** ps, char * buff){
    char temp[BUFFER_SIZE] = "";
    strcat(temp, buff);
    for (int x = 0; x < MAX_CLIENTS; x++){
        if(ps[x] != NULL){
            //printf("%ld", strlen(temp));
            int b = write(ps[x]->sd, temp, strlen(temp)+1);
            if (b == -1) err(16, "server write broke");
        }
    }
}
// help generates a list of commands to be sent to a player
// args:        p: the player
// returns:     no return value
void help(struct player *p){ //show all commands
    char buff[BUFFER_SIZE] = "|| Commands:\n";
    strcat(buff, "|| /help : show all commands\n");
    strcat(buff, "|| /howtoplay : see instructions on how to play\n");
    strcat(buff, "|| /start : start the Word Bomb game\n");
    //printf("%s", buff);
    write(p->sd, buff, strlen(buff)+1);
}
// how_to_play returns a guide to be sent to a player
// args:        p: the player
// returns:     no return value
void how_to_play(struct player *p){ //show how to play Word Bomb
    char buff[BUFFER_SIZE] = "|| How to play:\n";
    strcat(buff, "|| Players each begin with two lives. You can see other peoples lives\n");
    strcat(buff, "|| next to their name. (O)(O) for two lives left and (O)(X) for one life left.\n");
    strcat(buff, "|| Players will take turns submitting words that contain the prompt within them.\n");
    strcat(buff, "|| Failure to do so within 10 seconds results in the bomb exploding and the player losing a life.\n");
    strcat(buff, "|| If a player loses all their lives, they lose are removed from the game.\n");
    strcat(buff, "|| Last player standing wins!\n");
    //printf("%s", buff);
    write(p->sd, buff, strlen(buff)+1);
}
// star_game starts the game
// args:        ps: the player**, game_status:the current status of the game
// returns:     no return value
void start_game(struct player **ps, int* game_status){ //starts the game
    *game_status = 1; //change to true
    int l = open("usedWords", O_CREAT|O_TRUNC,0666);
    for (int x = 0; x < MAX_CLIENTS; x++){
        if(ps[x] != NULL) ps[x]->lives = 2;
    }
    close(l);
}
// chat_logic handles the logic of the chat system in the game
// args:        ps: the player**; p: the player; line: chat message from player; game_status: the status of the game
// returns:     no return value
void chat_logic(struct player** ps, struct player* p, char* line, int* game_status){
    //rot13(line);

    //printf("writing...\n");
    //printf("%d", (int)(sizeof(ps)/sizeof(struct player)));
    char buff[BUFFER_SIZE] = "";
    //printf("line: %s strlen(line): %ld\n", line, strlen(line));
    for (int x = 0; x < MAX_CLIENTS; x++){
        if(ps[x] != NULL && ps[x]->sd != p->sd){ //do not write to sender
            if (*game_status == 1) {
                char num_lives[10] = "";
                if (p->lives == 1) strcat(num_lives, "(O)(X)");
                else if (p->lives == 2) strcat(num_lives, "(O)(O)");
                else strcat(num_lives, "(DEAD)");
                snprintf(buff, BUFFER_SIZE, "%s %s: %s\n", num_lives, p->name, line); //shows lives
            }
            else snprintf(buff, BUFFER_SIZE, "%s: %s\n", p->name, line);
            //printf("chat: %s\n", buff);
            //printf("writing to: %d\n", ps[x]->sd);
            int b = write(ps[x]->sd, buff, strlen(buff)+1);
            if (b == -1) err(16, "server write broke");
        }
    }
}
// command_logic handles the logic of the commands based off the status of the game
// args:        ps: the player**; p: the player; line: cp,,amd from player; temp_gamestatus: game status in case game hasnt started yet; game_status: the status of the game
// returns:     no return value
void command_logic(struct player **ps, struct player *p, char* line, int* temp_game_status, int* game_status){
    printf("game_status: %d\n", *game_status);
    char * cmdargv[64];
    char buff[BUFFER_SIZE] = "|| Command does not exist.\n";
    parse_args(line, cmdargv);
    //printf("%s\n", cmdargv[0]);
    if (strcmp(cmdargv[0], "/help") == 0) help(p);
    else if (strcmp(cmdargv[0], "/howtoplay") == 0) how_to_play(p);
    else if (strcmp(cmdargv[0], "/start") == 0){
        if(*game_status == 1){
            sprintf(buff, "|| Game is in progress.\n");
            write(p->sd, buff, strlen(buff)+1);
        }
        else if (cur_players(ps) < 2){
            sprintf(buff, "|| Game requires at least 2 players. Current number of players: %d\n", cur_players(ps));
            write(p->sd, buff, strlen(buff)+1);
        }
        else start_game(ps, temp_game_status);
    }
    else write(p->sd, buff, strlen(buff)+1);
}
// ccheck_logic handles the rules of the game
// args:        ps: the player**; sent_p: the player who sent the line; temp_cur_p: a temporary current player inded; cur_p: index of where in the player** we are located; line: entry from player;
// returns:     no return value
void check_logic(struct player **ps, struct player *sent_p, int *temp_cur_p, int *cur_p, char* line,
                int* game_status, struct timeval *timeout, char** prompt){

    char promptbuff[10];
    strcpy(promptbuff, *prompt);
    for(int i = 0; (promptbuff)[i]; i++){
        promptbuff[i] = tolower(promptbuff[i]);
    }
    for(int i = 0; (line)[i]; i++){
        line[i] = tolower(line[i]);
    }

    if (strlen(line) < 3){
        char reply[BUFFER_SIZE] = "|| Word must be at least 3 characters long, try again!\n";
        write(sent_p->sd, reply, strlen(reply)+1);
    }
    else if (strstr(line, promptbuff) == NULL){
        char reply[BUFFER_SIZE] = "|| Word does not contain prompt, try again!\n";
        write(sent_p->sd, reply, strlen(reply)+1);
    }
    else{
        int check = parse(line);
        if (check == 0){    
            char reply[BUFFER_SIZE] = "";
            char temp[] = "|| Thats right!\n";
            strcpy(*prompt, randPrompt());
            snprintf(reply, BUFFER_SIZE, "%s|| It is %s's turn.\n|| The prompt is: %.3s\n", temp, ps[next_player_index(*cur_p, ps)]->name, *prompt);
            write_all(ps, reply);
            *temp_cur_p = next_player_index(*cur_p, ps);
            timeout->tv_sec = 10;
            timeout->tv_usec = 0;
        }else if(check == 1){
            char reply[BUFFER_SIZE] = "|| Word has already been used, try again!\n";
            write(sent_p->sd, reply, strlen(reply)+1);
        }else if(check == 2){
            char reply[BUFFER_SIZE] = "|| Word does not exist, try again!\n";
            write(sent_p->sd, reply, strlen(reply)+1);
        }
    }
    

}




