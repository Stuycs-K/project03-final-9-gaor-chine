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
#include "networking.h"
#include "gameFunctions.h"

#define BUFFERSIZE 100
#define READ 0
#define WRITE 1
// #define WRITE 1

// void err(int i, char*message){
//   if(i < 0){
// 	  printf("Error: %s - %s\n",message, strerror(errno));
//   	exit(1);
//   }
// }

struct player* create_player(char *name, int sd){
    struct player *p = malloc(sizeof(struct player));
    strcpy(p->name, name);
    p->lives = 2;
    p->sd = sd;
    return p;
}

void parse_args(char * line, char * args[]){
    char * linePointer = line;
    char * holder;
    int counter = 0;
    while(holder = strsep(&linePointer, " ")){
        args[counter] = holder;
        counter ++;
    }
    args[counter] = 0;
}
/*
parses words_alpha.txt for the inputed word
returns 1 if word doesn't exist and abides by the rules;
returns 0 of word exists
*/
int parse(char word[]){
    char cpy[100] = "";
    strcpy(cpy, word);
    char * cmdargv[64];
    int len = strlen(word);
    printf("size: %d\n", len);
    int f = fork();
    if(f == 0){
        char front[100] = "grep ";
        char back[] = " words_alpha.txt -w";
        strcat(front,word);
        strcat(front,back);
        parse_args(front,cmdargv);
        int ff = open("checkFile", O_CREAT,0666);
        int file = open("checkFile",O_RDONLY|O_WRONLY|O_TRUNC,0666);//err(file,"line 55");
        fflush(stdout);
        int backup_stdout = dup(STDOUT_FILENO); //err(backup_stdout,"line 57");
        int a = dup2(file, STDOUT_FILENO); //err(a,"line 58");
        int b = close(file); //err(b,"line 59");
        int c = execvp(cmdargv[0],cmdargv);//err(c,"line 60");
        int d = dup2(backup_stdout,STDOUT_FILENO);//err(d,"line 61");
        close(file);
        exit(0);
    }else{
        char string[100]="";
        char wds [100] = "";
        int status;
        wait(&status);
        int file = open("checkFile",O_RDONLY,0644);//err(file,"line 65");
        int s = read(file,string,len); //err(s,"line 68");
        printf("read from file: %s\n",string);
        int il = strcmp(string, cpy);
        printf("compared: %d\n",il);
        FILE * u = fopen("usedWords", "x");
        FILE * usedWords = fopen("usedWords", "r+");
        while(fgets(wds,100,usedWords)!= NULL){
        //     if(strcmp(string,wds) == 0) return 1;
        }
    }
    return 0;
}
//---------------------------------------------------------------------------------------------
int main(int argc, char * argv[]){
    
    printf("Tests:\n");
    char c[] = "ahfdsgakjdshgf";
    int i;
    char string[100];
    i = parse(c);
    printf("returned: %d\n", i);
    // int f = open("test.txt",O_RDONLY);
    // read(f,string,100);
    // printf("test: %s\n",string);
}

int cur_players(struct player **ps){
    int cur_clients = 0;
    for (int x = 0; x < MAX_CLIENTS; x++){
        if (ps[x] != NULL) cur_clients++;
    }
    return cur_clients;
}

void write_all(struct player** ps, char * buff){
    for (int x = 0; x < MAX_CLIENTS; x++){
        if(ps[x] != NULL){
            int b = write(ps[x]->sd, buff, BUFFER_SIZE);
            if (b == -1) err(16, "server write broke");
        }
    }
}

void help(struct player *p){ //show all commands
    char buff[BUFFER_SIZE] = "|| Commands:\n";
    strcat(buff, "|| /help : show all commands\n");
    strcat(buff, "|| /start : start the Word Bomb game");
    printf("%s", buff);
    write(p->sd, buff, BUFFER_SIZE);
}

void start_game(struct player **ps, int* game_status){ //starts the game
    *game_status = 1; //change to true
    // int f = fork();
    // if (f==0){ //child
    //     char buff[BUFFER_SIZE] = "Game is starting.";
    //     write_all(ps, buff);
    //     for (int x = 0; x < 5; x++){
    //         sleep(1);
    //     }
    //     sprintf(buff, "Game has ended.");
    //     write_all(ps, buff);
    // }
    // // else{
    // //     int status;
    //     wait(&status);
    // }
    // *game_status = 0; //change to false
}

void command_logic(struct player **ps, struct player *p, char* line, int* game_status){
    printf("%d\n", *game_status);
    char * cmdargv[64];
    char buff[100] = "command does not exist.";
    parse_args(line, cmdargv);
    //printf("%s\n", cmdargv[0]);
    if (strcmp(cmdargv[0], "/help") == 0) help(p);
    else if (strcmp(cmdargv[0], "/start") == 0){
        if(*game_status == 1){
            sprintf(buff, "Game is in progress.");
            write(p->sd, buff, sizeof(buff));
        }
        else if (cur_players(ps) < 2){
            sprintf(buff, "Game requires at least 2 players. Current number of players: %d", cur_players(ps));
            write(p->sd, buff, sizeof(buff));
        }
        else start_game(ps, game_status);
    }
    else write(p->sd, buff, sizeof(buff));
}

