#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL_ttf.h>
#include "common.h"
#include "player_menu.h"
#include <string.h>

void init_player_mode(player_mode *pm, SDL_Renderer *renderer)
{
    pm->hoverSound = Mix_LoadWAV("assets_player_menu/hover.wav");
    pm->selectSound = Mix_LoadWAV("assets_player_menu/select.wav");

    pm->mode_bg.current_img = 0;
    pm->mode_bg.img[0] = IMG_LoadTexture(renderer, "assets_player_menu/player_menu_bg.png");

    strcpy(pm->mode_text.text, "choose a player mode");
    pm->mode_text.pos.x = 400;
    pm->mode_text.pos.y = 50;
    pm->mode_text.font = TTF_OpenFont("assets_player_menu/Pixel Game.otf", 140);
    pm->mode_text.color = (SDL_Color){255,255,255};
    pm->mode_text.txt_image = TTF_RenderText_Blended(pm->mode_text.font, pm->mode_text.text, pm->mode_text.color);
    pm->mode_text.pos.w = pm->mode_text.txt_image->w;
    pm->mode_text.pos.h = pm->mode_text.txt_image->h;
    pm->mode_text.txt_text = SDL_CreateTextureFromSurface(renderer, pm->mode_text.txt_image);

    pm->mode_buttons[0].img[0] = IMG_LoadTexture(renderer, "assets_player_menu/singleplayer.png");
    pm->mode_buttons[0].img[1] = IMG_LoadTexture(renderer, "assets_player_menu/singleplayer_hover.png");
    pm->mode_buttons[0].pos.x = 87;
    pm->mode_buttons[0].pos.y = 310;
    pm->mode_buttons[0].action = 1;
    pm->mode_buttons[0].pos.w = 757;
    pm->mode_buttons[0].pos.h = 284;

    pm->mode_buttons[1].img[0] = IMG_LoadTexture(renderer, "assets_player_menu/multiplayer.png");
    pm->mode_buttons[1].img[1] = IMG_LoadTexture(renderer, "assets_player_menu/multiplayer_hover.png");
    pm->mode_buttons[1].pos.x = 1000;
    pm->mode_buttons[1].pos.y = 310;
    pm->mode_buttons[1].action = 2;
    pm->mode_buttons[1].pos.w = 757;
    pm->mode_buttons[1].pos.h = 284;

    pm->mode_buttons[2].img[0] = IMG_LoadTexture(renderer, "assets_player_menu/pm_return.png");
    pm->mode_buttons[2].img[1] = IMG_LoadTexture(renderer, "assets_player_menu/pm_return_hover.png");
    pm->mode_buttons[2].pos.x = 1250;
    pm->mode_buttons[2].pos.y = 750;
    pm->mode_buttons[2].action = 6;
    pm->mode_buttons[2].pos.w = 505;
    pm->mode_buttons[2].pos.h = 189;
    
    pm->num_players=1;
}

void handle_player_mode_input(player_mode *pm, SDL_Event event, int *interface)
{
    if(event.type == SDL_MOUSEBUTTONDOWN)
    {
        int mouseX = event.button.x;
        int mouseY = event.button.y;

        for(int i = 0; i < 3; i++)
        {
            if(mouseX >= pm->mode_buttons[i].pos.x &&
               mouseX <= pm->mode_buttons[i].pos.x + pm->mode_buttons[i].pos.w &&
               mouseY >= pm->mode_buttons[i].pos.y &&
               mouseY <= pm->mode_buttons[i].pos.y + pm->mode_buttons[i].pos.h)
            {
                Mix_PlayChannel(-1, pm->selectSound, 0);
                if(pm->mode_buttons[i].action == 1){
		    pm->num_players = 1;
		    *interface = 5;
		} else if(pm->mode_buttons[i].action == 2){
		    pm->num_players = 2;
		    *interface = 5;
		} else if(pm->mode_buttons[i].action == 6)
		    *interface = 0;
            }
        }
    }
}

void update_player_mode(player_mode *pm)
{
    int mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);
    static int lastHovered = -1;
    int currentHovered = -1;

    for(int i = 0; i < 3; i++)
    {
        if(mouseX >= pm->mode_buttons[i].pos.x &&
           mouseX <= pm->mode_buttons[i].pos.x + pm->mode_buttons[i].pos.w &&
           mouseY >= pm->mode_buttons[i].pos.y &&
           mouseY <= pm->mode_buttons[i].pos.y + pm->mode_buttons[i].pos.h)
        {
            currentHovered = i;
        }
    }
    if(currentHovered != lastHovered && currentHovered != -1)
        Mix_PlayChannel(-1, pm->hoverSound, 0);
    lastHovered = currentHovered;
}

void display_player_mode(player_mode *pm, SDL_Renderer *renderer)
{
    int mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);

    SDL_RenderCopy(renderer, pm->mode_bg.img[pm->mode_bg.current_img], NULL, NULL);
    SDL_RenderCopy(renderer, pm->mode_text.txt_text, NULL, &pm->mode_text.pos);

    for(int i = 0; i < 3; i++)
    {
        int over = (mouseX >= pm->mode_buttons[i].pos.x &&
                    mouseX <= pm->mode_buttons[i].pos.x + pm->mode_buttons[i].pos.w &&
                    mouseY >= pm->mode_buttons[i].pos.y &&
                    mouseY <= pm->mode_buttons[i].pos.y + pm->mode_buttons[i].pos.h);
        SDL_RenderCopy(renderer, pm->mode_buttons[i].img[over ? 1 : 0], NULL, &pm->mode_buttons[i].pos);
    }
}

void free_player_mode(player_mode *pm)
{
    SDL_DestroyTexture(pm->mode_bg.img[0]);
    for(int i = 0; i < 3; i++)
    {
        SDL_DestroyTexture(pm->mode_buttons[i].img[0]);
        SDL_DestroyTexture(pm->mode_buttons[i].img[1]);
    }
    Mix_FreeChunk(pm->hoverSound);
    Mix_FreeChunk(pm->selectSound);
    SDL_DestroyTexture(pm->mode_text.txt_text);
    SDL_FreeSurface(pm->mode_text.txt_image);
    TTF_CloseFont(pm->mode_text.font);
}
