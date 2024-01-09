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

void start_game(struct player **ps){ //starts the game
    char buff[BUFFER_SIZE] = "Game is starting.\n";
    write_all(ps, buff);
}

void command_logic(struct player **ps; struct player *p, char* line){
    char * cmdargv[64];
    char buff[100] = "command does not exist.";
    parse_args(line, cmdargv);
    //printf("%s\n", cmdargv[0]);
    if (strcmp(cmdargv[0], "/help") == 0) help(p);
    if (strcmp(cmdargv[0], "/start") == 0) start_game(ps);
    else write(p->sd, buff, sizeof(buff));
}
