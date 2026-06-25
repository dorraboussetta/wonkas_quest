#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h> 
#include <stdio.h>
#include <string.h>
#include "enemy.h"
#include "common.h"
#include "player.h"

int initialize_enemy(Enemy *enemy, SDL_Renderer *renderer, char name[]) {
	
	
	const char D[][6]={"Right", "Left"}; 
	const char states[][6]={"Idle", "Walk", "Run", "Jump", "Fight", "Hurt", "Death"}; 
	int i=0; //Counter for the states array
	char file_name[100];
	
	strcpy(enemy->name, name); 
	for (int cy=0; cy<14; cy+=2){
		
		for (int fr=0; fr<6; fr++){
			sprintf(file_name, "Assets_Enemy/%s/%s_%sSide/%s_%sSide_%d.png", name, states[i], D[0], states[i], D[0], fr+1); 
			enemy->sprite[cy][fr]=IMG_LoadTexture(renderer, file_name);
			
			if (enemy->sprite[cy][fr]==NULL) {
				printf("Failed loading: %s\n", file_name);
    				printf("IMG Error: %s\n", IMG_GetError());
    				return 1;
    				
			}
			
			sprintf(file_name, "Assets_Enemy/%s/%s_%sSide/%s_%sSide_%d.png", name, states[i], D[1], states[i], D[1], fr+1); 
			enemy->sprite[cy+1][fr]=IMG_LoadTexture(renderer, file_name);
			
			if (enemy->sprite[cy+1][fr]==NULL) {
				printf("Failed loading: %s\n", file_name);
    				printf("IMG Error: %s\n", IMG_GetError());
    				return 1;
			} 
			
		
		
		}
		i++; 
	}
	
	for (int bar=0; bar<8; bar++) {
		sprintf(file_name, "Assets_Enemy/Health Bar/Health_Bar_%d.png", bar+1); 
		enemy->status_bar[bar]=IMG_LoadTexture(renderer, file_name); 
		
		if (enemy->status_bar[bar]==NULL) {
				printf("Failed loading: %s\n", file_name);
    				printf("IMG Error: %s\n", IMG_GetError());
    				return 1;
			} 
	}
	
	for (int i=0; i<num_weapons; i++) {
		enemy->weapons[i].image=NULL; 
		enemy->weapons[i].image=IMG_LoadTexture(renderer,"Assets_Enemy/Weapon.png");
		if (enemy->weapons[i].image==NULL) {
			printf("Failed loading: %s\n", "Assets_Enemy/Weapon.png");
			printf("IMG Error: %s\n", IMG_GetError());
			return 1;}
		enemy->weapons[i].status=0; 
		enemy->weapons[i].score=1; 
		enemy->weapons[i].contact=0;
		enemy->weapons[i].pos.w=16; 
		enemy->weapons[i].pos.h=16; 
		enemy->weapons[i].direction=0; 
		
		
	}
	//TODO modify this after the levels and the enemies are set. Wonka or Noodle are the players in level 1 but he could be the enemy in level 2 depending on what player the user selects
	if (strcmp(name, "Wonka")==0) {
		enemy->cycle_requested=0;
		enemy->cycle=0; } 
	else {
		enemy->cycle_requested=1;
		enemy->cycle=1; } 
	enemy->frame=0;
	enemy->lastFrameSwitchTime=SDL_GetTicks();  

	SDL_QueryTexture(enemy->sprite[enemy->cycle][enemy->frame], NULL, NULL, &enemy->EnPos.w, &enemy->EnPos.h);
printf("EnPos.h=%d\n", enemy->EnPos.h);
	
	if (strcmp(name, "Wonka")==0 || strcmp(name, "Noodle")==0) enemy->EnPos.x=8892;
	else {
		if (strcmp(name, "Slugworth")==0) enemy->EnPos.x=8892;
		else {
			if (strcmp(name, "Scrubitt")==0) enemy->EnPos.x=1400;
		}
	} 
	enemy->EnPos.y=ground_level-(int)(enemy->EnPos.h*1.5)+1080;
	//Health status(0-7): 7 for full health, 0 for dead
	enemy->health_status=7; 
	enemy->is_attacking=0; enemy->first_throw_frame=0; 
	enemy->jump_var.is_jumping=0; 
	enemy->jump_var.jump_threshold=0; //here: haven't used this yet. should there be a ceiling anyway? 
	enemy->jump_var.y=0;
	enemy->velocity=0.3; enemy->acceleration=0; enemy->accelerate=0; enemy->deccelerate=0; 
	
	enemy->hurt_animation=0; enemy->death_animation=0; enemy->is_dead=0;
	
	enemy->player_collision.status=0; enemy->player_collision.first_contact=0; enemy->player_collision.on_top=0;   
	enemy->coin_collision.status=0;
	enemy->platform_collision.status=0; enemy->platform_collision.first_contact=0; enemy->platform_collision.on_top=0; enemy->jump_var.was_on_platform=0; 
	enemy->projectile_collision.status=0; enemy->projectile_collision.first_contact=0; enemy->projectile_collision.on_top=0;   
	enemy->bg_collision.status=0; enemy->jump_var.was_on_obstacle=0; enemy->bg_collision.on_top=0; 
	
	enemy->random_mv_var.last_random_switch_time=SDL_GetTicks();
	enemy->random_mv_var.first_rand_switch=1;  
	enemy->random_mv_var.random_started=1; 
	enemy->random_mv_var.pos_min=10; enemy->random_mv_var.pos_max=300; 
	
	enemy->weapon_fired=0; 
	enemy->shoot_sound=NULL; enemy->hurt_sound=NULL; enemy->death_sound=NULL; enemy->huh_sound=NULL; enemy->hey_sound=NULL;
	enemy->shoot_sound=Mix_LoadWAV("Assets_Enemy/shoot_sound.wav");
	enemy->huh_sound=Mix_LoadWAV("Assets_Enemy/huh_sound.mp3");
	enemy->hey_sound=Mix_LoadWAV("Assets_Enemy/hey_sound.mp3");
	if (strcmp(name,"Wonka")==0 || strcmp(name, "Slugworth")==0)
		{
			enemy->hurt_sound=Mix_LoadWAV("Assets_Enemy/male_hurt_sound.wav");
			enemy->death_sound=Mix_LoadWAV("Assets_Enemy/male_death_sound.wav");
		}
	else {
		enemy->hurt_sound=Mix_LoadWAV("Assets_Enemy/female_hurt_sound.wav");
		enemy->death_sound=Mix_LoadWAV("Assets_Enemy/female_death_sound.wav");
	}

	if (enemy->shoot_sound==NULL || enemy->hurt_sound==NULL || enemy->death_sound==NULL || enemy->huh_sound==NULL || enemy->hey_sound==NULL) {
			printf("MIX Error: %s\n", Mix_GetError());
			return 1;}
	Mix_VolumeChunk(enemy->hurt_sound, sound_volume); Mix_VolumeChunk(enemy->death_sound, sound_volume);
	Mix_VolumeChunk(enemy->shoot_sound, sound_volume);Mix_VolumeChunk(enemy->huh_sound, sound_volume); Mix_VolumeChunk(enemy->hey_sound, sound_volume);
	enemy->startup_moving_state=0; 
	if (strcmp(enemy->name, "Scrubitt")==0) {
		enemy->State=Moving; 
		enemy->startup_moving_state=1;  enemy->movingStateOnTime=SDL_GetTicks(); 
		}
		else enemy->State=Waiting;
	enemy->ai_direction=0;
	enemy->sound_played=0;
	
	return 0; 
	
}



void display_enemy(SDL_Renderer *renderer, Enemy *enemy, SDL_Rect camera){
	SDL_Rect pos_bar, screen_pos; 
	
	if (enemy->is_dead==1 && (SDL_GetTicks()-enemy->lastFrameSwitchTime>=2000)) return; 
 	SDL_QueryTexture(enemy->sprite[enemy->cycle][enemy->frame], NULL, NULL, &enemy->EnPos.w, &enemy->EnPos.h);
 	enemy->EnPos.w *= 1.5;
	enemy->EnPos.h *= 1.5;
 	if (enemy->jump_var.is_jumping==0) {
 		if (enemy->death_animation || enemy->is_dead) {
 			enemy->EnPos.y=enemy->death_y-enemy->EnPos.h; 
 			 
 			}
 		else 
 		if (enemy->bg_collision.status &&enemy->bg_collision.on_top) enemy->EnPos.y=enemy->bg_collision.y-enemy->EnPos.h; 
 		else {
	 		if (enemy->platform_collision.on_top==1) enemy->EnPos.y=enemy->platform_collision.y-enemy->EnPos.h; 
	 		else {
	 			if (enemy->jump_var.was_on_platform==0 && enemy->jump_var.was_on_obstacle==0) enemy->EnPos.y=ground_level_enemy-enemy->EnPos.h;
	 			 } }
 		}

 		
 	screen_pos.x = enemy->EnPos.x - camera.x;
 	screen_pos.y = enemy->EnPos.y - camera.y;
 	screen_pos.w = enemy->EnPos.w;
 	screen_pos.h = enemy->EnPos.h;

 	pos_bar.w=95; pos_bar.h=25; 
 	pos_bar.x=screen_pos.x-((pos_bar.w/2)-screen_pos.w/2); 
 	pos_bar.y=screen_pos.y-pos_bar.h; 
 	
	SDL_RenderCopy(renderer, enemy->sprite[enemy->cycle][enemy->frame], NULL, &screen_pos);
	SDL_RenderCopy(renderer, enemy->status_bar[enemy->health_status], NULL, &pos_bar); 
	fire_weapon_enemy(enemy, renderer, camera); 
}


