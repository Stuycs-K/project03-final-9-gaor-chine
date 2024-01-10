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

#define BUFFERSIZE 100
#define READ 0
// #define WRITE 1

struct player{char name[BUFFERSIZE];int lives;int sd;};

struct player* create_player(char *name, int sd){
    struct player *p = malloc(sizeof(struct player));
    strcpy(p->name, name);
    p->lives = 2;
    p->sd = sd;
    return p;
}

// void err(int i, char*message){
//   if(i < 0){
// 	  printf("Error: %s - %s\n",message, strerror(errno));
//   	exit(1);
//   }
// }

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
returns 1 if word doesn't exist
returns 0 of word exists
*/
int parse(char word[]){
    char * cmdargv[64];
    char front[100] = "grep ";
    char back[] = " words_alpha.txt -w";

    int usedWords = open("usedWords.txt" , O_CREAT|O_RDWR,644 );
    // printf("String: %s\n",front);
    strcat(front,word);
    // printf("String: %s\n",front);
    strcat(front,back);
    // printf("String: %s\n",front);
    parse_args(front,cmdargv);
    
    char grepReturn[100];
    execvp(cmdargv[0],cmdargv);
    // fgets(grepReturn, 100, stdout);
    int sOut = open("/dev/stdout",100,O_RDONLY);
    int i = read(sOut,grepReturn,100);
    err(i, "fgets fails");
    printf("returned val: %s\n", grepReturn);
    return 0;
}

void write_all(struct player** ps, char * buff){
    for (int x = 0; x < MAX_CLIENTS; x++){
        if(ps[x] != NULL){
            int b = write(ps[x]->sd, buff, BUFFER_SIZE);
            if (b == -1) err(16, "server write broke");
        }
    }
}

//---------------------------------------------------------------------------------------------
// int main(int argc, char * argv[]){
    
//     printf("Tests:\n");
//     char c[] = "apply";
//     int i;
//     i = parse(c);
//     // printf("returned: %d\n", i);
// }

void help(struct player *p){ //show all commands
    char buff[BUFFER_SIZE] = "|| Commands:\n";
    strcat(buff, "|| /help : show all commands\n");
    strcat(buff, "|| /start : start the Word Bomb game");
    write(p->sd, buff, BUFFER_SIZE);
}

void start_game(struct player **ps, int* game_status){ //starts the game
    *game_status = 1; //change to true
    int f = fork();
    if (f==0){
        char buff[BUFFER_SIZE] = "Game is starting.";
        write_all(ps, buff);
        for (int x = 0; x < 5; x++){
            sleep(1);
        }
        sprintf(buff, "Game has ended.");
        write_all(ps, buff);
    }
    else{
        int status;
        wait(&status);
        *game_status = 0; //change to false
    }
    
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
        else start_game(ps, game_status);
    }
    else write(p->sd, buff, sizeof(buff));
}
