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
struct player{char name[BUFFERSIZE];int lives};

/*
parses words_alpha.txt for the inputed word
returns 1 if word doesn't exist
returns 0 of word exists
*/
int parse(char word[BUFFERSIZE]){
    int wordLen = strlen(word);
    char string[wordLen];
    strncpy(string,word,wordLen);
    printf("Trimmed string: %s\n",string);
    return 0;

}
//---------------------------------------------------------------------------------------------
int main(int argc, char * argv[]){
    printf("Tests:\n");
    char b[] = {'g', 'o', 'o', 'd', 'b', 'y', 'e', 0};
    parse(b);
}


