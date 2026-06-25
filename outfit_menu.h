#ifndef OUTFIT_MENU_H
#define OUTFIT_MENU_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>

#define WIN_W 1920
#define WIN_H 1080

typedef struct {
    SDL_Texture *img[2];
    SDL_Rect     pos;
    int          action;
}button_om;

typedef struct {
    SDL_Texture *img;
    Mix_Music   *bg_sound;
}background;

typedef struct{
    SDL_Texture *buttons[2];
    SDL_Rect     pos[2];
    Mix_Chunk   *hoverSound;
    Mix_Chunk   *selectSound;
    int          outfit;   /* -1 = none selected, 0 or 1 */
    int          player;   /* 0 = wonka, 1 = noodle */
    int          hovered;  /* -1 = none, 0 or 1 */
} player_outfit;

typedef struct {
    background bg;
    button_om     b[2]; // button 0 validate, button 1 return
    TTF_Font *font;
} outfit_menu;

void init_outfit_menu(SDL_Renderer *renderer, player_outfit *po, outfit_menu *om, int num_players, int selected_player, int level);
void display_outfit_menu(SDL_Renderer *renderer, player_outfit *po, outfit_menu *om, int num_players);
void handle_outfit_menu(player_outfit *po, SDL_Event event, outfit_menu *om, int num_players, int *interface);
void free_outfit_menu(player_outfit *po, outfit_menu *om, int num_players);

#endif
