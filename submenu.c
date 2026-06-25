#include "submenu.h"
#include "common.h"

// ── Asset loading helpers 

static void load_background(image *img, char *path, SDL_Renderer *renderer){
    img->texture = IMG_LoadTexture(renderer, path);
    if(img->texture == NULL) printf("background error: %s\n", IMG_GetError());
    img->rect.x = 0; img->rect.y = 0;
    img->rect.w = SCREEN_WIDTH; img->rect.h = SCREEN_HEIGHT;
}

static void load_img_text(image *img, char *path, SDL_Renderer *renderer, int x, int y, int w, int h){
    img->texture = IMG_LoadTexture(renderer, path);
    if(img->texture == NULL) printf("img error: %s\n", IMG_GetError());
    img->rect.x = 0; img->rect.y = 0;
    img->rect.w = SCREEN_WIDTH; img->rect.h = SCREEN_HEIGHT;
    img->dest.x = x; img->dest.y = y; img->dest.w = w; img->dest.h = h;
}

static void load_buttons(button *btn, char *path, SDL_Renderer *renderer, int x, int y, int w, int h){
    btn->texture = IMG_LoadTexture(renderer, path);
    btn->rect.x = 0; btn->rect.y = 0;
    SDL_QueryTexture(btn->texture, NULL, NULL, &btn->rect.w, &btn->rect.h);
    btn->static_dest.x = x; btn->static_dest.y = y; btn->static_dest.w = w; btn->static_dest.h = h;
    btn->final_dest = btn->static_dest;
    btn->hover.w = btn->final_dest.w * 1.3;
    btn->hover.h = btn->final_dest.h * 1.3;
    btn->hover.x = (btn->final_dest.x + btn->final_dest.w / 2) - (btn->hover.w / 2);
    btn->hover.y = (btn->final_dest.y + btn->final_dest.h / 2) - (btn->hover.h / 2);
}

static const int orig[5][4] = {
    {560, 150,  75,  75},
    {840, 150,  75,  75},
    {502, 325, 195,  75},
    {788, 325, 195,  75},
    {850, 500, 195,  75},
};

static void load_assets(image *bg, image *text, button *btn, Mix_Music **music, Mix_Chunk **effect, SDL_Renderer *renderer){
    load_background(bg, "options_assets/background.png", renderer);
    load_buttons(&btn[0], "options_assets/-.png",          renderer, orig[0][0], orig[0][1], orig[0][2], orig[0][3]);
    load_buttons(&btn[1], "options_assets/+.png",          renderer, orig[1][0], orig[1][1], orig[1][2], orig[1][3]);
    load_buttons(&btn[2], "options_assets/normal.png",     renderer, orig[2][0], orig[2][1], orig[2][2], orig[2][3]);
    load_buttons(&btn[3], "options_assets/full.png",       renderer, orig[3][0], orig[3][1], orig[3][2], orig[3][3]);
    load_buttons(&btn[4], "options_assets/return.png",     renderer, orig[4][0], orig[4][1], orig[4][2], orig[4][3]);
    load_img_text(&text[0], "options_assets/volume.png",         renderer, 185, 150, 210,  80);
    load_img_text(&text[1], "options_assets/display mode.png",   renderer, 185, 325, 210, 150);
    *music  = Mix_LoadMUS("options_assets/Wonka music.mp3");
    *effect = Mix_LoadWAV("options_assets/button_click.mp3");
}

// ── Init 

int init_option_menu(SDL_Renderer *renderer, image *bg, image *text, button *btn, Mix_Music **music, Mix_Chunk **effect){
    load_assets(bg, text, btn, music, effect, renderer);
    btn[0].final_dest = btn[0].hover;   // >>>ADDED: pre-select first button on menu entry
    return 0;
}

// ── Hover / click

static int hover_effect(int x, int y, button *btn){
    int select = 0;
    for(int i = 0; i < 5; i++){
        if(btn[i].static_dest.x <= x && x <= btn[i].static_dest.x + btn[i].static_dest.w &&
           btn[i].static_dest.y <= y && y <= btn[i].static_dest.y + btn[i].static_dest.h){
            select = i + 1;
            btn[i].final_dest = btn[i].hover;
        } else {
            btn[i].final_dest = btn[i].static_dest;
        }
    }
    return select;
}

