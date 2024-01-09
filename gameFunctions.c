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

#define BUFFERSIZE 100
#define READ 0
#define WRITE 1
// #define WRITE 1

struct player{char name[BUFFERSIZE];int lives;};

void err(int i, char*message){
  if(i < 0){
	  printf("Error: %s - %s\n",message, strerror(errno));
  	exit(1);
  }
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
    char * cmdargv[64];
    int fds[2];
    int pid = getpid();
    pipe(fds);
    int f = fork();
    if(f == 0){
        close(fds[READ]);
        char front[100] = "grep ";
        char back[] = " words_alpha.txt -w";
        int usedWords = open("usedWords.txt" , O_CREAT|O_RDWR,644);
        strcat(front,word);
        strcat(front,back);
        parse_args(front,cmdargv);
        int file = open("checkFile.txt", O_CREAT|O_RDWR,666);
        fflush(stdout);
        int backup_stdout = dup(STDOUT_FILENO);
        dup2(file, STDOUT_FILENO);
        close(file);
        execvp(cmdargv[0],cmdargv);
        dup2(backup_stdout,STDOUT_FILENO);
        exit(0);
    }else{
        close(fds[WRITE]);
        char string[100];
        read(fds[READ],string,100);
        printf("output from stdout: %s\n",string);
        int file = open("checkfile.txt",666);
        read(file,string,100);
        printf("read from file: %s\n",string);
    }
    return 0;
}
//---------------------------------------------------------------------------------------------
int main(int argc, char * argv[]){
    
    printf("Tests:\n");
    char c[] = "yes";
    int i;
    i = parse(c);
    // printf("returned: %d\n", i);
}