int initialize_ES_level1(SDL_Renderer *renderer, Entity coins[], int *num_coins_level1){
	FILE *Score_coins_file=fopen("./Assets_Enemy/Score_Coins_level1.txt", "r"), *Health_coins_file=fopen("./Assets_Enemy/Health_Coins_level1.txt", "r"); 
	
	if (!Score_coins_file) {
		printf("Error opening score coins file.\n"); return 1; 
	}
	if (!Health_coins_file) {
		printf("Error opening Health coins file.\n"); return 1; 
	}

	

	
	while (fscanf(Score_coins_file, "%d %d", &coins[*num_coins_level1].pos.x, &coins[*num_coins_level1].pos.y)==2){ 
		coins[*num_coins_level1].image=NULL; 
		coins[*num_coins_level1].status=1; 
		coins[*num_coins_level1].contact=0;
		coins[*num_coins_level1].score=1; 
		coins[*num_coins_level1].pos.w=52; 
		coins[*num_coins_level1].pos.h=52; 
		coins[*num_coins_level1].type=1;
		coins[*num_coins_level1].image=IMG_LoadTexture(renderer, "Assets_Enemy/Score_Coin.png");
		if (!coins[*num_coins_level1].image) {
			printf("IMG Error: %s\n", IMG_GetError());
			return 1;
		}
		(*num_coins_level1)++; 

	}
	fclose(Score_coins_file); 
	
	while (fscanf(Health_coins_file, "%d %d", &coins[*num_coins_level1].pos.x, &coins[*num_coins_level1].pos.y)==2){ 

		coins[*num_coins_level1].image=NULL; 
		coins[*num_coins_level1].status=1; 
		coins[*num_coins_level1].contact=0;
		coins[*num_coins_level1].score=1; 
		coins[*num_coins_level1].pos.w=52; 
		coins[*num_coins_level1].pos.h=52; 
		coins[*num_coins_level1].type=0;
		coins[*num_coins_level1].image=IMG_LoadTexture(renderer, "Assets_Enemy/Health_Coin.png");
		if (!coins[*num_coins_level1].image) {
			printf("IMG Error: %s\n", IMG_GetError());
			return 1;
		}
		(*num_coins_level1)++; 

	}
	
	fclose(Health_coins_file); 
 
	
	return 0; 
}

int initialize_ES_level2(SDL_Renderer *renderer, Entity coins[], int *num_coins_level2, Enemy *enemy1, Enemy *enemy2, Background_temp *bg){
	FILE *Score_coins_file=fopen("./Assets_Enemy/Score_Coins_level1.txt", "r"), *Health_coins_file=fopen("./Assets_Enemy/Health_Coins_level1.txt", "r"); 
	
	if (!Score_coins_file) {
		printf("Error opening score coins file.\n"); return 1; 
	}
	if (!Health_coins_file) {
		printf("Error opening Health coins file.\n"); return 1; 
	}

	

	
	while (fscanf(Score_coins_file, "%d %d", &coins[*num_coins_level2].pos.x, &coins[*num_coins_level2].pos.y)==2){ 
		coins[*num_coins_level2].image=NULL; 
		coins[*num_coins_level2].status=1; 
		coins[*num_coins_level2].contact=0;
		coins[*num_coins_level2].score=1; 
		coins[*num_coins_level2].pos.w=52; 
		coins[*num_coins_level2].pos.h=52; 
		coins[*num_coins_level2].type=0;
		coins[*num_coins_level2].image=IMG_LoadTexture(renderer, "Assets_Enemy/Score_Coin.png");
		if (!coins[*num_coins_level2].image) {
			printf("IMG Error: %s\n", IMG_GetError());
			return 1;
		}
		(*num_coins_level2)++; 

	}
	fclose(Score_coins_file); 
	
	while (fscanf(Health_coins_file, "%d %d", &coins[*num_coins_level2].pos.x, &coins[*num_coins_level2].pos.y)==2){ 

		coins[*num_coins_level2].image=NULL; 
		coins[*num_coins_level2].status=1; 
		coins[*num_coins_level2].contact=0;
		coins[*num_coins_level2].score=1; 
		coins[*num_coins_level2].pos.w=52; 
		coins[*num_coins_level2].pos.h=52; 
		coins[*num_coins_level2].type=0;
		coins[*num_coins_level2].image=IMG_LoadTexture(renderer, "Assets_Enemy/Health_Coin.png");
		if (!coins[*num_coins_level2].image) {
			printf("IMG Error: %s\n", IMG_GetError());
			return 1;
		}
		(*num_coins_level2)++; 

	}
	
	fclose(Health_coins_file); 
	
	
	for (int i=0; i<num_weapons; i++) 
		{
		enemy1->weapons[i].score=2; 
		enemy2->weapons[i].score=2; 
		}
	
	return 0; 
	coins[*num_coins_level2].image=NULL; 
	coins[*num_coins_level2].status=1; 
	coins[*num_coins_level2].contact=0;
	coins[*num_coins_level2].score=1; 
	coins[*num_coins_level2].pos.w=65; 
	coins[*num_coins_level2].pos.h=70; 
	coins[*num_coins_level2].type=0;
	coins[*num_coins_level2].image=IMG_LoadTexture(renderer, "Assets_Enemy/Key.png");
		if (!coins[*num_coins_level2].image) {
			printf("IMG Error: %s\n", IMG_GetError());
			return 1;
		}
	(*num_coins_level2)++; 
	
}

void navigate_to_platform(Enemy *enemy, Platform platforms[], SDL_Rect player_pos){
	SDL_Rect dest_platform={platforms[0].pos.x, platforms[0].pos.y, platforms[0].pos.w, platforms[0].pos.h};
	int target_platform=-1, next_platform=-1, max_jump_height=200, jump_start_distance=200;

	/*
		If the player is below the enemy, do not choose an upward platform.
		Just run toward the player/edge and let the existing edge logic handle the drop.
	*/
	if (player_pos.y+player_pos.h>enemy->EnPos.y+enemy->EnPos.h) {
		if (player_pos.x>enemy->EnPos.x) enemy->ai_direction=1;
		else {
			if (player_pos.x+player_pos.w<enemy->EnPos.x) enemy->ai_direction=0;
		}
		enemy->State=Following;
		return;
	}

	/*
		Find the platform the player is standing on. This is the final target,
		not necessarily the next platform the enemy should jump to.
	*/
	for (int i=0; i<num_platforms; i++) {
		if (platforms[i].status==0) continue;
		if (player_pos.x>platforms[i].pos.x && player_pos.x+player_pos.w<platforms[i].pos.x+platforms[i].pos.w && platforms[i].pos.y-(player_pos.y+player_pos.h)<=5 && platforms[i].pos.y-(player_pos.y+player_pos.h)>=-5 ) {
			dest_platform.x=platforms[i].pos.x;
			dest_platform.y=platforms[i].pos.y;
			target_platform=i;
			break;
		}
	}

	/*
		If the player is not on a detected platform, keep chasing by X.
		This covers the player dropping to ground or being between platforms.
	*/
	if (target_platform==-1) {
		if (player_pos.x>enemy->EnPos.x) enemy->ai_direction=1;
		else {
			if (player_pos.x+player_pos.w<enemy->EnPos.x) enemy->ai_direction=0;
		}
		enemy->State=Following;
		return;
	}

	next_platform=target_platform;

	/*
		If the final target is too high, choose the highest reachable intermediate
		platform below the player's platform.
	*/
	if (enemy->EnPos.y-platforms[target_platform].pos.y>max_jump_height) {

		next_platform=-1;
		for (int i=0; i<num_platforms; i++) {
			if (i==target_platform || platforms[i].status==0) continue;
			if(platforms[i].pos.y<enemy->EnPos.y && platforms[i].pos.y>platforms[target_platform].pos.y && enemy->EnPos.y-platforms[i].pos.y<=max_jump_height) {
				if (next_platform==-1 || platforms[i].pos.y<platforms[next_platform].pos.y) next_platform=i;
			}
		}
	if (next_platform==-1) {
		enemy->State=Waiting; return;
	}
	}
	dest_platform.x=platforms[next_platform].pos.x;
	dest_platform.y=platforms[next_platform].pos.y;
	dest_platform.w=platforms[next_platform].pos.w;
	dest_platform.h=platforms[next_platform].pos.h;

	/*
		If the enemy is underneath the platform, do not jump straight up.
		Run out from under it first, then approach from the nearest side.
	*/
	if (enemy->EnPos.y>dest_platform.y &&
		enemy->EnPos.x+enemy->EnPos.w>dest_platform.x &&
		enemy->EnPos.x<dest_platform.x+dest_platform.w) {

		if (enemy->EnPos.x+enemy->EnPos.w/2 < dest_platform.x+dest_platform.w/2) {
			enemy->ai_direction=0;
		}
		else {
			enemy->ai_direction=1;
		}

		enemy->State=Following;
		return;
	}

	if (dest_platform.x>enemy->EnPos.x+enemy->EnPos.w) enemy->ai_direction=1;
			else {
				if (dest_platform.x+dest_platform.w<enemy->EnPos.x) enemy->ai_direction=0;
			}


	int enemy_center=enemy->EnPos.x+enemy->EnPos.w/2;
	int platform_center=dest_platform.x+dest_platform.w/2;

	if ((enemy->ai_direction==0 && enemy_center>platform_center+jump_start_distance) || (enemy->ai_direction==1 && enemy_center<platform_center-jump_start_distance)){enemy->State=Following;return; }

	enemy->State=Jumping; return;

}

