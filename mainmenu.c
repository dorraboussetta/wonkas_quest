#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h> 
#include <stdio.h>
#include <string.h>
#include "mainmenu.h"
#include "common.h"

int SDL_Systems_init(){
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) {
        printf("SDL_Init Error: %s\n", SDL_GetError());
        return 1;
    }
	if (TTF_Init()==-1) {
	printf("TTF_Init error: %s\n", TTF_GetError()); 
	return 1; 
	}

	if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
	printf("IMG_Init error: %s\n", SDL_GetError()); 
	}

	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048)<0){
	printf("Mix_OpenAudio error: %s\n", Mix_GetError()); 
	return 1; 
	}
	return 0; 
}

SDL_Window* create_window(char title[], int width, int height){

	 SDL_Window *window = SDL_CreateWindow(
        title,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        width, height,
        SDL_WINDOW_FULLSCREEN
    );
    if (!window) {
        printf("Erreur SDL_CreateWindow : %s\n", SDL_GetError());
        SDL_Quit();
    }
    return window;
    
}

SDL_Renderer* create_renderer(SDL_Window *window){
	SDL_Renderer *renderer= SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED); 
    
	if (renderer==NULL) {
	    	printf("Renderer error: %s\n", SDL_GetError());  
	    	}
	return renderer; 
}
	


int init_bg_main_menu(SDL_Renderer *renderer, Background *bg) {
	int load_fail=0; 
	  
	 char file_name[100]; 
	 
	 for (int i=0; i<29; i++) {
	 	sprintf(file_name, "./Assets_main/Background_images/bg_%02d.png", i+1); 
	 	bg->img[i]=IMG_LoadTexture(renderer, file_name); 
	 	if (bg->img[i]==NULL) load_fail=1; 
	 }
	 if (load_fail==1) {
	 	printf("Background images loading failed.\n"); 
	 	printf("IMG Error: %s\n", IMG_GetError());
	 	return 1; }
	 bg->txt.font=TTF_OpenFont("./Assets_main/Pixel_Game.otf", 24); 
	 if (!bg->txt.font) {
	 	printf("Font loading failed.\n"); 
	 	return 1; }
	 bg->txt.color.r=255; bg->txt.color.g=215; bg->txt.color.b=0; bg->txt.color.a=255; 
	 bg->txt.size=64.23; 
	 bg->txt.pos.x=1500.82; 
	 bg->txt.pos.y=156.63; 
	 bg->txt.pos.w=354.37; 
	 bg->txt.pos.h=68.28; 
	 strcpy(bg->txt.text,"Wonka's Quest"); 
	 bg->txt.txt_image=TTF_RenderText_Solid(bg->txt.font, bg->txt.text, bg->txt.color); 
	 bg->txt.txt_text=SDL_CreateTextureFromSurface(renderer, bg->txt.txt_image); 
	 
	 bg->current_img=0; 
	 
	 bg->bg_sound=Mix_LoadMUS("./Assets_main/bg_sound.mp3"); 
	 if (! bg->bg_sound) return 1; 
	 
	 bg->logo=IMG_LoadTexture(renderer, "./Assets_main/logo.png"); 
	 if (!bg->logo) return 1; 
	 
	 
	 return 0; 
}

int init_buttons_main_menu(SDL_Renderer *renderer, Button buttons[]){

	const char* button_names[]={"play", "options", "highscore", "history", "exit"}; 
	char file_name[100]; 
	
	for (int i=0; i<5; i++) {
	
		buttons[i].action=i; 
		sprintf(file_name, "./Assets_main/Buttons/Normal/%s_bt_normal.png", button_names[i]); 
		buttons[i].img[0]=IMG_LoadTexture(renderer, file_name); 
		
		sprintf(file_name, "./Assets_main/Buttons/Hover/%s_bt_hovered.png", button_names[i]); 
		buttons[i].img[1]=IMG_LoadTexture(renderer, file_name); 
		
		if (!buttons[i].img[1]) {
			printf("Failed loading: %s\n", file_name);
    			printf("IMG Error: %s\n", IMG_GetError());
    			return 1; 
		}
		
		sprintf(file_name, "./Assets_main/Buttons/Pressed/%s_bt_pressed.png", button_names[i]); 
		buttons[i].img[2]=IMG_LoadTexture(renderer, file_name); 
		
		buttons[i].click_sound=Mix_LoadWAV("./Assets_main/click_sound.wav"); 
		buttons[i].hover_sound=Mix_LoadWAV("./Assets_main/hover_sound.wav"); 
		
		if (!buttons[i].img[0] || !buttons[i].img[1] || !buttons[i].img[2] || !buttons[i].click_sound) {
	 	printf("Buttons loading failed.\n"); 
	 	return 1; } 
	 	}
		
	buttons[0].pos.x=169; buttons[0].pos.y=88; buttons[0].pos.w=523; buttons[0].pos.h=138; 
	buttons[1].pos.x=169; buttons[1].pos.y=252; buttons[1].pos.w=523; buttons[1].pos.h=136; 
	buttons[2].pos.x=169; buttons[2].pos.y=415; buttons[2].pos.w=523; buttons[2].pos.h=137; 
	buttons[3].pos.x=169; buttons[3].pos.y=577; buttons[3].pos.w=523; buttons[3].pos.h=137; 
	buttons[4].pos.x=1333; buttons[4].pos.y=874; buttons[4].pos.w=523; buttons[4].pos.h=137; 
	
	
	
	
	return 0; 
}

