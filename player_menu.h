#ifndef PLAYER_MENU_H
#define PLAYER_MENU_H
#include "common.h"
//player_mode

#define WIN_W 1920
#define WIN_H 1080
typedef struct
{
	Button mode_buttons[3];
	Text mode_text;
	Background mode_bg;
	
	Mix_Chunk *hoverSound;  
    	Mix_Chunk *selectSound;
    	
    	int num_players;
}player_mode;


void init_player_mode(player_mode *pm,SDL_Renderer *renderer);
void update_player_mode(player_mode *pm);
void handle_player_mode_input(player_mode *pm,SDL_Event event,int *interface);
void display_player_mode(player_mode *pm,SDL_Renderer *renderer);
int player_mode_click(player_mode *pm, SDL_Event *event);
void free_player_mode(player_mode *pm);



//player_selection
typedef struct
{
	Button selec_buttons[6];
	Text selec_text;
	Background selec_bg;
	Mix_Chunk *hoverSound;  
    	Mix_Chunk *selectSound;
    	
    	int player;
}player_selection;


void init_player_selection(player_selection *ps,SDL_Renderer *renderer);
void update_player_selection(player_selection *ps);
void handle_player_selection_input(player_selection *ps,SDL_Event event,int *interface);
void display_player_selection(player_selection *ps,SDL_Renderer *renderer);
void free_player_selection(player_selection *ps);

#endif
