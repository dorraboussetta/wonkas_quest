#include "outfit_menu.h"
#include <stdio.h>

void outfit_image_path(char *buf, int player, int outfit){
    sprintf(buf, "assets_outfit/%d_%d.png", player, outfit);
}

void init_outfit(SDL_Renderer *renderer, player_outfit *po, int player){
    char buf[100];
    for(int i = 0; i < 2; i++){
        outfit_image_path(buf, player, i);
        po->buttons[i] = IMG_LoadTexture(renderer, buf);
        if(!po->buttons[i]) printf("error loading %s\n", buf);
    }
    po->hoverSound  = Mix_LoadWAV("assets_player_menu/hover.wav");
    po->selectSound = Mix_LoadWAV("assets_player_menu/select.wav");
    po->outfit  = -1;
    po->player  = player;
    po->hovered = -1;
}

void init_buttons(button_om *b, SDL_Renderer *renderer){
    b[0].img[0] = IMG_LoadTexture(renderer, "assets_player_menu/pl_validate.png");
    b[0].img[1] = IMG_LoadTexture(renderer, "assets_player_menu/pl_validate_hover.png");
    b[0].pos.w = 505;
    b[0].pos.x = WIN_W-b[0].pos.w-25;
    b[0].pos.h = 141;
    b[0].pos.y = WIN_H-b[0].pos.h-25;
    b[0].action=0;

    b[1].img[0] = IMG_LoadTexture(renderer, "assets_player_menu/ps_return.png");
    b[1].img[1] = IMG_LoadTexture(renderer, "assets_player_menu/ps_return_hover.png");
    b[1].pos.w = 380;
    b[1].pos.x = 25;
    b[1].pos.h = 141;
    b[1].pos.y = WIN_H-b[1].pos.h-25;
    b[1].action=0;
}

void render_text(SDL_Renderer *renderer, TTF_Font *font, char *text, int x, int y){
    if(!font){ printf("font is NULL\n"); return; }
    SDL_Color white = {255, 255, 255, 255};
    SDL_Surface *s = TTF_RenderText_Blended(font, text, white);
    if(!s){ printf("surface is NULL\n"); return; }
    SDL_Texture *t = SDL_CreateTextureFromSurface(renderer, s);
    if(!t){ printf("texture is NULL\n"); SDL_FreeSurface(s); return; }
    SDL_Rect r = {x, y, s->w, s->h};
    SDL_RenderCopy(renderer, t, NULL, &r);
    SDL_FreeSurface(s);
    SDL_DestroyTexture(t);
}


void init_outfit_pos(SDL_Renderer *renderer, player_outfit *po, int num_players){
    for(int j = 0; j < 2; j++){
        if(po->buttons[j] != NULL){
            SDL_QueryTexture(po->buttons[j], NULL, NULL, &po->pos[j].w, &po->pos[j].h);
            po->pos[j].w*=2;
            po->pos[j].h*=2;
            if(j == 0) po->pos[j].x = WIN_W/2 - po->pos[j].w - 80;
            else       po->pos[j].x = WIN_W/2 + 80;
        }
    }
    if(num_players == 1){
        for(int j = 0; j < 2; j++)
            po->pos[j].y = WIN_H/2 - po->pos[j].h/2;
    }
    if(num_players == 2){
        if(po->player == 0){
            for(int j = 0; j < 2; j++)
                po->pos[j].y = WIN_H/2 - po->pos[j].h - 80;
        } else {
            for(int j = 0; j < 2; j++)
                po->pos[j].y = WIN_H/2 + 80;
        }
    }
}

void init_outfit_menu(SDL_Renderer *renderer, player_outfit *po, outfit_menu *om, int num_players, int selected_player, int level){
    om->bg.img=IMG_LoadTexture(renderer, "assets_player_menu/player_menu_bg.png");
    if(!om->bg.img) printf("bg load failed\n");

    om->font = TTF_OpenFont("assets_outfit/Pixel Game.otf", 40);
    if(!om->font) printf("font error: %s\n", TTF_GetError());
    
    init_buttons(om->b,renderer);
    if(!om->b[0].img[0]) printf("validate btn failed\n");
    if(!om->b[1].img[0]) printf("return btn failed\n");

    if(level == 1){
        if(num_players == 1){
            init_outfit(renderer, &po[0], selected_player);
            init_outfit_pos(renderer, &po[0], num_players);
        } else {
            for(int i = 0; i < 2; i++){
                init_outfit(renderer, &po[i], i);
                init_outfit_pos(renderer, &po[i], num_players);
            }
        }
    } else {
        for(int i = 0; i < num_players; i++){
            init_outfit(renderer, &po[i], 0);
            init_outfit_pos(renderer, &po[i], num_players);
        }
    }
}

