#include "player.h"
#include <stdio.h>



int init_player(Player *p, SDL_Renderer *renderer, int width, int height, int folder, int outfit){
    char path[100];
    int i, j;
    for(i = 0; i < NBL; i++){
        for(j = 0; j < NBC; j++){
            sprintf(path, "assets_player/%d/%d/%d_%d.png", folder, outfit, i+1, j+1);
            p->tab[i][j] = IMG_LoadTexture(renderer, path);
            if(p->tab[i][j] == NULL){
                printf("error loading %s\n", path);
                return 1;
            }
            SDL_QueryTexture(p->tab[i][j],NULL,NULL,&p->sizes[i][j].w,&p->sizes[i][j].h);
            p->sizes[i][j].w *=1.5;
            p->sizes[i][j].h *=1.5;
        }
    }
    
    for(i = 0;i<8;i++){
        sprintf(path, "assets_player/life/%d.png", i);
        p->life_b[i] = IMG_LoadTexture(renderer, path);
    }
    SDL_Texture *choco= IMG_LoadTexture(renderer, "assets_player/weapon.png");
    for(i = 0;i<15;i++){
    	p->weapons[i].image= choco;
    	p->weapons[i].pos.w= 20;
    	p->weapons[i].pos.h= 20;
    	
    	p->weapons[i].status=0;
    	p->weapons[i].contact=0;
    	p->weapons[i].score=1;
    }
    
    p->font =TTF_OpenFont("assets_player/Pixel Game.otf",24);
    if(p->font==NULL){
    	printf("error font\n");
    	return 1;
    }
    
    p->num = 0;
    p->mode = 0;
    p->direction = RIGHT;
    p->anim_counter = 0;
    p->rel_x = -50;
    p->pos_screen.w = p->sizes[0][0].w;
    p->pos_screen.h = p->sizes[0][0].h;
    p->pos_screen.x = 100;
    p->pos_screen.y = ground_level-p->pos_screen.h;
    p->velocity = 0;
    p->acceleration = 0;
    p->up = 0;
    p->life = 7;
    p->score = 0;
    p->outfit = outfit;
    p->pos_init=p->pos_screen.y;    
    return 0;
}

SDL_Rect to_screen(SDL_Rect world_pos, SDL_Rect camera){
	SDL_Rect screen ={
		world_pos.x - camera.x,
		world_pos.y - camera.y,
		world_pos.w,
		world_pos.h,
	};
	return screen;
}

void fire_weapon(Player *p){
	for(int k=0;k<15;k++){
		if(p->weapons[k].active==0){
			p->weapons[k].active=1;
			p->weapons[k].direction=p->direction;
			p->weapons[k].pos.x=p->pos_screen.x;
			p->weapons[k].pos.y=p->pos_screen.y+40;
			break;
		}
	}

}

void update_weapons(Player *p){
	for(int k=0;k<15;k++){
		if(p->weapons[k].active==1){
			if(p->weapons[k].direction == RIGHT){
				p->weapons[k].pos.x+=8;
			}else{
				p->weapons[k].pos.x-=8;
			}
			
			if(p->weapons[k].pos.x + p->weapons[k].pos.w< 0 || p->weapons[k].pos.x>p->max_x){
				p->weapons[k].active=0;
			}
		}
	}
}

void display_weapons(Player *p, SDL_Renderer *renderer, SDL_Rect camera){
	for(int k=0;k<15;k++){
		if(p->weapons[k].active==1){
			SDL_Rect dest =to_screen(p->weapons[k].pos,camera);
			SDL_RenderCopy(renderer, p->weapons[k].image, NULL, &dest);
		}
	}
}

void animate_player(Player *p){
    p->anim_counter++;
    if(p->anim_counter >= 24){
        p->anim_counter = 0;
        if(p->num == NBC-1)
            p->num = 0;
        else
            p->num++;
            
        p->pos_screen.w=p->sizes[p->mode][p->num].w;
        p->pos_screen.h=p->sizes[p->mode][p->num].h;
    }
}

