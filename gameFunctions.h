#ifndef GAMEFUNCTIONS_H
#define GAMEFUNCTIONS_H
struct player{char name[100];int lives;int sd;};
char * randPrompt();
struct player* create_player(char *name, int sd);
void parse_args(char * line, char * args[]);
int parse(char word[]);
int cur_players(struct player** ps);
int next_player_index(int cur_player_index, struct player **ps);
void write_all(struct player** ps, char * buff);
void command_logic(struct player **ps, struct player *p, char* line, int* gs_pointer);
void help(struct player *p);
void start_game(struct player **ps, int* gs_pointer);
#endif