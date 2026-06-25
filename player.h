#ifndef PLAYER_H
#define PLAYER_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>


#define NBL 7 
#define NBC 6 

#define RIGHT 0
#define LEFT 1

#define HEIGHT 1080
#define WIDTH 1920

#define ground_level 1786

typedef struct {
	SDL_Texture *image; 
	int direction, active; 
	int status,contact ,score;
	SDL_Rect pos; 
}weapon;

typedef struct{
    SDL_Texture *tab[NBL][NBC];
    int num, mode, direction, anim_counter, pos_init,max_x;
    float rel_x; 
    SDL_Rect pos_screen, sizes[NBL][NBC];
    double velocity, acceleration;
    int up; 
    
    SDL_Texture *life_b[8];
    int life, score;
    TTF_Font *font;
    
    weapon weapons[15];
    int outfit;
    
} Player;

typedef weapon Entity_player;



void player_take_damage(Player *p, int damage);

void player_collect_coin(Player *p, int coin_type, int level);


void init_players(Player *p, SDL_Renderer *renderer, int num_players, int width, int height, int folder, int level, int outfit_p1, int outfit_p2);
void event_players(Player *p, SDL_Event event, int num_players);
void update_players(Player *p, Uint32 dt, int num_players,int width, int height);
void display_players(Player *p, SDL_Renderer *renderer, int num_players,int width, int height, SDL_Rect camera);
void free_players(Player *p, int num_players);


#endif
