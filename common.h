#ifndef COMMON_H_INCLUDED
#define COMMON_H_INCLUDED

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <string.h>

#define SCREEN_WIDTH  1920
#define SCREEN_HEIGHT 1080

typedef struct {
    TTF_Font        *font;
    SDL_Color        color;
    float            size;
    SDL_Rect         pos;
    char             text[100];
    SDL_Surface     *txt_image;
    SDL_Texture     *txt_text;
} Text;

typedef struct {
    SDL_Texture *img[3];
    int          current_img;
    SDL_Rect     pos;
    Mix_Chunk   *click_sound;
    Mix_Chunk   *hover_sound;
    int          action;
    /* ADDED: flag set to 1 when this button is keyboard-selected,
       prevents update_button from resetting current_img to idle */
    int          keyboard_selected;
} Button;

typedef struct {
    SDL_Texture *img[30];
    int          current_img;
    Mix_Music   *bg_sound;
    Text         txt;
    SDL_Texture *logo;
} Background;

#endif // COMMON_H_INCLUDED