static int click_effect(int x, int y, button *btn){
    int j;
    for(j = 0; j < 5; j++){
        if(btn[j].static_dest.x <= x && x <= btn[j].static_dest.x + btn[j].static_dest.w &&
           btn[j].static_dest.y <= y && y <= btn[j].static_dest.y + btn[j].static_dest.h)
            break;
    }
    if(j < 5){ btn[j].final_dest = btn[j].static_dest; return j + 1; }
    return 0;
}

static void volume_down(int *volume){
    *volume -= 8;
    if(*volume < 0) *volume = 0;
    Mix_VolumeMusic(*volume);
    Mix_Volume(-1, *volume);
}

static void volume_up(int *volume){
    *volume += 8;
    if(*volume > MIX_MAX_VOLUME) *volume = MIX_MAX_VOLUME;
    Mix_VolumeMusic(*volume);
    Mix_Volume(-1, *volume);
}

static void change_fullsize(button *btn, int newW, int newH){
    float scaleX = (float)newW / SCREEN_WIDTH;
    float scaleY = (float)newH / SCREEN_HEIGHT;
    btn->static_dest.x = (int)(btn->static_dest.x * scaleX);
    btn->static_dest.y = (int)(btn->static_dest.y * scaleY);
    btn->static_dest.w = (int)(btn->static_dest.w * scaleX);
    btn->static_dest.h = (int)(btn->static_dest.h * scaleY);
    btn->final_dest = btn->static_dest;
    btn->hover.w = (int)(btn->final_dest.w * 1.3);
    btn->hover.h = (int)(btn->final_dest.h * 1.3);
    btn->hover.x = (btn->final_dest.x + btn->final_dest.w / 2) - btn->hover.w / 2;
    btn->hover.y = (btn->final_dest.y + btn->final_dest.h / 2) - btn->hover.h / 2;
}

static void change_normalsize(button *btn, int origX, int origY, int origW, int origH){
    btn->static_dest.x = origX; btn->static_dest.y = origY;
    btn->static_dest.w = origW; btn->static_dest.h = origH;
    btn->final_dest = btn->static_dest;
    btn->hover.w = (int)(btn->final_dest.w * 1.3);
    btn->hover.h = (int)(btn->final_dest.h * 1.3);
    btn->hover.x = (btn->final_dest.x + btn->final_dest.w / 2) - btn->hover.w / 2;
    btn->hover.y = (btn->final_dest.y + btn->final_dest.h / 2) - btn->hover.h / 2;
}

// ── Helper: execute action for a given button index (1-based)  >>>ADDED
static void execute_action(int action, button *btn, Mix_Chunk *effect, SDL_Window *window, int *volume, int *interface, int *prev_interface){   // >>>ADDED
    Mix_PlayChannel(-1, effect, 0);                                                                                         // >>>ADDED
    int newW, newH;                                                                                                         // >>>ADDED
    if(action == 1) volume_down(volume);                                                                                    // >>>ADDED
    if(action == 2) volume_up(volume);                                                                                      // >>>ADDED
    if(action == 3){                                                                                                        // >>>ADDED
        SDL_SetWindowFullscreen(window, 0);                                                                                 // >>>ADDED
        for(int i = 0; i < 5; i++)                                                                                         // >>>ADDED
            change_normalsize(&btn[i], orig[i][0], orig[i][1], orig[i][2], orig[i][3]);                                    // >>>ADDED
    }                                                                                                                       // >>>ADDED
    if(action == 4){                                                                                                        // >>>ADDED
        SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);                                                     // >>>ADDED
        SDL_GetWindowSize(window, &newW, &newH);                                                                            // >>>ADDED
        for(int i = 0; i < 5; i++)                                                                                         // >>>ADDED
            change_fullsize(&btn[i], newW, newH);                                                                           // >>>ADDED
    }                                                                                                                       // >>>ADDED
    if(action == 5)                                                                                                         // >>>ADDED
        *interface = (*prev_interface == 7 || *prev_interface == 8) ? *prev_interface : 0;                                  // >>>ADDED
}                                                                                                                           // >>>ADDED

// ── Event 

