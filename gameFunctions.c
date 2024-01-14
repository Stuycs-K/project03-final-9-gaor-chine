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
char * randPrompt(){
    char *buff = malloc(5);
    char str[100];
    int rfile = open("/dev/urandom",O_RDONLY,444);
    int i;
    read(rfile, &i, sizeof(int));
    i = i % 1305;
    if(i < 0) i *= -1;
    FILE * prompts = fopen("prompts.txt", "r");
    for(int j = 0; j < i; j++){
        fgets(str,100,prompts);
    }
    fgets(buff,5,prompts);
    stripNewLine(buff);
    // printf("randomized prompt: %s\n",s);
    return buff;
}

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
returns 2 if word does not exist
returns 1 if word exists but is used
returns 0 of word exists
*/
int parse(char *word){
    char cpy[100] = "";
    strcpy(cpy, word);
    char * cmdargv[64];
    int len = strlen(word);
    printf("size: %d\n", len);
    int f = fork();
    if(f == 0){
        char front[256] = "grep ";
        char back[] = " words_alpha.txt -w";
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
        // printf("read: %s\n",wds);
        strcat(cpy,"\n");
        while(fgets(wds,100,usedWords)!= NULL){
            if(strcmp(wds, cpy) == 0) return 1;
        }
        int uWords = open("usedWords",O_APPEND|O_WRONLY,0644);
        write(uWords,cpy,strlen(cpy));
        printf("added %s to usedWords\n",cpy);
        close(uWords);
        close(file);
        close(s);
        close(l);
    }
    return 0;
}
//---------------------------------------------------------------------------------------------
// int main(int argc, char * argv[]){
//     // printf("Tests:\n");
//     // int i;
//     // char a[]="hi";
//     // char b[]="asjdfhajsf";
//     // char c[]="hi";
//     // char d[]="yes";
//     // i = parse(a);
//     // printf("returned: %d\n", i);
//     // printf("--------------------------\n");
//     // i = parse(b);
//     // printf("returned: %d\n", i);
//     // printf("--------------------------\n");
//     // i = parse(c);
//     // printf("returned: %d\n", i);
//     // printf("--------------------------\n");
//     // i = parse(d);
//     // printf("returned: %d\n", i);
//     randPrompt();
// }

int cur_players(struct player **ps){
    int cur_clients = 0;
    for (int x = 0; x < MAX_CLIENTS; x++){
        if (ps[x] != NULL) {
            printf("%s, ", ps[x]->name);
            cur_clients++;
        }
        else printf("0, ");
    }
    return cur_clients;
}

int next_player_index(int cur_player_index, struct player **ps){
    for (int x = cur_player_index + 1; x < MAX_CLIENTS; x++){
        if (ps[x] != NULL) return x;
    }
    for (int x = 0; x < cur_player_index; x++){ //loop back around
        if (ps[x] != NULL) return x;
    }
}

void write_all(struct player** ps, char * buff){
    char temp[BUFFER_SIZE] = "";
    strcpy(temp, buff);
    for (int x = 0; x < MAX_CLIENTS; x++){
        if(ps[x] != NULL){
            int b = write(ps[x]->sd, temp, BUFFER_SIZE);
            if (b == -1) err(16, "server write broke");
        }
    }
}

void help(struct player *p){ //show all commands
    char buff[BUFFER_SIZE] = "|| Commands:\n";
    strcat(buff, "|| /help : show all commands\n");
    strcat(buff, "|| /start : start the Word Bomb game");
    //printf("%s", buff);
    write(p->sd, buff, BUFFER_SIZE);
}

void start_game(struct player **ps, int* game_status){ //starts the game
    *game_status = 1; //change to true
    int l = open("usedWords", O_CREAT|O_TRUNC,0666);
    for (int x = 0; x < MAX_CLIENTS; x++){
        if(ps[x] != NULL) ps[x]->lives = 2;
    }
    char buff[BUFFER_SIZE] = "|| Game is starting.";
    write_all(ps, buff);
    close(l);
}

void command_logic(struct player **ps, struct player *p, char* line, int* game_status){
    printf("game_status: %d\n", *game_status);
    char * cmdargv[64];
    char buff[BUFFER_SIZE] = "|| Command does not exist.";
    parse_args(line, cmdargv);
    //printf("%s\n", cmdargv[0]);
    if (strcmp(cmdargv[0], "/help") == 0) help(p);
    else if (strcmp(cmdargv[0], "/start") == 0){
        if(*game_status == 1){
            sprintf(buff, "|| Game is in progress.");
            write(p->sd, buff, BUFFER_SIZE);
        }
        else if (cur_players(ps) < 2){
            sprintf(buff, "|| Game requires at least 2 players. Current number of players: %d", cur_players(ps));
            write(p->sd, buff, BUFFER_SIZE);
        }
        else start_game(ps, game_status);
    }
    else write(p->sd, buff, BUFFER_SIZE);
}

void check_logic(struct player **ps, struct player *sent_p, int *temp_cur_p, int *cur_p, char* line,
                int* game_status, struct timeval *timeout, char* prompt){
    char temp[BUFFER_SIZE] = "";
    int check = parse(line);
    char reply[BUFFER_SIZE] = "";
    if (check == 0){
        sprintf(temp, "|| thats right!\n");
        timeout->tv_sec = 10;
        timeout->tv_usec = 0;
        *temp_cur_p = next_player_index(*cur_p, ps);
        strcpy(prompt, randPrompt());
        sprintf(reply, "|| It is %s's turn.\n|| The prompt is: %s", ps[*temp_cur_p]->name, prompt);
        strcat(temp, reply);
        write_all(ps, temp);
    }else if(check == 1){
        sprintf(reply, "|| word has already been used, try again!");
        write(ps[*cur_p]->sd, reply, BUFFER_SIZE);
    }else if(check == 2){
        sprintf(reply, "|| word doesn't exist, try again!");
        write(ps[*cur_p]->sd, reply, BUFFER_SIZE);
    }

}


