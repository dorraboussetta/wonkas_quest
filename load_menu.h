#ifndef LOAD_MENU_H_INCLUDED
#define LOAD_MENU_H_INCLUDED

#include "common.h"

typedef struct
{
    Background bg;
    Button load_btn;
    Button new_btn;
    Text title;
} LoadMenu;

void init_load_menu(LoadMenu *menu, SDL_Renderer *renderer);
void handle_load_menu_input(LoadMenu *menu, SDL_Event event, int *state);
void update_load_menu(LoadMenu *menu);
void display_load_menu(LoadMenu menu, SDL_Renderer *renderer);
void free_load_menu(LoadMenu *menu);

#endif