void event_options(SDL_Event event, button *btn, Mix_Music *music, Mix_Chunk *effect, SDL_Window *window, int *volume, int *interface, int *prev_interface){
    static int clicked_btn = -1;
    static int focused_btn = 0;   // >>>ADDED: tracks keyboard-focused button
    int click = 0;

    if(event.type == SDL_MOUSEMOTION){
        int hovered = hover_effect(event.motion.x, event.motion.y, btn);   // >>>ADDED: capture return value
        if(hovered > 0) focused_btn = hovered - 1;                         // >>>ADDED: sync focus to mouse
        btn[focused_btn].final_dest = btn[focused_btn].hover; // >>>ADDED: restore focus if mouse leaves
    }

    if(event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT){
        click = click_effect(event.button.x, event.button.y, btn);
        clicked_btn = click - 1;
        if(click > 0){
            focused_btn = click - 1;           // >>>ADDED: sync focus to click
            Mix_PlayChannel(-1, effect, 0);
        }
        if(click == 1) volume_down(volume);
        if(click == 2) volume_up(volume);
        if(click == 3){
            SDL_SetWindowFullscreen(window, 0);
            for(int i = 0; i < 5; i++)
                change_normalsize(&btn[i], orig[i][0], orig[i][1], orig[i][2], orig[i][3]);
        }
        if(click == 4){
            int newW, newH;
            SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
            SDL_GetWindowSize(window, &newW, &newH);
            for(int i = 0; i < 5; i++)
                change_fullsize(&btn[i], newW, newH);
        }
	//Added
        if(click == 5)
   	 *interface = (*prev_interface == 7 || *prev_interface == 8) ? *prev_interface : 0;
    }

    if(event.type == SDL_MOUSEBUTTONUP && clicked_btn >= 0){
        btn[clicked_btn].final_dest = btn[clicked_btn].hover;
        clicked_btn = -1;
    }

    if(event.type == SDL_KEYDOWN){
        if(event.key.keysym.sym == SDLK_MINUS)                        volume_down(volume);
        if(event.key.keysym.sym == SDLK_PLUS ||
           event.key.keysym.sym == SDLK_EQUALS)                       volume_up(volume);

        // >>>ADDED: arrow key navigation between buttons
        if(event.key.keysym.sym == SDLK_LEFT ||
           event.key.keysym.sym == SDLK_RIGHT ||
           event.key.keysym.sym == SDLK_UP){
            for(int i = 0; i < 5; i++) btn[i].final_dest = btn[i].static_dest;   // clear all
            if(event.key.keysym.sym == SDLK_LEFT)
                focused_btn = (focused_btn - 1 + 5) % 5;   // wrap left
            else
                focused_btn = (focused_btn + 1) % 5;        // right/up → next
            btn[focused_btn].final_dest = btn[focused_btn].hover;
        }
        // >>>ADDED END

        // >>>ADDED: X key confirms the focused button
        if(event.key.keysym.sym == SDLK_SPACE)
            execute_action(focused_btn + 1, btn, effect, window, volume, interface, prev_interface);
        // >>>ADDED END

        if(event.key.keysym.sym == SDLK_b)
            if(*prev_interface == 7 || *prev_interface == 8)
                *interface = *prev_interface;
    }
}

// ── Update opt menu

void update_options_menu(SDL_Renderer *renderer, image bg, image *text, button *btn){
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, bg.texture, NULL, NULL);
    for(int i = 0; i < 2; i++)
        SDL_RenderCopy(renderer, text[i].texture, NULL, &text[i].dest);
    for(int i = 0; i < 5; i++)
        SDL_RenderCopy(renderer, btn[i].texture, &btn[i].rect, &btn[i].final_dest);
}

// ── Display

void display_options_menu(SDL_Renderer *renderer){
    SDL_RenderPresent(renderer);
}

// ── Free 

void free_options_menu(image bg, button *btn, image *text, Mix_Music *music, Mix_Chunk *effect){
    for(int i = 0; i < 5; i++) SDL_DestroyTexture(btn[i].texture);
    for(int i = 0; i < 2; i++) SDL_DestroyTexture(text[i].texture);
    SDL_DestroyTexture(bg.texture);
    Mix_FreeMusic(music);
    Mix_FreeChunk(effect);
}