void display_outfit_menu(SDL_Renderer *renderer, player_outfit *po, outfit_menu *om, int num_players){
    SDL_RenderCopy(renderer, om->bg.img, NULL, NULL);
    for(int i=0;i<2;i++){
    	SDL_RenderCopy(renderer, om->b[i].img[om->b[i].action], NULL, &om->b[i].pos);
    }
    render_text(renderer, om->font, "Choose your outfit", WIN_W/2 - 150, 30);
    char buf[100];
    for(int i = 0; i < num_players; i++){
    	sprintf(buf,"Player %d",i+1);
    	render_text(renderer, om->font, buf, WIN_W/2 - 50, po[i].pos[0].y - 50);
        for(int j = 0; j < 2; j++){
            SDL_RenderCopy(renderer, po[i].buttons[j], NULL, &po[i].pos[j]);

            SDL_Rect border = {
                po[i].pos[j].x - 4,
                po[i].pos[j].y - 4,
                po[i].pos[j].w + 8,
                po[i].pos[j].h + 8,
            };

            if(po[i].hovered == j || po[i].outfit == j){
                if(po[i].outfit == j)
                    SDL_SetRenderDrawColor(renderer, 255, 215, 0, 255);
                else
                    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                SDL_RenderDrawRect(renderer, &border);
            }
        }
    }
}

int check_position(int x, int y, SDL_Rect pos){
	if(x >= pos.x && x <=pos.x + pos.w && y >= pos.y && y <= pos.y + pos.h){
		return 1;
	}
	return 0;
}



void handle_outfit_menu(player_outfit *po, SDL_Event event, outfit_menu *om, int num_players, int *interface){
    if(event.type == SDL_MOUSEMOTION){
        int mx = event.motion.x, my = event.motion.y;
        
        for(int i = 0; i < num_players; i++){
            int prev = po[i].hovered;
            po[i].hovered = -1;
            
            for(int j = 0; j < 2; j++){
                if(check_position(mx,my,po[i].pos[j])==1)
                    po[i].hovered = j;
            }
            
            if(po[i].hovered != prev && po[i].hovered != -1)
                Mix_PlayChannel(-1, po[i].hoverSound, 0);
        }
        
        for(int i=0;i<2;i++){
        	if(check_position(mx,my,om->b[i].pos)==1){
        		om->b[i].action=1;
        		Mix_PlayChannel(-1, po[i].hoverSound, 0);
        	}else
        		om->b[i].action=0;
      
        }
        
    }

    if(event.type == SDL_MOUSEBUTTONDOWN){
        int mx = event.button.x, my = event.button.y;
        for(int i = 0; i < num_players; i++){
            for(int j = 0; j < 2; j++){
                if(check_position(mx,my,po[i].pos[j])==1){
                    po[i].outfit = j;
                    Mix_PlayChannel(-1, po[i].selectSound, 0);
                }
            }
        }
        
        
	if(check_position(mx,my,om->b[0].pos)==1){
            Mix_PlayChannel(-1, po[0].selectSound, 0);
	    if(po[0].outfit == -1) return;
	    if(num_players == 2 && po[1].outfit == -1) return;
		*interface = 7;
	}
	if(check_position(mx,my,om->b[1].pos)==1){
	    Mix_PlayChannel(-1, po[0].selectSound, 0);
		*interface = 5;
	}
    }

    if(event.type == SDL_KEYDOWN){
        if(event.key.keysym.sym == SDLK_ESCAPE)
            *interface = 5;
        if(event.key.keysym.sym == SDLK_RETURN){
            if(po[0].outfit == -1) return;
            if(num_players == 2 && po[1].outfit == -1) return;
            *interface = 7;
        }
    }
    if(event.type == SDL_QUIT)
        *interface = -1;
}

void free_outfit_menu(player_outfit *po, outfit_menu *om, int num_players){
    for(int i = 0; i < num_players; i++){
        for(int j = 0; j < 2; j++)
            SDL_DestroyTexture(po[i].buttons[j]);
        Mix_FreeChunk(po[i].hoverSound);
        Mix_FreeChunk(po[i].selectSound);
    }
    SDL_DestroyTexture(om->bg.img);
    for(int i=0;i<2;i++){
    	for(int j=0;j<2;j++){
    		SDL_DestroyTexture(om->b[i].img[j]);
    	}
    }
    TTF_CloseFont(om->font);
}
