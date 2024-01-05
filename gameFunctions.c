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
struct player{char name[BUFFERSIZE];int lives;};

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
    // int wordLen = strlen(word);
    // char string[wordLen];
    // strncpy(string,word,wordLen);
    // printf("Trimmed string: %s\n",string);
    // word[strlen(word)-1] = 0;
    char front[] = "grep ";
    strcat(front,word);
    char back[] = " words_alpha.txt -w";
    strcat(front,back);
    printf("String: %s\n",front);
    char * cmdargv[64];
    parse_args(front,cmdargv);
    execvp(cmdargv[0],cmdargv);
    return 0;
}
//---------------------------------------------------------------------------------------------
int main(int argc, char * argv[]){
    
    printf("Tests:\n");
    char c[] = "yes";
    int i;
    i = parse(c);
    printf("returned: %d\n", i);
}


