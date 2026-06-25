#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL_ttf.h>
#include "common.h"
#include "player_menu.h"
#include <string.h>
//void init_test()







void init_player_mode(player_mode *pm,SDL_Renderer *renderer)
{
    //sfx
    pm->hoverSound = Mix_LoadWAV("hover.wav");
    pm->selectSound = Mix_LoadWAV("select.wav");
    
    // Background
    pm->mode_bg.current_img = 0;
    pm->mode_bg.img[0] = IMG_LoadTexture(renderer, "player_menu_bg.jpeg");
    // Title text
    strcpy(pm->mode_text.text, "choose a player mode");
    pm->mode_text.pos.x = 250;
    pm->mode_text.pos.y = 50;
    pm->mode_text.font = TTF_OpenFont("Pixel Game.otf", 80);
    pm->mode_text.color = (SDL_Color){255,255,255};
    pm->mode_text.txt_image = TTF_RenderText_Blended(pm->mode_text.font,pm->mode_text.text,pm->mode_text.color);
    pm->mode_text.pos.w = pm->mode_text.txt_image->w;
    pm->mode_text.pos.h = pm->mode_text.txt_image->h;
    pm->mode_text.txt_text = SDL_CreateTextureFromSurface(renderer,pm->mode_text.txt_image);
    
    // Button 1 (Single Player)
    pm->mode_buttons[0].img[0] = IMG_LoadTexture(renderer, "pm_bt_1.jpeg");
    pm->mode_buttons[0].img[1] = IMG_LoadTexture(renderer, "pm_bt_1_hover.jpeg");  // hover
    pm->mode_buttons[0].pos.x = 87;
    pm->mode_buttons[0].pos.y = 310;
    pm->mode_buttons[0].action = 1;
    pm->mode_buttons[0].pos.w = 378;
    pm->mode_buttons[0].pos.h = 85;
    



    // Button 2 (Multiplayer)
    pm->mode_buttons[1].img[0] = IMG_LoadTexture(renderer, "pm_bt_2.jpeg");
    pm->mode_buttons[1].img[1] = IMG_LoadTexture(renderer, "pm_bt_2_hover.jpeg");  // hover
    pm->mode_buttons[1].pos.x = 640;
    pm->mode_buttons[1].pos.y = 310;
    pm->mode_buttons[1].action = 2;
    pm->mode_buttons[1].pos.w = 378;
    pm->mode_buttons[1].pos.h = 85;
    
    //button 3 (return)
    pm->mode_buttons[2].img[0] = IMG_LoadTexture(renderer, "test.jpeg");
    pm->mode_buttons[2].img[1] = IMG_LoadTexture(renderer, "test_hover.jpeg");  // hover
    pm->mode_buttons[2].pos.x = 850;
    pm->mode_buttons[2].pos.y = 520;
    pm->mode_buttons[2].action = 6;
    pm->mode_buttons[2].pos.w = 189;
    pm->mode_buttons[2].pos.h = 42;
    for(int j=0;j<3;j++)
    {
    	pm->mode_buttons[j].hovered = 0;
    }

}


void player_mode_hover(player_mode *pm,SDL_Renderer *renderer)
{
	
	int mouseX, mouseY;
	
	SDL_GetMouseState(&mouseX, &mouseY);
	for (int i = 0; i < 3; i++) {
        SDL_Texture *tex = pm->mode_buttons[i].img[0]; // default
	int wasHovered = pm->mode_buttons[i].hovered;
        pm->mode_buttons[i].hovered = 0;
        // If mouse is over button, use hover texture
        if (mouseX >= pm->mode_buttons[i].pos.x &&
            mouseX <= pm->mode_buttons[i].pos.x + pm->mode_buttons[i].pos.w &&
            mouseY >= pm->mode_buttons[i].pos.y &&
            mouseY <= pm->mode_buttons[i].pos.y + pm->mode_buttons[i].pos.h) {
            tex = pm->mode_buttons[i].img[1]; // hover texture
            pm->mode_buttons[i].hovered = 1;
		    	if (!wasHovered)
			    {
				Mix_PlayChannel(-1, pm->hoverSound, 0);
			    }
        }

        // Render the correct texture
        SDL_RenderCopy(renderer, tex, NULL, &pm->mode_buttons[i].pos);
        }
}