void update_enemy_State(Enemy *enemy, Player player, Platform platforms[], SDL_Rect camera, int level) {
	int distance;
	player.pos_screen.x+=camera.x;
	/*player.pos_screen.y+=camera.y;*/
	
	if (enemy->EnPos.x>player.pos_screen.x+player.pos_screen.w) distance= enemy->EnPos.x - player.pos_screen.x;
	else {
		if (enemy->EnPos.x<=player.pos_screen.x+player.pos_screen.w) distance= player.pos_screen.x - enemy->EnPos.x;
		else distance=0;
		}
	if (enemy->startup_moving_state) {
		if (distance<=700) {
			enemy->startup_moving_state=0; 
			enemy->State=Following; 
		}
		else enemy->State=Moving; 
		return; 
	}
	
	//changed this if blocks
	if (distance>1000 || player.life<0)
		{
		if (enemy->State==Moving && SDL_GetTicks()-enemy->movingStateOnTime<=10000) return;
		enemy->State=Waiting;
		enemy->sound_played=0;
		return;
		}

	// Collision says the enemy landed on a platform. End the jump before
	// update_enemy() has a chance to request another jump cycle.
	if (enemy->jump_var.is_jumping && enemy->platform_collision.on_top==1) {
		enemy->jump_var.is_jumping=0;
		enemy->jump_var.x=-100;
		enemy->jump_var.y=0;
		enemy->EnPos.y=enemy->platform_collision.y-enemy->EnPos.h;
		if (enemy->cycle%2==0) enemy->cycle_requested=Idle_Right;
		else enemy->cycle_requested=Idle_Left;
		enemy->State=Waiting;
		return;
	}

	if (enemy->jump_var.is_jumping) {
	enemy->State=Jumping; return; }

	// If the jump physics finished, let the current Jump cycle switch back to Idle/Run
	// before platform navigation is allowed to request another jump.
	if ((enemy->cycle==Jump_Right || enemy->cycle==Jump_Left) && enemy->jump_var.is_jumping==0) {
		enemy->State=Waiting;
		return;
	}

	if (!enemy->jump_var.is_jumping && (enemy->EnPos.x-camera.x<=0 || enemy->EnPos.x+enemy->EnPos.w-camera.x>=SCREEN_WIDTH)) {
		enemy->State=Waiting;
		enemy->sound_played=0;
		return;
	}

	// Platform collision may have already requested an automatic edge jump.
	// Preserve that request instead of letting platform navigation overwrite it.
	if ((enemy->cycle_requested==Jump_Right || enemy->cycle_requested==Jump_Left) && enemy->jump_var.is_jumping==0 && enemy->cycle!=Jump_Right && enemy->cycle!=Jump_Left) {
		enemy->State=Jumping;
		return;
	}

	/*
		Level 2 can chase vertical platform differences. Level 1 ignores
		X-distance detection while the player is above or below the enemy.
	*/
	if ((player.pos_screen.y+player.pos_screen.h <enemy->EnPos.y) || (player.pos_screen.y+player.pos_screen.h>enemy->EnPos.y+enemy->EnPos.h) ) {
		if (level==2) { navigate_to_platform(enemy, platforms, player.pos_screen);return;}
		if (enemy->State==Moving && SDL_GetTicks()-enemy->movingStateOnTime>10000) {enemy->State=Waiting; enemy->sound_played=0;}
		if (enemy->State==Following || enemy->State==Attacking) {enemy->State=Waiting; enemy->sound_played=0;}
		return;

		}

	switch (enemy->State) {
		case Waiting:
			if (distance<=150) enemy->State=Attacking; //changed this
			else {
				if (distance<=700) {
					enemy->State=Following;
					if (enemy->sound_played!=2) {
						Mix_PlayChannel(-1, enemy->hey_sound, 0);
						enemy->sound_played=2;
					}
				}
				else {
					if (distance<=1000) {
						enemy->State=Moving;
						enemy->movingStateOnTime=SDL_GetTicks();
						if (enemy->sound_played!=1) {
							Mix_PlayChannel(-1, enemy->huh_sound, 0);
							enemy->sound_played=1;
						}

					}
				}
			}
			break;
		case Moving:
			enemy->movingStateOnTime=SDL_GetTicks();
			if (distance<=700) {
				enemy->State=Following;
				if (enemy->sound_played!=2) {
					Mix_PlayChannel(-1, enemy->hey_sound, 0);
					enemy->sound_played=2;
				}
			}
			else {
				if (enemy->sound_played!=1) {
					Mix_PlayChannel(-1, enemy->huh_sound, 0);
					enemy->sound_played=1;
				}
			}
			break;
		case Following:
			if (distance<=150 && (player.pos_screen.y+player.pos_screen.h-enemy->EnPos.y>=-10 && player.pos_screen.y+player.pos_screen.h-enemy->EnPos.y<=10)) enemy->State=Attacking; //changed this
			break;

		case Attacking:
			if (distance>1000) enemy->State=Waiting;

			//printf("%d\n", enemy->State);

			break;
		case Jumping:
			if ((enemy->cycle_requested==Jump_Right || enemy->cycle_requested==Jump_Left) && enemy->jump_var.is_jumping==0) enemy->State=Waiting;
			break;
	}

}

void update_enemy(Enemy *enemy, SDL_Rect player_pos, int level, SDL_Rect camera) {
	player_pos.x+=camera.x;
	//player_pos.y+=camera.y;
	if (enemy->health_status==0) {
		if (enemy->cycle%2==0 && enemy->cycle_requested!=Death_Right) enemy->cycle_requested=Death_Right;
		else {
			if (enemy->cycle%2==0 && enemy->cycle_requested!=Death_Left)enemy->cycle_requested=Death_Left;
		}
		return;
	}
	if (enemy->hurt_animation) return;
	switch (enemy->State) {
		case Waiting:
			if ( enemy->cycle %2==0 && enemy->cycle_requested!=Idle_Right && !enemy->hurt_animation) enemy->cycle_requested=Idle_Right;
			else {
				if ( enemy->cycle %2!=0 && enemy->cycle_requested!=Idle_Left && !enemy->hurt_animation) enemy->cycle_requested=Idle_Left;
			}
			break;
		case Moving:
			if (level==1) random_movement_level1(enemy);
			else random_movement_level2(enemy);
			break;

		case Following:
			if ((enemy->cycle_requested==Jump_Right || enemy->cycle_requested==Jump_Left) && enemy->jump_var.is_jumping==0 && enemy->cycle!=Jump_Right && enemy->cycle!=Jump_Left) break;
			 if ((player_pos.y+player_pos.h<enemy->EnPos.y) || (player_pos.y+player_pos.h>enemy->EnPos.y+enemy->EnPos.h)) {
				if (enemy->ai_direction==1 && enemy->cycle_requested!=Run_Right) enemy->cycle_requested=Run_Right;
					else {
				if (enemy->ai_direction==0 && enemy->cycle_requested!=Run_Left) enemy->cycle_requested=Run_Left;
					}
					break;
			 }
			if ((enemy->EnPos.x>player_pos.x+player_pos.w) && enemy->cycle_requested!=Run_Left)  enemy->cycle_requested=Run_Left;
			else {
				if ((enemy->EnPos.x<=player_pos.x+player_pos.w) && enemy->cycle_requested!=Run_Right) enemy->cycle_requested=Run_Right;
				}

			break;
		case Attacking:
			if ((enemy->EnPos.x>player_pos.x+player_pos.w) && enemy->cycle_requested!=Attack_Left)  {
				enemy->cycle_requested=Attack_Left;
				return; }
			else {
				if ((enemy->EnPos.x<=player_pos.x+player_pos.w) && enemy->cycle_requested!=Attack_Right)
					{
					enemy->cycle_requested=Attack_Right;
					return;
					}
				}
			if ((enemy->EnPos.x>player_pos.x+player_pos.w) && !enemy->is_attacking) enemy->cycle_requested=Idle_Left;
			else {
				if ((enemy->EnPos.x<=player_pos.x+player_pos.w) && !enemy->is_attacking) enemy->cycle_requested=Idle_Right;
			}
			break;
		case Jumping:
			if ((enemy->cycle_requested==Jump_Right || enemy->cycle_requested==Jump_Left) && enemy->jump_var.is_jumping==0 && enemy->cycle!=enemy->cycle_requested) break;
			if (enemy->jump_var.is_jumping==0 && (enemy->cycle==Jump_Right || enemy->cycle==Jump_Left)) {
				if (enemy->cycle==Jump_Right) enemy->cycle_requested=Idle_Right;
				else enemy->cycle_requested=Idle_Left;
			}
			else {
				if ( enemy->ai_direction==1 && enemy->cycle_requested!=Jump_Right && !enemy->hurt_animation) enemy->cycle_requested=Jump_Right;
				else {
					if ( enemy->ai_direction==0 && enemy->cycle_requested!=Jump_Left && !enemy->hurt_animation) enemy->cycle_requested=Jump_Left;
				}
			}
			break;



	}
}


