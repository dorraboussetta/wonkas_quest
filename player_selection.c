#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL_ttf.h>
#include "common.h"
#include "player_menu.h"
#include <string.h>

void init_player_selection(player_selection *ps, SDL_Renderer *renderer)
{
    ps->hoverSound = Mix_LoadWAV("assets_player_menu/hover.wav");
    ps->selectSound = Mix_LoadWAV("assets_player_menu/select.wav");

    ps->selec_bg.current_img = 0;
    ps->selec_bg.img[0] = IMG_LoadTexture(renderer, "assets_player_menu/player_menu_bg.png");

    strcpy(ps->selec_text.text, "choose a character");
    ps->selec_text.pos.x = 400;
    ps->selec_text.pos.y = 50;
    ps->selec_text.font = TTF_OpenFont("assets_player_menu/Pixel Game.otf", 140);
    ps->selec_text.color = (SDL_Color){255,255,255};
    ps->selec_text.txt_image = TTF_RenderText_Blended(ps->selec_text.font, ps->selec_text.text, ps->selec_text.color);
    ps->selec_text.pos.w = ps->selec_text.txt_image->w;
    ps->selec_text.pos.h = ps->selec_text.txt_image->h;
    ps->selec_text.txt_text = SDL_CreateTextureFromSurface(renderer, ps->selec_text.txt_image);

    ps->selec_buttons[0].img[0] = IMG_LoadTexture(renderer, "assets_player_menu/ps_1.png");
    ps->selec_buttons[0].img[1] = IMG_LoadTexture(renderer, "assets_player_menu/ps_1_hover.png");
    ps->selec_buttons[0].pos.x = 550;
    ps->selec_buttons[0].pos.y = 160;
    ps->selec_buttons[0].action = 1;
    ps->selec_buttons[0].pos.w = 258;
    ps->selec_buttons[0].pos.h = 274;

    ps->selec_buttons[1].img[0] = IMG_LoadTexture(renderer, "assets_player_menu/ps_2.png");
    ps->selec_buttons[1].img[1] = IMG_LoadTexture(renderer, "assets_player_menu/ps_2_hover.png");
    ps->selec_buttons[1].pos.x = 1000;
    ps->selec_buttons[1].pos.y = 160;
    ps->selec_buttons[1].action = 2;
    ps->selec_buttons[1].pos.w = 258;
    ps->selec_buttons[1].pos.h = 274;

    ps->selec_buttons[2].img[0] = IMG_LoadTexture(renderer, "assets_player_menu/key_mode.png");
    ps->selec_buttons[2].img[1] = IMG_LoadTexture(renderer, "assets_player_menu/key_mode_hover.png");
    ps->selec_buttons[2].pos.x = 550;
    ps->selec_buttons[2].pos.y = 440;
    ps->selec_buttons[2].action = 3;
    ps->selec_buttons[2].pos.w = 258;
    ps->selec_buttons[2].pos.h = 274;

    ps->selec_buttons[3].img[0] = IMG_LoadTexture(renderer, "assets_player_menu/controller_mode.png");
    //ps->selec_buttons[3].img[1] = IMG_LoadTexture(renderer, "assets_player_menu/controller_mode_hover.png");
    ps->selec_buttons[3].img[1] = IMG_LoadTexture(renderer, "assets_player_menu/contoller_mode_h.png");
    ps->selec_buttons[3].pos.x = 1000;
    ps->selec_buttons[3].pos.y = 440;
    ps->selec_buttons[3].action = 4;
    ps->selec_buttons[3].pos.w = 258;
    ps->selec_buttons[3].pos.h = 274;

    ps->selec_buttons[4].img[0] = IMG_LoadTexture(renderer, "assets_player_menu/pl_validate.png");
    ps->selec_buttons[4].img[1] = IMG_LoadTexture(renderer, "assets_player_menu/pl_validate_hover.png");
    ps->selec_buttons[4].pos.x = 650;
    ps->selec_buttons[4].pos.y = 735;
    ps->selec_buttons[4].action = 5;
    ps->selec_buttons[4].pos.w = 505;
    ps->selec_buttons[4].pos.h = 189;

    ps->selec_buttons[5].img[0] = IMG_LoadTexture(renderer, "assets_player_menu/ps_return.png");
    ps->selec_buttons[5].img[1] = IMG_LoadTexture(renderer, "assets_player_menu/ps_return_hover.png");
    ps->selec_buttons[5].pos.x = 1400;
    ps->selec_buttons[5].pos.y = 800;
    ps->selec_buttons[5].action = 6;
    ps->selec_buttons[5].pos.w = 380;
    ps->selec_buttons[5].pos.h = 141;
    
    ps->player=0;
}