void handle_player_mode_input(player_mode *pm,SDL_Event event,int *interface)
{  
    //int *game_mode = 0;
    if(event.type == SDL_MOUSEBUTTONDOWN)
    {
        int mouseX = event.button.x;
        int mouseY = event.button.y;

        for(int i=0;i<3;i++)
        {
            if(mouseX >= pm->mode_buttons[i].pos.x &&
               mouseX <= pm->mode_buttons[i].pos.x + pm->mode_buttons[i].pos.w &&
               mouseY >= pm->mode_buttons[i].pos.y &&
               mouseY <= pm->mode_buttons[i].pos.y + pm->mode_buttons[i].pos.h)
            {
                Mix_PlayChannel(-1, pm->selectSound, 0);

                if(pm->mode_buttons[i].action == 1 ||
                   pm->mode_buttons[i].action == 2)
                {
                    //*game_mode = pm->mode_buttons[i].action;
                    *interface = 5;   // go to player selection
                }
                else if(pm->mode_buttons[i].action == 6)
                {
                    *interface = 0;   // back to main menu
                }
            }
        }
    }
}







void update_player_mode(player_mode *pm)
{
    int mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);

    for(int i=0;i<3;i++)
    {
        int wasHovered = pm->mode_buttons[i].hovered;
        pm->mode_buttons[i].hovered = 0;

        if(mouseX >= pm->mode_buttons[i].pos.x &&
           mouseX <= pm->mode_buttons[i].pos.x + pm->mode_buttons[i].pos.w &&
           mouseY >= pm->mode_buttons[i].pos.y &&
           mouseY <= pm->mode_buttons[i].pos.y + pm->mode_buttons[i].pos.h)
        {
            pm->mode_buttons[i].hovered = 1;

            if(!wasHovered)
                Mix_PlayChannel(-1, pm->hoverSound, 0);
        }
    }
}


	
void display_player_mode(player_mode *pm,SDL_Renderer *renderer)
{
    // Background
    SDL_RenderCopy(renderer,pm->mode_bg.img[pm->mode_bg.current_img],NULL,NULL);

    // Title
    SDL_RenderCopy(renderer,pm->mode_text.txt_text,NULL,&pm->mode_text.pos);

    // Buttons
    SDL_RenderCopy(renderer,pm->mode_buttons[0].img[0],NULL,&pm->mode_buttons[0].pos);

    SDL_RenderCopy(renderer,pm->mode_buttons[1].img[0],NULL,&pm->mode_buttons[1].pos);
    
    SDL_RenderCopy(renderer,pm->mode_buttons[2].img[0],NULL,&pm->mode_buttons[2].pos);
    //player_mode_hover(pm,renderer);
}	
	
/*int player_mode_click(player_mode *pm, SDL_Event *event)
{
    
    if (event->type == SDL_MOUSEBUTTONDOWN)
    {
        int mouseX = event->button.x;
        int mouseY = event->button.y;

        for (int i = 0; i < 3; i++)
        {
            if (mouseX >= pm->mode_buttons[i].pos.x &&
                mouseX <= pm->mode_buttons[i].pos.x + pm->mode_buttons[i].pos.w &&
                mouseY >= pm->mode_buttons[i].pos.y &&
                mouseY <= pm->mode_buttons[i].pos.y + pm->mode_buttons[i].pos.h)
            {
            	Mix_PlayChannel(-1, pm->selectSound, 0);
                return pm->mode_buttons[i].action;
            }
        }
    }

    return 0; // no click
}*/
	

void free_player_mode(player_mode *pm)
{
    SDL_DestroyTexture(pm->mode_bg.img[0]);

    for(int i=0;i<3;i++)
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
	
	
	
	
	
	
	
	