void display_score(Player *p, SDL_Renderer *renderer){
	char score_text[20];
	
	sprintf(score_text,"Score: %d", p->score);
	
	SDL_Color gold={255,215,0,255};
	SDL_Surface *surface= TTF_RenderText_Blended(p->font, score_text, gold);
	if(surface==NULL) return;
	SDL_Texture *texture= SDL_CreateTextureFromSurface(renderer, surface);
	SDL_Rect score_rect={10,70,surface->w,surface->h};
	
	
	
	SDL_RenderCopy(renderer, texture, NULL,&score_rect);
	SDL_FreeSurface(surface);
	SDL_DestroyTexture(texture);
	
}







void display_player(Player *p, SDL_Renderer *renderer, int height, SDL_Rect camera){
    SDL_RendererFlip flip;
    if(p->direction == LEFT)
        flip = SDL_FLIP_HORIZONTAL;
    else
        flip = SDL_FLIP_NONE;
    p->pos_screen.w=p->sizes[p->mode][p->num].w;
    p->pos_screen.h=p->sizes[p->mode][p->num].h;

    SDL_Rect dest =to_screen(p->pos_screen,camera);
    SDL_RenderCopyEx(renderer, p->tab[p->mode][p->num], NULL, &dest, 0, NULL, flip);

    /* Reset clip so HUD always draws on top */
    SDL_RenderSetClipRect(renderer, NULL);
    SDL_Rect bar={10,10,225,50};
    SDL_RenderCopy(renderer,p->life_b[p->life],NULL,&bar);

    display_score(p, renderer);
    display_weapons(p, renderer,camera);
}



void move_player(Player *p, Uint32 dt){
    p->velocity += p->acceleration;
    if(p->velocity < 0) p->velocity = 0;
    if(p->velocity > 5) p->velocity = 5;
    if(p->direction == RIGHT)
        p->pos_screen.x += (int)p->velocity;
    else
        p->pos_screen.x -= (int)p->velocity;
    if(p->pos_screen.x < 0) p->pos_screen.x = 0;
    if(p->pos_screen.x + p->pos_screen.w > p->max_x)
        p->pos_screen.x = p->max_x - p->pos_screen.w;
}



void jump_player(Player *p, Uint32 dt){
    if(p->up == 1){
        p->rel_x += 1;
        p->pos_screen.y = p->pos_init - (int)(-0.04 * p->rel_x * p->rel_x + 100);
        if(p->direction == RIGHT)
            p->pos_screen.x += 3;
        else if(p->direction == LEFT)
            p->pos_screen.x -= 3;
        if(p->pos_screen.x < 0) p->pos_screen.x = 0;
        if(p->pos_screen.x + p->pos_screen.w > p->max_x)
            p->pos_screen.x = p->max_x - p->pos_screen.w;
        if(p->rel_x >= 50){
            p->pos_screen.y = p->pos_init;
            p->up = 0;
            p->rel_x = -50;
            p->num = 0;
            p->mode = 0;
        }
    }
}



void event_player(Player *p, SDL_Event event){
    if(event.type == SDL_KEYDOWN){
    	if(p->life==0) return;
        if(event.key.keysym.sym == SDLK_RIGHT){
            p->direction = RIGHT;
            p->mode = 1;
            p->acceleration += 0.2;
        }
        if(event.key.keysym.sym == SDLK_LEFT){
            p->direction = LEFT;
            p->mode = 1;
            p->acceleration += 0.2;
        }
        if(event.key.keysym.sym == SDLK_UP){
            if(p->up == 0){
                p->up = 1;
                p->rel_x = -50;
                p->mode = 3;
                p->num = 0;
            }
        }
        if(event.key.keysym.sym == SDLK_SPACE){
        	if(p->up==0){
        		p->mode=4;
        		p->num=0;
        	}
        	fire_weapon(p);
        }
    }
    if(event.type == SDL_KEYUP){
        p->acceleration = 0;
        p->velocity = 0;
        if(p->up == 0 && p->mode<4 )
            p->mode = 0;
    }
}