void handle_player_selection_input(player_selection *ps, SDL_Event event, int *interface)
{
    if(event.type == SDL_MOUSEBUTTONDOWN)
    {
        int mouseX = event.button.x;
        int mouseY = event.button.y;

        for(int i = 0; i < 6; i++)
        {
            if(mouseX >= ps->selec_buttons[i].pos.x &&
               mouseX <= ps->selec_buttons[i].pos.x + ps->selec_buttons[i].pos.w &&
               mouseY >= ps->selec_buttons[i].pos.y &&
               mouseY <= ps->selec_buttons[i].pos.y + ps->selec_buttons[i].pos.h)
            {
                Mix_PlayChannel(-1, ps->selectSound, 0);
                if(ps->selec_buttons[i].action == 1) ps->player = 0;
		if(ps->selec_buttons[i].action == 2) ps->player = 1;
                if(ps->selec_buttons[i].action == 5)
                    *interface = 6;
                else if(ps->selec_buttons[i].action == 6)
                    *interface = 0;
            }
        }
    }

    if(event.type == SDL_KEYDOWN)
    {
        if(event.key.keysym.sym == SDLK_RETURN)
        {
            Mix_PlayChannel(-1, ps->selectSound, 0);
            *interface = 6;
        }
    }
}

void update_player_selection(player_selection *ps)
{
    int mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);
    static int lastHovered = -1;
    int currentHovered = -1;

    for(int i = 0; i < 6; i++)
    {
        if(mouseX >= ps->selec_buttons[i].pos.x &&
           mouseX <= ps->selec_buttons[i].pos.x + ps->selec_buttons[i].pos.w &&
           mouseY >= ps->selec_buttons[i].pos.y &&
           mouseY <= ps->selec_buttons[i].pos.y + ps->selec_buttons[i].pos.h)
        {
            currentHovered = i;
        }
    }
    if(currentHovered != lastHovered && currentHovered != -1)
        Mix_PlayChannel(-1, ps->hoverSound, 0);
    lastHovered = currentHovered;
}

void display_player_selection(player_selection *ps, SDL_Renderer *renderer)
{
    int mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);

    SDL_RenderCopy(renderer, ps->selec_bg.img[ps->selec_bg.current_img], NULL, NULL);
    SDL_RenderCopy(renderer, ps->selec_text.txt_text, NULL, &ps->selec_text.pos);

    for(int i = 0; i < 6; i++)
    {
        int over = (mouseX >= ps->selec_buttons[i].pos.x &&
                    mouseX <= ps->selec_buttons[i].pos.x + ps->selec_buttons[i].pos.w &&
                    mouseY >= ps->selec_buttons[i].pos.y &&
                    mouseY <= ps->selec_buttons[i].pos.y + ps->selec_buttons[i].pos.h);
        SDL_RenderCopy(renderer, ps->selec_buttons[i].img[over ? 1 : 0], NULL, &ps->selec_buttons[i].pos);
    }
}

void free_player_selection(player_selection *ps)
{
    SDL_DestroyTexture(ps->selec_bg.img[0]);
    for(int i = 0; i < 6; i++)
    {
        SDL_DestroyTexture(ps->selec_buttons[i].img[0]);
        SDL_DestroyTexture(ps->selec_buttons[i].img[1]);
    }
    Mix_FreeChunk(ps->hoverSound);
    Mix_FreeChunk(ps->selectSound);
    SDL_DestroyTexture(ps->selec_text.txt_text);
    SDL_FreeSurface(ps->selec_text.txt_image);
    TTF_CloseFont(ps->selec_text.font);
}