int choose_enemy_target(Enemy *enemy, Player players[], int num_players, int split_mode, SDL_Rect camera1, SDL_Rect camera2) {
	if (num_players!=2 || !split_mode) return 0;
	if (players[0].life<=0 && players[1].life>0) return 1;
	if (players[1].life<=0) return 0;
	
	SDL_Rect player0_world = {
		players[0].pos_screen.x + camera1.x, players[0].pos_screen.y + camera1.y, players[0].pos_screen.w, players[0].pos_screen.h
	};
	SDL_Rect player1_world = {
		players[1].pos_screen.x + camera2.x,
		players[1].pos_screen.y + camera2.y,
		players[1].pos_screen.w,
		players[1].pos_screen.h
	};
	
	int enemy_center_x = enemy->EnPos.x + enemy->EnPos.w / 2;
	int enemy_center_y = enemy->EnPos.y + enemy->EnPos.h / 2;

	int player0_dx = enemy_center_x - (player0_world.x + player0_world.w / 2);
	int player0_dy = enemy_center_y - (player0_world.y + player0_world.h / 2);

	int player1_dx = enemy_center_x - (player1_world.x + player1_world.w / 2);
	int player1_dy = enemy_center_y - (player1_world.y + player1_world.h / 2);

	int player0_distance = player0_dx * player0_dx + player0_dy * player0_dy;
	int player1_distance = player1_dx * player1_dx + player1_dy * player1_dy;

	
	if (player1_distance<player0_distance) return 1;
	return 0;
}

void display_ES(SDL_Renderer *renderer, Entity coins[], int num_coins, SDL_Rect camera){
	/* [CAMERA INTEGRATION] camera parameter added so coins render at the correct
	 * screen position. coins[i].pos stores world coordinates and is never changed
	 * here — collision code that reads it remains unaffected. */
	for (int i=0; i<num_coins; i++) {
		if (coins[i].status==0) continue; 
		/* [CAMERA INTEGRATION] Compute screen-space rect by subtracting camera offset */
		SDL_Rect screen_pos = {
			coins[i].pos.x - camera.x,
			coins[i].pos.y - camera.y,
			coins[i].pos.w,
			coins[i].pos.h
		};
		SDL_RenderCopy(renderer, coins[i].image, NULL, &screen_pos); 
		
	}
	
}

void enemy_events(SDL_Event event, Enemy *enemy){
	if (enemy->is_dead==1) return; 

	switch (event.type) {	 
			case SDL_KEYDOWN: {
				switch (event.key.keysym.sym) {
					//Attack cycle event handling
					case SDLK_x: 
						if (enemy->cycle%2==0)	enemy->cycle_requested=8;  
						else  enemy->cycle_requested=9;
						break; 
					//jump cycle event handling
					case SDLK_UP: 
						if (enemy->cycle%2==0)	enemy->cycle_requested=6;  
						else  enemy->cycle_requested=7;
						break;  
				}
				break; 
			} 
			case SDL_KEYUP:
				if (enemy->jump_var.is_jumping==0 && enemy->is_attacking==0 && enemy->hurt_animation==0 && enemy->death_animation==0){
					if (enemy->cycle%2==0)	enemy->cycle_requested=0;  
					else  enemy->cycle_requested=1; 

					}
				break; 

        
        }
        		
        		
}

void enemy_keyboard_events(Enemy *enemy, const Uint8 *keyboard_keys){
	if (enemy->is_dead==1) return; 
	if ((keyboard_keys[SDL_SCANCODE_RIGHT] && keyboard_keys[SDL_SCANCODE_UP]) || (keyboard_keys[SDL_SCANCODE_LEFT] && keyboard_keys[SDL_SCANCODE_UP])) return; 
	//walk cycle
	if (enemy->death_animation==0 && enemy->hurt_animation==0 && enemy->jump_var.is_jumping==0) {
		if (keyboard_keys[SDL_SCANCODE_RIGHT] && !keyboard_keys[SDL_SCANCODE_S] && enemy->cycle!=2) enemy->cycle_requested=2; 
		else {
		if (keyboard_keys[SDL_SCANCODE_LEFT] && !keyboard_keys[SDL_SCANCODE_S] && enemy->cycle!=3) enemy->cycle_requested=3; 
		}
		}			      		
        		
        		
	//Run cycle 
	if (enemy->death_animation==0 && enemy->hurt_animation==0 && enemy->jump_var.is_jumping==0) {
		if (keyboard_keys[SDL_SCANCODE_RIGHT] && keyboard_keys[SDL_SCANCODE_S] && enemy->hurt_animation==0 && enemy->jump_var.is_jumping==0) {
			enemy->cycle_requested=4; 
			if (keyboard_keys[SDL_SCANCODE_A]) enemy->accelerate=1; 
			else {
				enemy->accelerate=0;
				if (keyboard_keys[SDL_SCANCODE_D]) enemy->deccelerate=1; 
				else enemy->deccelerate=0;
			}	
				
			}
		else {
			if (keyboard_keys[SDL_SCANCODE_LEFT] && keyboard_keys[SDL_SCANCODE_S] && enemy->hurt_animation==0 && enemy->jump_var.is_jumping==0) {
			enemy->cycle_requested=5; 
			if (keyboard_keys[SDL_SCANCODE_A]) enemy->accelerate=1; 
			else {
				enemy->accelerate=0;
				if (keyboard_keys[SDL_SCANCODE_D]) enemy->deccelerate=1; 
				else enemy->deccelerate=0;
			}	
				
			}
		}  
		} 		        	
}

void random_movement_level1(Enemy *enemy){
	int current_time=SDL_GetTicks(), cycle_number; 
	int animation_on= (
			    enemy->hurt_animation==1
			|| enemy->death_animation==1
			|| enemy->is_attacking==1
			|| enemy->jump_var.is_jumping==1
			); 
			
	if (animation_on) {
		enemy->random_mv_var.first_rand_switch=1; 
		return;
	} 
	else {	
		//Switching to random mode
		if (enemy->random_mv_var.first_rand_switch==1) {
			enemy->random_mv_var.pos_min=enemy->EnPos.x-200; 
			enemy->random_mv_var.pos_max=enemy->EnPos.x+200; 
			//The special case where the sprite goes beyond the screen, this will bring it back once random mode is on
			if (enemy->random_mv_var.pos_min<=0) {
				enemy->random_mv_var.pos_min=0; 
				enemy->random_mv_var.pos_max=400; 
			}
			else {
				if (enemy->random_mv_var.pos_max>SCREEN_WIDTH) {
				enemy->random_mv_var.pos_min=SCREEN_WIDTH-400; 
				enemy->random_mv_var.pos_max=SCREEN_WIDTH; 
				}
			}
			enemy->random_mv_var.control_off_time=current_time;
			enemy->random_mv_var.first_rand_switch=0;
			enemy->random_mv_var.random_started=1;   
		}
		//Random movement starts 5 seconds after random mode intiates
		if (current_time-enemy->random_mv_var.control_off_time>=5000) {
			if (enemy->random_mv_var.random_started){
				if (enemy->cycle%2==0) {
					enemy->cycle_requested=2;
					} 
				else {
					if (enemy->cycle%2!=0) enemy->cycle_requested=3; 							 
				}
					enemy->random_mv_var.random_started=0; 
				}
			else {
				if (enemy->EnPos.x>enemy->random_mv_var.pos_max || enemy->EnPos.x+enemy->EnPos.w>=SCREEN_WIDTH) enemy->cycle_requested=3; 
					
				
				else {
					if (enemy->EnPos.x<enemy->random_mv_var.pos_min || enemy->EnPos.x<=0) enemy->cycle_requested=2;  
					}
				}					
		
	}
	
}
}