void event_player2(Player *p, SDL_Event event){
    if(event.type == SDL_KEYDOWN){
    	if(p->life==0) return;
        if(event.key.keysym.sym == SDLK_d){
            p->direction = RIGHT;
            p->mode = 1;
            p->acceleration += 0.2;
        }
        if(event.key.keysym.sym == SDLK_q){
            p->direction = LEFT;
            p->mode = 1;
            p->acceleration += 0.2;
        }
        if(event.key.keysym.sym == SDLK_z){
            if(p->up == 0){
                p->up = 1;
                p->rel_x = -50;
                p->mode = 3;
                p->num = 0;
            }
        }
        if(event.key.keysym.sym == SDLK_f){
        	if(p->up==0){
        		p->mode=4;
        		p->num=0;
        	}
        	
        	fire_weapon(p);
        }
    }
    if(event.type == SDL_KEYUP){
        p->acceleration = 0;
        p->velocity = 0;
        if(p->up == 0 && p->mode<4 )
            p->mode = 0;
    }
}

void update_player(Player *p, Uint32 dt){

    if(p->life==0){
    	p->mode=6;
    	if(p->num<NBC-1){
    		p->num++;
    	}
    	return;
    }
    p->acceleration -= 0.01;
    if(p->acceleration < 0) p->acceleration = 0;
    if(p->up==0){
    	if(p->mode>=4){
    		if( p->num==NBC-1){
    			p->mode = 0;
    	}
    	}else if(p->velocity >3){
    		p->mode =2;
    	}else if(p-> velocity >0){
    		p->mode = 1;
    	}else{
    		p->mode = 0;
    	}
    }
    update_weapons(p);
    move_player(p, dt);
    if(p->up != 0)
	jump_player(p, dt);
	
    p->pos_screen.w=p->sizes[p->mode][p->num].w;
    p->pos_screen.h=p->sizes[p->mode][p->num].h;
    animate_player(p);

}


void free_player(Player *p){
    int i, j;
    for(i = 0; i < NBL; i++){
        for(j = 0; j < NBC; j++){
            SDL_DestroyTexture(p->tab[i][j]);
        }
    }
    
    for(i=0;i<8;i++){
    	SDL_DestroyTexture(p->life_b[i]);
    }
    SDL_DestroyTexture(p->weapons[0].image);
    TTF_CloseFont(p->font);
}




void init_players(Player *p, SDL_Renderer *renderer, int num_players, int width, int height, int folder, int level, int outfit_p1, int outfit_p2){

	if(level==1){
		if(num_players==1){
			init_player(&p[0], renderer, width, height, folder, outfit_p1);
			p[0].max_x=width;
		}else if(num_players==2){
			init_player(&p[0], renderer, width, height, 0, outfit_p1);
			init_player(&p[1], renderer, width, height, 1, outfit_p2);
			p[0].max_x=width/2;
			p[1].max_x=width/2;
		}
	}
	else if(level==2){
		init_player(&p[0], renderer, width, height, 0, outfit_p1);
		if(num_players==2){
			init_player(&p[1], renderer, width, height, 0, outfit_p2);
			p[0].max_x=width/2;
			p[1].max_x=width/2;
		}else{
			p[0].max_x=width;
		}
	}
}

void event_players(Player *p, SDL_Event event, int num_players){
	event_player(&p[0], event);
	if(num_players==2){
		event_player2(&p[1], event);
	}
}
void update_players(Player *p, Uint32 dt, int num_players,int width, int height){
	for(int i=0;i<num_players;i++){
		update_player(&p[i],dt);
	}
}

void display_players(Player *p, SDL_Renderer *renderer, int num_players,int width, int height, SDL_Rect camera){
	if(num_players==2){
		
		SDL_Rect left={0,0,width/2,height};
		SDL_RenderSetViewport(renderer,&left);
		display_player(&p[0],renderer,height,camera);
		
		SDL_Rect right={width/2,0,width/2,height};
		SDL_RenderSetViewport(renderer,&right);
		display_player(&p[1],renderer,height,camera);
		
		SDL_RenderSetViewport(renderer,NULL);
	}else{
		display_player(&p[0],renderer,height,camera);
	}
}



void free_players(Player *p, int num_players){
	for(int i=0;i<num_players;i++){
		free_player(&p[i]);
	}
}



void player_take_damage(Player *p, int damage){
	if(p->life<=0) return;
	p->life -=damage;
	if(p->life < 0) p->life = 0;
	p->mode =5;
	p->num = 0;
}


void player_collect_coin(Player *p, int coin_type, int level){
	if(coin_type==1) p->score +=level;
	else if (p->life <7 && coin_type==0) p->life++;
}