int init_main_menu(SDL_Renderer *renderer, MainMenu *main_menu){
	
	if (init_bg_main_menu(renderer, &main_menu->bg)==1) return 1; 
	if (init_buttons_main_menu(renderer, main_menu->buttons)==1) return 1; 
	return 0;    

}

 
SDL_Texture* init_sub_menu(SDL_Renderer *renderer){
	SDL_Texture *sub_img=IMG_LoadTexture(renderer, "./Assets_main/sub_img.png"); 
	return sub_img; 
} 

SDL_Texture* init_highscore_menu(SDL_Renderer *renderer) {
	SDL_Texture *highscore_img=IMG_LoadTexture(renderer, "./Assets_main/highscore_img.png"); 
	return highscore_img; 
}
SDL_Texture* init_history_menu(SDL_Renderer *renderer) {
	SDL_Texture *history_img=IMG_LoadTexture(renderer, "./Assets_main/history_img.png"); 
	return history_img; 
} 



void display_buttons_main_menu(SDL_Renderer *renderer, MainMenu main_menu, int num_button_modified) {
	int newBt_State; 
	
	for (int i=0; i<5; i++) {
	if (num_button_modified==i) {
	newBt_State=main_menu.buttons[i].current_img; 
	SDL_RenderCopy(renderer, main_menu.buttons[i].img[newBt_State], NULL, &main_menu.buttons[i].pos); }
	else SDL_RenderCopy(renderer, main_menu.buttons[i].img[0], NULL, &main_menu.buttons[i].pos);
	
	}

}
void display_main_menu(SDL_Renderer *renderer, MainMenu main_menu, int num_button_modified){
	SDL_Rect pos_logo={1620,40,119,99}; 

	SDL_RenderClear(renderer);
	SDL_RenderCopy(renderer, main_menu.bg.img[main_menu.bg.current_img], NULL, NULL); 
	SDL_RenderCopy(renderer, main_menu.bg.logo, NULL, &pos_logo); 
	SDL_RenderCopy(renderer, main_menu.bg.txt.txt_text, NULL, &main_menu.bg.txt.pos);
	display_buttons_main_menu(renderer, main_menu, num_button_modified); 
	SDL_RenderPresent(renderer); 

}

void display_sub_menu(SDL_Renderer *renderer, SDL_Texture *sub_img){
	SDL_Rect pos={0,0,1200,675};
	SDL_RenderClear(renderer);
	SDL_RenderCopy(renderer, sub_img , NULL, &pos);  
	SDL_RenderPresent(renderer); 

} 

void display_highscore_menu(SDL_Renderer *renderer, SDL_Texture *highscore_img){
	SDL_Rect pos={0,0,1200,675};
	SDL_RenderClear(renderer);
	SDL_RenderCopy(renderer, highscore_img , NULL, &pos);  
	SDL_RenderPresent(renderer); 
} 

void display_history_menu(SDL_Renderer *renderer, SDL_Texture *history_img){
	SDL_Rect pos={0,0,1200,675};
	SDL_RenderClear(renderer);
	SDL_RenderCopy(renderer, history_img , NULL, &pos);  
	SDL_RenderPresent(renderer); 
} 