void random_movement_level2(Enemy *enemy){
	int current_time=SDL_GetTicks(), cycle_number; 
	int animation_on= (
			    enemy->hurt_animation==1
			|| enemy->death_animation==1
			|| enemy->is_attacking==1
			|| enemy->jump_var.is_jumping==1
			); 
			
	if (animation_on) {
		enemy->random_mv_var.first_rand_switch=1; 
		return;
	} 
	else {	
		//Switching to random mode
		if (enemy->random_mv_var.first_rand_switch==1) {
			enemy->random_mv_var.pos_min=enemy->EnPos.x-200; 
			enemy->random_mv_var.pos_max=enemy->EnPos.x+200; 
			//The special case where the sprite goes beyond the screen, this will bring it back once random mode is on
			if (enemy->random_mv_var.pos_min<=0) {
				enemy->random_mv_var.pos_min=0; 
				enemy->random_mv_var.pos_max=400; 
			}
			else {
				if (enemy->random_mv_var.pos_max>SCREEN_WIDTH) {
				enemy->random_mv_var.pos_min=SCREEN_WIDTH-400; 
				enemy->random_mv_var.pos_max=SCREEN_WIDTH; 
				}
			}
			enemy->random_mv_var.control_off_time=current_time;
			enemy->random_mv_var.first_rand_switch=0;
			enemy->random_mv_var.random_started=1;   
		}
		//Random movement starts 5 seconds after random mode intiates
		if (current_time-enemy->random_mv_var.control_off_time>=5000) {
					 
			if (current_time - enemy->random_mv_var.last_random_switch_time>=2000){
				cycle_number=rand()%4; 
				if (enemy->EnPos.x+enemy->EnPos.w>=SCREEN_WIDTH) enemy->cycle_requested=3; 
				
				else {
					if (enemy->EnPos.x<=0) enemy->cycle_requested=2;
					else {
						switch (cycle_number){
							case 0: 
								enemy->cycle_requested=0; 
								enemy->cycle_switch=1; 
								break; 
							case 1: 
								enemy->cycle_requested=1; 
								enemy->cycle_switch=1; 
								break; 
							case 2: 
								enemy->cycle_requested=2; 
								enemy->cycle_switch=1; 
								break;
							case 3: 
								enemy->cycle_requested=3; 
								enemy->cycle_switch=1; 
								break;  	
									
						}
						}
					}
				
				enemy->random_mv_var.last_random_switch_time=current_time; 
			}
			
		
	}
	
}
}

void set_enemy_cycle(Enemy *enemy){

	if (enemy->hurt_animation && enemy->cycle_requested!=10 && enemy->cycle_requested!=11) enemy->hurt_animation=0; 
	if (enemy->is_attacking && enemy->cycle_requested!=8 && enemy->cycle_requested!=9) enemy->is_attacking=0; 
	if (enemy->death_animation && enemy->cycle_requested!=12 && enemy->cycle_requested!=13) enemy->death_animation=0; 
	
	//Death cycle
	if (enemy->cycle_requested==12 && enemy->cycle!=12) { 
	enemy->cycle=12; 
	enemy->lastFrameSwitchTime=SDL_GetTicks(); 
	enemy->cycle_switch=1; 
	return; 
	}
	else 
	{
		if (enemy->cycle_requested==13 && enemy->cycle!=13) {
		enemy->cycle=13; 
		enemy->lastFrameSwitchTime=SDL_GetTicks(); 
		enemy->cycle_switch=1; 
		return;  
		}
	} 
	if (enemy->cycle==12 || enemy->cycle==13) return;

	//Idle cycle		 
	if (enemy->cycle_requested==0 && enemy->cycle!=0) {
		enemy->cycle=0; enemy->cycle_switch=1;
		enemy->lastFrameSwitchTime=SDL_GetTicks(); 

	}
	else {
		if (enemy->cycle_requested==1 && enemy->cycle!=1) {
		enemy->cycle=1; enemy->cycle_switch=1;
		enemy->lastFrameSwitchTime=SDL_GetTicks(); 
		}        
	}
	
	//walk cycle
	if (enemy->cycle_requested==2 && enemy->cycle!=2) 
	 {
		enemy->cycle_switch=1;
		enemy->cycle=2;
		enemy->lastFrameSwitchTime=SDL_GetTicks(); 
	
			}
	else {
		if (enemy->cycle_requested==3 && enemy->cycle!=3) { 
				enemy->cycle_switch=1;
				enemy->cycle=3;
				enemy->lastFrameSwitchTime=SDL_GetTicks(); 
			
		}
	
	}
        		
        		
        		
        //Run cycle event handling
        if (enemy->cycle_requested==4 && enemy->cycle!=4) {
			 
				enemy->cycle_switch=1;
				enemy->cycle=4; 
				enemy->lastFrameSwitchTime=SDL_GetTicks();
				enemy->acceleration=0; 
			}

	else {

		if (enemy->cycle_requested==5 && enemy->cycle!=5) 
		{ 
			enemy->cycle_switch=1;
			enemy->cycle=5;
			enemy->lastFrameSwitchTime=SDL_GetTicks();
			enemy->acceleration=0; 
		}
	
		}
		
        		
        			
	
	//Attack cycle 
	if (enemy->cycle_requested==8 && enemy->cycle!=8) {
		
		enemy->cycle=8; 
		enemy->cycle_switch=1;
		enemy->lastFrameSwitchTime=SDL_GetTicks(); 
		if (enemy->is_attacking==0) enemy->is_attacking=1;
			
	}
	else {
		
		if (enemy->cycle_requested==9 && enemy->cycle!=9){
			enemy->cycle=9; 
			enemy->cycle_switch=1;
			enemy->lastFrameSwitchTime=SDL_GetTicks();
			if (enemy->is_attacking==0) enemy->is_attacking=1; 
			}
	 }     
	
	//jump cycle
	if (enemy->cycle_requested==6 && enemy->cycle!=6 && enemy->jump_var.is_jumping==0) {
		enemy->cycle=6;
		enemy->jump_var.x=-100;
		enemy->jump_var.y=0;
		enemy->jump_var.is_jumping=1;
		enemy->lastFrameSwitchTime=SDL_GetTicks(); }
	else {
		if (enemy->cycle_requested==7 && enemy->cycle!=7 && enemy->jump_var.is_jumping==0) {
		enemy->cycle=7;
		enemy->jump_var.x=100;
		enemy->jump_var.y=0;
		enemy->jump_var.is_jumping=1;
		enemy->lastFrameSwitchTime=SDL_GetTicks(); 
	} }
			     
		
		
	//Hurt Cycle 
		if (enemy->cycle_requested==10 && enemy->cycle!=10) { 
			enemy->cycle=10; 
			enemy->lastFrameSwitchTime=SDL_GetTicks(); 
			enemy->cycle_switch=1;  
			
			if (!enemy->coin_collision.status) enemy->EnPos.x-=50; 
			else enemy->coin_collision.status=0; 
			}
		else 
		{
			if (enemy->cycle_requested==11 && enemy->cycle!=11) {
			enemy->cycle=11; 
			enemy->lastFrameSwitchTime=SDL_GetTicks(); 
			enemy->cycle_switch=1;  
			
			if (!enemy->coin_collision.status) enemy->EnPos.x+=50; 
			else enemy->coin_collision.status=0; 
			}
			}
		  		
}

void move_enemy(Enemy *enemy, int dt){
	switch (enemy->cycle) { 
		case 0: 
		case 1:  
		idle_enemy(enemy); 
		break;
		
		case 2: 
		case 3:  
		walk_enemy(enemy); 
		break; 
		
		case 4: 
		case 5: 
		run_enemy(enemy, dt); 
		break; 
		
		case 6: 
		case 7: 
		jump_enemy(enemy, dt); 
		break;  
		
		case 8: 
		case 9:
		attack_enemy(enemy); 
		break;   
		
		case 10: 
		case 11:
		hurt_enemy(enemy); 
		break; 
		
		case 12: 
		case 13:
		death_enemy(enemy); 
		break;       
	}
}


void idle_enemy(Enemy *enemy){

	//If the enemy jumps on top of a platform, the Idle cycle is requested and the jump cycle is interrupted and
	if (enemy->platform_collision.on_top==1 && enemy->jump_var.is_jumping==1) {
		enemy->jump_var.is_jumping=0; 
		enemy->jump_var.x=-100; 
		enemy->jump_var.y=0; 
		enemy->EnPos.y=enemy->platform_collision.y - enemy->EnPos.h;  
	} 

	if (enemy->cycle_switch==1) {
		enemy->frame=0; 
		enemy->cycle_switch=0; 
		return; 
		
	}
	if (SDL_GetTicks()-enemy->lastFrameSwitchTime>=100){
	
		if (enemy->frame==5) enemy->frame=0; 
		else enemy->frame++; 
		
		enemy->lastFrameSwitchTime=SDL_GetTicks(); 
	}
	
	
}


void walk_enemy(Enemy *enemy){

	if (enemy->cycle_switch==1) {
		enemy->frame=0; 
		enemy->cycle_switch=0; 
		return; 
		
	}
	if (SDL_GetTicks()-enemy->lastFrameSwitchTime>=100){
	
		if (enemy->frame==5) enemy->frame=0; 
		else enemy->frame++; 
		
		enemy->lastFrameSwitchTime=SDL_GetTicks(); 
	}
	
	if (enemy->cycle==2) enemy->EnPos.x++; 
	else enemy->EnPos.x--; 
	
}

void run_enemy(Enemy *enemy, int dt){
	
	float dx; 
	
	if (enemy->cycle_switch==1) {
		enemy->frame=0; 
		enemy->cycle_switch=0; 
		return; 
	}
	
	if (SDL_GetTicks()-enemy->lastFrameSwitchTime>=100){
	
		if (enemy->frame==5) enemy->frame=0; 
		else enemy->frame++; 
		
		enemy->lastFrameSwitchTime=SDL_GetTicks(); 
	}
	if (enemy->accelerate) enemy->acceleration+=0.005; 
	else {
		if (enemy->deccelerate) enemy->acceleration-=0.01;
		else enemy->acceleration-=0.001;
	}
	if (enemy->acceleration<0) enemy->acceleration=0; 
	dx=0.5*enemy->acceleration*dt*dt + enemy->velocity*dt; 
	if (enemy->cycle==4) enemy->EnPos.x+=(int)dx; 
	else enemy->EnPos.x-=(int)dx; 
	
}

