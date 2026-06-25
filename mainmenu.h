#ifndef MAINMENU_H_INCLUDED
#define MAINMENU_H_INCLUDED
#include "common.h"



typedef struct {
	Background bg; 
	Button buttons[5]; 

}MainMenu;


int SDL_Systems_init(); 
SDL_Window* create_window(char title[], int width, int height); 
SDL_Renderer* create_renderer(SDL_Window *window); 

int init_bg_main_menu(SDL_Renderer *renderer, Background *bg); 
int init_buttons_main_menu(SDL_Renderer *renderer, Button *buttons); 
int init_main_menu(SDL_Renderer *renderer, MainMenu *main_menu);

SDL_Texture* init_sub_menu(SDL_Renderer *renderer); //to remove after integration
SDL_Texture* init_highscore_menu(SDL_Renderer *renderer); //to remove after integration
SDL_Texture* init_history_menu(SDL_Renderer *renderer); //to remove after integration
 
void display_buttons_main_menu(SDL_Renderer *renderer, MainMenu main_menu, int num_button_modified); 
void display_main_menu(SDL_Renderer *renderer, MainMenu main_menu, int num_button_modified); 

void display_sub_menu(SDL_Renderer *renderer, SDL_Texture *sub_img);  //to remove after integration
void display_highscore_menu(SDL_Renderer *renderer, SDL_Texture *highscore_img); //to remove after integration
void display_history_menu(SDL_Renderer *renderer, SDL_Texture *history_img); //to remove after integration

void event_main_menu(MainMenu main_menu, int *click_time, int *interface, int *click, int *motion, int *mx, int *my, int *num_button_modified, SDL_Event event, int *last_hover);  

void update_bg_main_menu(MainMenu *main_menu); 
void update_buttons_main_menu(MainMenu *main_menu, int click_time, int *click, int motion, int num_button_modified); 
void update_main_menu(MainMenu *main_menu, int click_time, int *running, int *click, int motion, int interface, int num_button_modified); 

void free_main_menu(MainMenu main_menu); 

#endif // MAINMENU_H_INCLUDED