void event_main_menu(MainMenu main_menu, int *click_time, int *interface, int *click, int *motion, int *mx, int *my, int *num_button_modified, SDL_Event event, int *last_hover) {
	int current_hover=-1, i; 
	

	if (*interface!=0) return; 
	if (event.type == SDL_KEYDOWN) {
	   if (event.key.keysym.sym == SDLK_ESCAPE) *interface=9; 
	   else {
	   	if (*interface!=0) return; 
	   	if (event.key.keysym.sym == SDLK_j) *interface=3;
	   	else {
	   		if (event.key.keysym.sym == SDLK_o) *interface=1;
	   		else {
	   			if (event.key.keysym.sym == SDLK_m) *interface=6;
	   			
	   		}
	   	}
	   
	   }
	    }
	    
	 
	if (event.type == SDL_MOUSEBUTTONDOWN) {
	    	SDL_GetMouseState(mx, my); 
	    	*click=0; 
	    	
	    	for (i=0; i<5; i++) {
		    	if (*mx>=main_menu.buttons[i].pos.x && *mx<=main_menu.buttons[i].pos.x + main_menu.buttons[i].pos.w && *my>=main_menu.buttons[i].pos.y && *my<=main_menu.buttons[i].pos.y + main_menu.buttons[i].pos.h) {
		    	*click=1; 
		    	*num_button_modified=i; 
		    	*click_time=SDL_GetTicks(); 
		    	Mix_PlayChannel(-1, main_menu.buttons[i].click_sound, 0);
		    	break; 
		    	} 
		    	
	    	
	    	}
	    	if (*click){
	    		switch (*num_button_modified)  {
	    			case 0: 
	    			*interface=3; //load menu
	    			break; 
	    			
	    			case 1: 
	    			*interface=1; //sub menu
	    			break; 
	    			
	    			case 2: 
	    			*interface=6; //highscore menu
	    			break; 
	    			
	    			
	    			case 4: 
	    			*interface=9; //exit
	    			break; 
					    		
	    		}
	    		
	    	 }
	    	
	    	}
	
	
	
	
	   
	if (event.type == SDL_MOUSEMOTION) {
	    	SDL_GetMouseState(mx, my); 
	    	*motion=0; 
	    	
	    	for (i=0; i<5; i++) {
		    	if (*mx>=main_menu.buttons[i].pos.x && *mx<=main_menu.buttons[i].pos.x + main_menu.buttons[i].pos.w && *my>=main_menu.buttons[i].pos.y && *my<=main_menu.buttons[i].pos.y + main_menu.buttons[i].pos.h) {
		    	*motion=1; 
		    	*num_button_modified=i; 
		    	current_hover=i; 
		    	
		    	} 
	    	
	    	}
	    	if ( current_hover!=-1 && *last_hover!=current_hover) {
	    		Mix_PlayChannel(-1, main_menu.buttons[current_hover].hover_sound, 0); 
	    		*last_hover=current_hover; 
	    	}
	    
	    	}
	    	
	    	if (*motion==0 && *click==0) *num_button_modified=-1;    
}

void update_bg_main_menu(MainMenu *main_menu) {
	
	
	if (main_menu->bg.current_img<28) main_menu->bg.current_img++; 
	else main_menu->bg.current_img=0; 
		
	
}

void update_buttons_main_menu(MainMenu *main_menu, int click_time, int *click, int motion, int num_button_modified) {
	if (num_button_modified == -1) return;
	int current_Time=SDL_GetTicks(); 
	if (*click==1 && current_Time-click_time<=200) main_menu->buttons[num_button_modified].current_img=2; 
	else 
	{
		*click=0; 
		if (motion && !*click ) main_menu->buttons[num_button_modified].current_img=1; 
		else {if (!motion && !*click) main_menu->buttons[num_button_modified].current_img=0;}
	}

}

void update_main_menu(MainMenu *main_menu,int click_time, int *running, int *click, int motion, int interface, int num_button_modified) {
	update_bg_main_menu(main_menu);
	//if (interface==9) *running=0; 
	 
	if (num_button_modified==-1) return; 
	update_buttons_main_menu(main_menu, click_time,click, motion, num_button_modified); 
	
}

void free_main_menu(MainMenu main_menu){
	for (int i=0; i<5; i++) {
		for (int j=0; j<3; j++) SDL_DestroyTexture(main_menu.buttons[i].img[j]);
		Mix_FreeChunk(main_menu.buttons[i].click_sound);
		Mix_FreeChunk(main_menu.buttons[i].hover_sound);
		}
	for (int i=0; i<29; i++) SDL_DestroyTexture(main_menu.bg.img[i]); 
	TTF_CloseFont(main_menu.bg.txt.font); 
	SDL_DestroyTexture(main_menu.bg.txt.txt_text); 
	Mix_FreeMusic(main_menu.bg.bg_sound); 
	SDL_FreeSurface(main_menu.bg.txt.txt_image); 
	SDL_DestroyTexture(main_menu.bg.logo); 



}