void attack_enemy(Enemy *enemy){
	int i=0; 
	
	if (enemy->is_attacking==0) return; 
	
	if (enemy->cycle_switch==1) {
		enemy->frame=0; 
		enemy->cycle_switch=0; 
		return; 
	}
	
	if (SDL_GetTicks()-enemy->lastFrameSwitchTime>=80){
	if (enemy->frame==5) {
	
		if (enemy->cycle%2==0) enemy->cycle_requested=0; 
		else enemy->cycle_requested=1;  
		enemy->is_attacking=0; 
	}
	else {
		enemy->frame++;  
		if (enemy->frame==4) enemy->first_throw_frame=1;
		
		}
	enemy->lastFrameSwitchTime=SDL_GetTicks(); 
	}
	if (enemy->frame==4) {
		enemy->weapon_fired=1;
		while (i<num_weapons && enemy->weapons[i].status!=0) i++; 
		if (i<num_weapons && enemy->first_throw_frame) {
			Mix_PlayChannel(-1, enemy->shoot_sound, 0);
			enemy->weapons[i].status=1; 
			enemy->first_throw_frame=0; 
			enemy->weapons[i].pos.y=enemy->EnPos.y+42; 
			if (enemy->cycle==8) enemy->weapons[i].direction=0; 
			else {
				if (enemy->cycle==9) enemy->weapons[i].direction=1;
			}
			if (enemy->weapons[i].direction==0) enemy->weapons[i].pos.x=enemy->EnPos.x + enemy->EnPos.w +2; 
			else enemy->weapons[i].pos.x=enemy->EnPos.x-2;
		} 
		}
}

void update_enemy_weapons(Enemy *enemy, int map_w){
	int active_weapon=0;
	
	for (int i=0; i<num_weapons; i++) {
		if (enemy->weapons[i].status==0) continue;
		
		if (enemy->weapons[i].direction==0) enemy->weapons[i].pos.x+=15; 
		else {
			if (enemy->weapons[i].direction==1) enemy->weapons[i].pos.x-=15; 
		}
		
		if (enemy->weapons[i].pos.x > map_w || enemy->weapons[i].pos.x + enemy->weapons[i].pos.w < 0) {
			enemy->weapons[i].status=0;
			continue;
		}
		
		active_weapon=1;
	}
	
	enemy->weapon_fired=active_weapon;
}

void fire_weapon_enemy(Enemy *enemy, SDL_Renderer *renderer, SDL_Rect camera){
	/* [CAMERA INTEGRATION] camera parameter added. Weapon positions (pos.x/y) are
	 * kept in world space for movement and collision. Only rendering uses the offset. */
	if (enemy->weapon_fired==0) return; 
	
	for (int i=0; i<num_weapons; i++) {
		if (enemy->weapons[i].status==0) continue; 
		
		//HERE rendering only offsets world-space weapon positions; it never moves them
		SDL_Rect wscreen = {
			enemy->weapons[i].pos.x - camera.x,
			enemy->weapons[i].pos.y - camera.y,
			enemy->weapons[i].pos.w,
			enemy->weapons[i].pos.h
		};
		SDL_RenderCopy(renderer, enemy->weapons[i].image, NULL, &wscreen);
	}
}

void jump_enemy(Enemy *enemy, int dt){
	float dx, v0=0.5, prev_y=enemy->jump_var.y, x_collision, y_collision; 
	
	if (enemy->jump_var.is_jumping==0) return; 
	
	dx=v0*dt; 
	
	if (enemy->cycle % 2==0) enemy->jump_var.x+= dx;
	else  enemy->jump_var.x-= dx;
	
	if (enemy->cycle % 2==0) enemy->EnPos.x+=(int)dx;
	else enemy->EnPos.x-=(int)dx; 
	
	//side collision	
	if (enemy->jump_var.temp_equation==1) 
		{
		if ((enemy->platform_collision.status==1 && enemy->platform_collision.first_contact==1) || (enemy->platform_collision.platform_type==2 && enemy->jump_var.was_on_platform)) {
			enemy->platform_collision.x= (int)enemy->jump_var.x; 
			enemy->platform_collision.y=(int)enemy->jump_var.y;
			enemy->platform_collision.first_contact=0;  	
		}
	
		
		
		enemy->jump_var.y=-0.035 * (enemy->jump_var.x - enemy->platform_collision.x) * (enemy->jump_var.x-enemy->platform_collision.x) +enemy->platform_collision.y;
		}
	else enemy->jump_var.y=-0.035 * enemy->jump_var.x * enemy->jump_var.x +350;  
	
	enemy->EnPos.y-=(int)(enemy->jump_var.y - prev_y); 

	
	if (enemy->jump_var.was_on_platform) enemy->jump_var.was_on_platform=0; 
	if (enemy->jump_var.was_on_obstacle) enemy->jump_var.was_on_obstacle=0;
	
	if (enemy->jump_var.x>-80 && enemy->jump_var.x< 80) enemy->frame=2; 
	else {
		if ((enemy->jump_var.x>=-100 && enemy->jump_var.x<=-80 ) || (enemy->jump_var.x<=100 && enemy->jump_var.x>=80) )enemy->frame=1;
		
	}


		
		
		//The enemy makes it to ground level
		if (enemy->EnPos.y>=ground_level_enemy-enemy->EnPos.h){
			enemy->jump_var.is_jumping=0; 
			enemy->jump_var.x=-100; 
			enemy->jump_var.y=0; 
			enemy->EnPos.y=ground_level_enemy-enemy->EnPos.h;
			 if (enemy->death_animation==0) {
				if (enemy->cycle %2 ==0) enemy->cycle_requested=0; 
				else enemy->cycle_requested=1;  
				
			}
			if (enemy->jump_var.temp_equation) enemy->jump_var.temp_equation=0; 
			
		}
			
}

void collision_enemy_platforms(Enemy *enemy, Platform platforms[]){
	int found_collision=0, dx, dy; 
	
	for (int i=0; i<MAX_PLATFORMS; i++) {
		if (platforms[i].status==0) continue; 
		
		if ((platforms[i].pos.x + platforms[i].pos.w < enemy->EnPos.x) || (platforms[i].pos.x> enemy->EnPos.x + enemy->EnPos.w) || (platforms[i].pos.y + platforms[i].pos.h < enemy->EnPos.y) || (platforms[i].pos.y> enemy->EnPos.y + enemy->EnPos.h)) continue;  
		else {
			found_collision=1; 
			enemy->platform_collision.platform_type=platforms[i].type; 
			enemy->platform_collision.first_contact= (enemy->platform_collision.status == 0); 
			enemy->platform_collision.status=1;
			enemy->platform_collision.on_top=0; 
			//If the enemy lands on top of the platform
			if ((platforms[i].pos.y<= enemy->EnPos.y + enemy->EnPos.h) && (platforms[i].pos.y> enemy->EnPos.y) && (enemy->EnPos.x>platforms[i].pos.x) && (enemy->EnPos.x+enemy->EnPos.w< platforms[i].pos.x + platforms[i].pos.w)) {
				enemy->platform_collision.on_top=1;
				enemy->platform_collision.y= platforms[i].pos.y;
			
				
				
				if (enemy->platform_collision.first_contact==1 && !enemy->jump_var.was_on_platform){ 
					if (enemy->cycle %2 ==0) enemy->cycle_requested=0; 
					else enemy->cycle_requested=1;
				}
				enemy->jump_var.was_on_platform=1; 
				if (platforms[i].type==1 && (enemy->cycle_requested==0 || enemy->cycle_requested==1)) {
					dx=platforms[i].pos.x-platforms[i].prev_x; 
					dy=platforms[i].pos.y-platforms[i].prev_y; 
					enemy->EnPos.x+=dx; enemy->EnPos.y+=dy;
				}
				if (platforms[i].type==2) {platforms[i].status=0;} //remove this after integration
			}
			else{	//collision from the sides or bottom
				if (enemy->jump_var.is_jumping==1) {
					if (enemy->cycle %2 ==0) enemy->cycle_requested=6; 
					else enemy->cycle_requested=7; 
					enemy->jump_var.temp_equation=1; 
				}
							}
			break; 
		}
	
	}
	if (!found_collision) {
		enemy->platform_collision.status=0;
		enemy->platform_collision.first_contact=0;
		enemy->platform_collision.on_top=0;
		if (enemy->jump_var.was_on_platform==1 && enemy->platform_collision.status==0 && enemy->jump_var.is_jumping==0) {			
			if (enemy->cycle %2 ==0) enemy->cycle_requested=6; 
			else enemy->cycle_requested=7; 
			
			}
		if (enemy->platform_collision.platform_type==2 && enemy->jump_var.was_on_platform){
			enemy->jump_var.temp_equation=1; 
			if (enemy->cycle %2 ==0) enemy->cycle_requested=6; 
			else enemy->cycle_requested=7;
		}
	
		
	}
}

