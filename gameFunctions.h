#include <sys/time.h>
#include "networking.h"
#define NAME_SIZE 100

#ifndef GAMEFUNCTIONS_H
#define GAMEFUNCTIONS_H
struct player{char name[NAME_SIZE];int lives;int sd;};
char * randPrompt();
struct player* create_player(char *name, int sd);
void parse_args(char * line, char * args[]);
int parse(char *word);
int cur_players(struct player** ps);
int next_player_index(int cur_player_index, struct player **ps);
void write_all(struct player** ps, char * buff);
void chat_logic(struct player** ps, struct player* p, char* line, int* game_status);
void command_logic(struct player **ps, struct player *p, char* line, int* temp_game_status, int* game_status);
void help(struct player *p);
void start_game(struct player **ps, int* gs_pointer);
void check_logic(struct player **ps, struct player *sent_p, int *temp_cur_p, int *cur_p, char* line,
                int* game_status, struct timeval *timeout, char* prompt);
#endif