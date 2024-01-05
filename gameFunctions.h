#ifndef GAMEFUNCTIONS_H
#define GAMEFUNCTIONS_H
struct player{char name[100];int lives;};
void err(int i, char*message);
void parse_args(char * line, char * args[]);
int parse(char word[]);
#endif