void collision_player_enemy(Enemy *enemy, Player player, SDL_Rect camera){
	SDL_Rect player_world = {
		player.pos_screen.x + camera.x,
		player.pos_screen.y /*+ camera.y*/,
		player.pos_screen.w,
		player.pos_screen.h 
	};

	//TODO: modify the parameter and player filds used here depending on miriam's work
	if ((player_world.x + player_world.w < enemy->EnPos.x) || (player_world.x> enemy->EnPos.x + enemy->EnPos.w) || (player_world.y + player_world.h < enemy->EnPos.y) || (player_world.y> enemy->EnPos.y + enemy->EnPos.h)) {
	
		enemy->player_collision.status=0; 
		if (enemy->player_collision.first_contact==1) enemy->player_collision.first_contact=0; 
	}
	else {
		enemy->player_collision.status=1; 
		if (enemy->player_collision.first_contact==0) {
		enemy->player_collision.first_contact=1; 
		enemy->player_collision.is_hurt=1; 
		}
		else enemy->player_collision.is_hurt=0; 
		
		enemy->player_collision.on_top=0;
		
		if ((player_world.y<= enemy->EnPos.y + enemy->EnPos.h) && (player_world.y> enemy->EnPos.y) && enemy->jump_var.is_jumping==1) {
		enemy->player_collision.on_top=1; 
		enemy->EnPos.y=player_world.y-enemy->EnPos.h; 
		}
		
		
		
	}

}

void collision_enemy_entities(Enemy *enemy, Entity_player Projectile[], Entity coins[], int num_coins){

	for (int i=0; i<num_coins; i++) {
		if (coins[i].status==0) continue; 
		
		if ((coins[i].pos.x + coins[i].pos.w< enemy->EnPos.x) || (coins[i].pos.x> enemy->EnPos.x +enemy->EnPos.w) || (coins[i].pos.y + coins[i].pos.h< enemy->EnPos.y) || (coins[i].pos.y> enemy->EnPos.y + enemy->EnPos.h))enemy->coin_collision.status=0; 
		else {
			enemy->coin_collision.status=1;
			coins[i].contact=1; 
		break;  
	}
	}
	
	//Projectile Collision handling
	for (int i=0; i<num_projectiles; i++) {
		if (Projectile[i].status==1 && Projectile[i].contact==0)
		{
			if (((Projectile[i].pos.x + Projectile[i].pos.w< enemy->EnPos.x) || (Projectile[i].pos.x> enemy->EnPos.x +enemy->EnPos.w) || (Projectile[i].pos.y + Projectile[i].pos.h< enemy->EnPos.y) || (Projectile[i].pos.y> enemy->EnPos.y + enemy->EnPos.h))) enemy->projectile_collision.status=0; 
				else {
					
					Projectile[i].contact=1; 
					if ((Projectile[i].pos.y<= enemy->EnPos.y + enemy->EnPos.h) && (Projectile[i].pos.y> enemy->EnPos.y) && enemy->jump_var.is_jumping==1 ) {
					enemy->projectile_collision.on_top=1; 
					}
					break;  
					

			}}
			}
 
}
SDL_Color GetPixel(SDL_Surface *bg, SDL_Rect pos){

	SDL_Color color = {255, 255, 255, 255}; 
	
	if (pos.x < 0 || pos.x >= bg->w || pos.y < 0 || pos.y >= bg->h) 
	return color; 
	
	Uint8 *pPosition = (Uint8 *)bg->pixels + (pos.y * bg->pitch) + (pos.x * bg->format->BytesPerPixel); 
	
	Uint32 pixel_value = 0; 
	memcpy(&pixel_value, pPosition, bg->format->BytesPerPixel); 
	SDL_GetRGB(pixel_value, bg->format, &color.r, &color.g, &color.b); 
	return color; 

}

void collision_enemy_bg(Enemy *enemy, Background_temp bg){
	SDL_Color c; 
	SDL_Rect pos[8]; int found_collision=0; 
	
	pos[0].x=enemy->EnPos.x; pos[0].y=enemy->EnPos.y; pos[1].x=enemy->EnPos.x+ enemy->EnPos.w/2; pos[1].y=enemy->EnPos.y;
	pos[2].x=enemy->EnPos.x+ enemy->EnPos.w; pos[2].y=enemy->EnPos.y; pos[3].x=enemy->EnPos.x;pos[3].y=enemy->EnPos.y+ enemy->EnPos.h/2;
	pos[4].x=enemy->EnPos.x;pos[4].y=enemy->EnPos.y+ enemy->EnPos.h; pos[5].x=enemy->EnPos.x+ enemy->EnPos.w/2;pos[5].y=enemy->EnPos.y+ enemy->EnPos.h; 
	pos[6].x=enemy->EnPos.x+ enemy->EnPos.w;pos[6].y=enemy->EnPos.y+ enemy->EnPos.h; pos[7].x=enemy->EnPos.x+ enemy->EnPos.w;pos[7].y=enemy->EnPos.y+ enemy->EnPos.h/2;
	
	if (!found_collision) {
		c=GetPixel(bg.mask, pos[5]);
		if (c.r==0 && c.g==0 && c.b==0) {
			enemy->bg_collision.first_contact= (enemy->bg_collision.status == 0);
			enemy->bg_collision.status=1; 
			enemy->bg_collision.on_top=1; 
			enemy->jump_var.was_on_obstacle=1; 
			enemy->bg_collision.y=pos[5].y; 
			if (enemy->jump_var.is_jumping) enemy->jump_var.is_jumping=0; 
			if (enemy->bg_collision.first_contact) {
				if (enemy->cycle %2 ==0) enemy->cycle_requested=0; 
				else enemy->cycle_requested=1;
			}
			found_collision=1; 
		}
	}
		
	if (!found_collision) {
		c=GetPixel(bg.mask, pos[7]);
		if (c.r==0 && c.g==0 && c.b==0) {
			enemy->bg_collision.first_contact= (enemy->bg_collision.status == 0);
			enemy->bg_collision.status=1; 
			enemy->EnPos.x-=20; 
			if (enemy->jump_var.is_jumping) {
				enemy->jump_var.is_jumping=0; enemy->EnPos.y=ground_level_enemy - enemy->EnPos.y; 
			}
			if (enemy->bg_collision.first_contact) enemy->cycle_requested=0;
			found_collision=1;
		}
	}
	
	if (!found_collision) {
		c=GetPixel(bg.mask, pos[3]);
		if (c.r==0 && c.g==0 && c.b==0) {
			enemy->bg_collision.first_contact= (enemy->bg_collision.status == 0);
			enemy->bg_collision.status=1; 
			enemy->EnPos.x+=20; 
			if (enemy->jump_var.is_jumping) {
				enemy->jump_var.is_jumping=0; enemy->EnPos.y=ground_level_enemy - enemy->EnPos.y; 
			}
			if (enemy->bg_collision.first_contact) enemy->cycle_requested=1;
			found_collision=1;
		}
	}
	
	if (!found_collision) {
	c=GetPixel(bg.mask, pos[4]);
	if (c.r==0 && c.g==0 && c.b==0) {
		enemy->bg_collision.first_contact= (enemy->bg_collision.status == 0);
		enemy->bg_collision.status=1; 
		if (enemy->bg_collision.first_contact) enemy->cycle_requested=7; 
		found_collision=1; 
	}
	}
	
	if (!found_collision) {
	c=GetPixel(bg.mask, pos[6]);
	if (c.r==0 && c.g==0 && c.b==0) {
		enemy->bg_collision.first_contact= (enemy->bg_collision.status == 0);
		enemy->bg_collision.status=1; 
		if (enemy->bg_collision.first_contact) enemy->cycle_requested=6; 
		found_collision=1;
	}
	}
	
	if (!found_collision) {
		enemy->bg_collision.status=0;
		enemy->bg_collision.first_contact=0;
		enemy->bg_collision.on_top=0;

		if (enemy->jump_var.was_on_obstacle==1 && enemy->bg_collision.status==0 && enemy->jump_var.is_jumping==0) {			
			if (enemy->cycle %2 ==0) enemy->cycle_requested=6; 
			else enemy->cycle_requested=7; 
			} 
	}
	

}


// check collision // miriam

int check_collision(SDL_Rect a,SDL_Rect b){
	return (a.x<b.x + b.w && a.x + a.w > b.x && a.y < b.y + b.h && a.y + a.h > b.h);
}



