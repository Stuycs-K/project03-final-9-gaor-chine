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
    int f = fork();
    if(f == 0){
        char front[100] = "grep ";
        char back[] = " words_alpha.txt -w";
        strcat(front,word);
        strcat(front,back);
        parse_args(front,cmdargv);
        int ff = open("checkFile", O_CREAT,0666);
        int file = open("checkFile",O_RDONLY|O_WRONLY|O_TRUNC,0666);err(file,"line 55");
        fflush(stdout);
        int backup_stdout = dup(STDOUT_FILENO); err(backup_stdout,"line 57");
        int a = dup2(file, STDOUT_FILENO); err(a,"line 58");
        int b = close(file); err(b,"line 59");
        int c = execvp(cmdargv[0],cmdargv);err(c,"line 60");
        int d = dup2(backup_stdout,STDOUT_FILENO);err(d,"line 61");
        close(file);
        exit(0);
    }else{
        char string[100];
        int status;
        wait(&status);
        // sleep(1);
        FILE * file = fopen("checkFile","r");//err(file,"line 65");
        // int file = open("checkFile",O_RDONLY,0644);//err(file,"line 65");
        // int usedWords = open("usedWords", O_CREAT|O_RDONLY|O_WRONLY); err(usedWords,"line 51");
        // close(usedWords);
        // int s = read(file,string,100); err(s,"line 68");
        // fgets(string,100,file);
        printf("read from file: %s\n",string);
    }
    return 0;
}
//---------------------------------------------------------------------------------------------
int main(int argc, char * argv[]){
    
    printf("Tests:\n");
    char c[] = "yes";
    int i;
    char string[100];
    i = parse(c);
    // printf("returned: %d\n", i);
    // int f = open("test.txt",O_RDONLY);
    // read(f,string,100);
    // printf("test: %s\n",string);
}


