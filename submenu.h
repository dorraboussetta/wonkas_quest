#ifndef SUBMENU_H
#define SUBMENU_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <stdio.h>

typedef struct {
    SDL_Texture *texture;
    SDL_Rect rect, dest;
} image;

typedef struct {
    SDL_Texture *texture;
    SDL_Rect rect, static_dest, hover, final_dest;
} button;

int  init_option_menu(SDL_Renderer *renderer, image *bg, image *text, button *btn, Mix_Music **music, Mix_Chunk **effect);
void event_options(SDL_Event event, button *btn, Mix_Music *music, Mix_Chunk *effect, SDL_Window *window, int *volume, int *interface, int *prev_interface);
void update_options_menu(SDL_Renderer *renderer, image bg, image *text, button *btn);
void display_options_menu(SDL_Renderer *renderer);
void free_options_menu(image bg, button *btn, image *text, Mix_Music *music, Mix_Chunk *effect);

#endif