void health_management_enemy(Enemy *enemy, Player *player, int level, Entity coins[], int num_coins, SDL_Rect camera){

	collision_enemy_entities(enemy, player->weapons, coins, num_coins); 
	collision_player_enemy(enemy, *player, camera);
	
	SDL_Rect player_world = {
		player->pos_screen.x + camera.x,
		player->pos_screen.y /*+ camera.y*/,
		player->pos_screen.w,
		player->pos_screen.h 
	};
	
	if (enemy->player_collision.status==1 && enemy->player_collision.is_hurt==1 && enemy->health_status>0 && player->life>0) {
		
		enemy->player_collision.is_hurt=0; 
		 
		enemy->health_status--;
	
		Mix_PlayChannel(-1, enemy->hurt_sound, 0);
		
		
		//if the enemy jumps right on top of the player, he'll just bounce right off. Otherwise, hurt animation is on
		switch (enemy->player_collision.on_top) {
			case 0:
				if (enemy->health_status) enemy->hurt_animation=1;
				//if the enemy collides with the player mid jump and he's not on top of the player, he'll go back to the initial y axis and the hurt animation will start (I want the enemy to bounce off the player if he's on top)
				if (enemy->jump_var.is_jumping==1 && enemy->player_collision.status==1) {
					enemy->jump_var.is_jumping=0; 
					enemy->EnPos.y=ground_level_enemy-enemy->EnPos.h; 
					
				}
				if (enemy->cycle % 2==0 && enemy->cycle!=10) enemy->cycle_requested=10; 
				else {
				if (enemy->cycle % 2!=0 && enemy->cycle!=11)enemy->cycle_requested=11; }
				break; 
			case 1:


				if (enemy->cycle % 2==0 && enemy->cycle!=6) enemy->cycle_requested=6; 
				else {
				if (enemy->cycle % 2!=0 && enemy->cycle!=7)enemy->cycle_requested=7; 
				}
				break; 
		}
		
		}
	//Collision with health coins
	for (int i=0; i<num_coins; i++) {
		if (coins[i].status==1 && coins[i].contact==1) {
			if (enemy->health_status<7 && coins[i].type==0) enemy->health_status++; 
			coins[i].status=0;
			coins[i].contact=0;  
			break; 
	}
	}
	
	//Collision with projectiles
	for (int i=0; i<num_projectiles; i++) {
		if (player->weapons[i].status==1 && player->weapons[i].contact==1 && enemy->health_status>0) 
		{
			enemy->health_status--;
			if (enemy->health_status<0) enemy->health_status=0; 
			player->weapons[i].status=0; 
			player->weapons[i].contact=0; 
			if (enemy->projectile_collision.on_top==0) {
				if (enemy->jump_var.is_jumping==1) {
					enemy->jump_var.is_jumping=0; 
					enemy->EnPos.y=ground_level_enemy-enemy->EnPos.h; 
				}
				if (enemy->cycle%2==0) enemy->cycle_requested=10; 
				else enemy->cycle_requested=11;
				if (enemy->health_status) {
					enemy->hurt_animation=1; 
					Mix_PlayChannel(-1, enemy->hurt_sound, 0);}
				}
			else enemy->projectile_collision.on_top=0; 
			break;
		} 
	}
	if (enemy->health_status==0 && enemy->death_animation==0 && enemy->is_dead==0 && enemy->jump_var.is_jumping==0) {
		enemy->death_animation=1;  
		enemy->death_y=enemy->EnPos.y+enemy->EnPos.h; 
		enemy->lastFrameSwitchTime=SDL_GetTicks(); 
		enemy->cycle_switch=1;
		Mix_PlayChannel(-1, enemy->death_sound, 0); 
		if (enemy->cycle % 2==0) 
		{
			enemy->cycle_requested=12; 
			}
		else 
		{
			enemy->cycle_requested=13; 
			}
	
	}
	for(int i=0;i< num_weapons;i++){
		if(enemy->weapons[i].status == 1){
			if(check_collision(enemy->weapons[i].pos, player_world)){
				player_take_damage(player, level);
				enemy->weapons[i].status=0;
			}
		}
	}
	
	for(int i=0; i < num_coins; i++){
		if(coins[i].status == 1 && coins[i].contact==0){
			if(check_collision(coins[i].pos, player_world)){
				player_collect_coin(player,coins[i].type, level);
				coins[i].status=0;
			}
		}
	}
	
}

void hurt_enemy(Enemy *enemy) {

	if (enemy->hurt_animation==0) return; 
	int current_time= SDL_GetTicks(); 
	
	if (enemy->cycle_switch==1) {
		enemy->frame=0; 
		enemy->cycle_switch=0; 
		return; 
		
	}
	
	if (current_time- enemy->lastFrameSwitchTime>=150){	
		
			if (enemy->frame<5) enemy->frame++; 
			else {
				enemy->hurt_animation=0; 
				if (enemy->cycle==10 && enemy->hurt_animation==0) enemy->cycle_requested=0; 
				else {
				if (enemy->cycle==11 && enemy->hurt_animation==0) enemy->cycle_requested=1;
				}
							
				
			}
		 
		enemy->lastFrameSwitchTime= current_time; 
		}
}

void death_enemy(Enemy *enemy) {

	if (enemy->death_animation==0) return; 
        

	int current_time= SDL_GetTicks(); 
	
	if (enemy->cycle_switch==1) {
		enemy->frame=0; 
		enemy->cycle_switch=0; 
		return; 
		
	}
	
	if (current_time- enemy->lastFrameSwitchTime>=150){	

			if (enemy->frame<5) enemy->frame++; 	
			else {
				enemy->death_animation=0; 
				enemy->is_dead=1;
				
				
			}
		 
		enemy->lastFrameSwitchTime= current_time; 
		}
}



void free_enemy(Enemy *enemy){
	
	for (int cycle=0; cycle<14; cycle++) {
		for (int frame=0; frame<6; frame++) SDL_DestroyTexture(enemy->sprite[cycle][frame]); 
	}
	
	for (int bar=0; bar<8; bar++) SDL_DestroyTexture(enemy->status_bar[bar]); 
	for (int i=0; i<num_weapons; i++) SDL_DestroyTexture(enemy->weapons[i].image);
}

void free_ES(Entity coins[], int num_coins){
	for (int i=0; i<num_coins; i++){
		SDL_DestroyTexture(coins[i].image); 
	}
}


//To remove after integration



int init_bg(SDL_Renderer *renderer, Background_temp *bg){


	bg->image=NULL; 
	bg->image=IMG_LoadTexture(renderer, "Assets_Enemy/bg.png"); 
	if (bg->image==NULL){
		printf("Failed loading: %s\n", "Assets_Enemy/bg.png");
    		printf("IMG Error: %s\n", IMG_GetError());
    		return 1;
	}
	
	bg->mask=NULL; 
	bg->mask=IMG_Load("Assets_Enemy/bg_mask.png"); 
	if (bg->mask==NULL){
		printf("Failed loading: %s\n", "Assets_Enemy/bg_mask.png");
    		printf("IMG Error: %s\n", IMG_GetError());
    		return 1;
	}
	
	bg->image_mask= SDL_CreateTextureFromSurface(renderer, bg->mask); 
	if (bg->image_mask==NULL){
		printf("Failed loading: %s\n", "Assets_Enemy/bg_mask.png");
    		printf("IMG Error: %s\n", IMG_GetError());
    		return 1;
	}
	
	for (int i=0; i<num_platforms; i++) { 
		bg->platforms[i].image=NULL; 
		bg->platforms[i].status=1; 
		bg->platforms[i].image=IMG_LoadTexture(renderer, "Assets_Enemy/Platform.png"); 
		if (bg->platforms[i].image==NULL){
			printf("Failed loading: %s\n", "Assets_Enemy/Platform.png");
	    		printf("IMG Error: %s\n", IMG_GetError());
	    		return 1;
		}
		bg->platforms[i].pos.w=180; 
		bg->platforms[i].pos.h=65;
	}
	
	bg->platforms[0].pos.x=300; bg->platforms[0].pos.y=633; 
	bg->platforms[1].pos.x=771; bg->platforms[1].pos.y=461; 
	bg->platforms[2].pos.x=1031; bg->platforms[2].pos.y=461;
	bg->platforms[3].pos.x=1250; bg->platforms[3].pos.y=633;
	
	bg->platforms[3].pos_init.x=1215; bg->platforms[3].pos_init.y=296;
	
	bg->platforms[0].type=0; 
	bg->platforms[1].type=2; 
	bg->platforms[2].type=0; 
	bg->platforms[3].type=1; 
	bg->platforms[3].move_forward=1; bg->platforms[3].move_back=0; 
		
	return 0; 
}

void display_bg(SDL_Renderer *renderer, Background_temp *bg){
	
	SDL_Rect pos_bg={0,0, SCREEN_WIDTH, SCREEN_HEIGHT}; 
	
	SDL_RenderCopy(renderer, bg->image_mask, NULL, NULL);
	SDL_RenderCopy(renderer, bg->image, NULL, NULL);
	
	for (int i=0; i<num_platforms; i++) {
	if (i==3) {
		bg->platforms[i].prev_x=bg->platforms[i].pos.x; 
		bg->platforms[i].prev_y=bg->platforms[i].pos.y; 
		if (bg->platforms[i].pos.x+bg->platforms[i].pos.w >=SCREEN_WIDTH && bg->platforms[i].move_forward) {
			bg->platforms[i].move_back=1; 
			bg->platforms[i].move_forward=0; }
		if (bg->platforms[i].pos.x <=bg->platforms[i].pos_init.x) {
			bg->platforms[i].move_back=0; 
			bg->platforms[i].move_forward=1; }
		if (bg->platforms[i].move_back==1) bg->platforms[i].pos.x--; 
		if (bg->platforms[i].move_forward==1) bg->platforms[i].pos.x++; }
		
	if (bg->platforms[i].status==1) SDL_RenderCopy(renderer, bg->platforms[i].image, NULL, &bg->platforms[i].pos);
	}
		 
}
void free_bg(Background_temp *bg){
	SDL_DestroyTexture(bg->image); 
	SDL_DestroyTexture(bg->image_mask); 
	SDL_FreeSurface(bg->mask); 
	for (int i=0; i<num_platforms; i++) SDL_DestroyTexture(bg->platforms[i].image); 
} 

int switch_levels(Entity coins[], int num_coins){
	for (int i=0; i<num_coins; i++) {
		if (coins[i].status==1) return 0; 	
	}
	return 1; 
}




