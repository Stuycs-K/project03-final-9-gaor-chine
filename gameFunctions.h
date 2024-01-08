#ifndef GAMEFUNCTIONS_H
#define GAMEFUNCTIONS_H
struct player{char name[100];int lives;int sd;};
struct player* create_player(char *name, int sd);
void parse_args(char * line, char * args[]);
int parse(char word[]);
void command_logic(struct player *p, char* line);
void help(struct player *p);
#endif