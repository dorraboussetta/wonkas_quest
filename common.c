#include "common.h"

void init_text(Text *t, char *font_path, int size, char *content, SDL_Color color, int x, int y, SDL_Renderer *renderer)
{
    t->font = TTF_OpenFont(font_path, size);
    if (!t->font)
        printf("Font load error: %s\n", TTF_GetError());
    t->color = color;
    t->size = size;
    strcpy(t->text, content);
    t->txt_image = TTF_RenderText_Blended(t->font, t->text, t->color);
    t->txt_text  = SDL_CreateTextureFromSurface(renderer, t->txt_image);
    t->pos.x = x;
    t->pos.y = y;
    t->pos.w = t->txt_image->w;
    t->pos.h = t->txt_image->h;
}

void display_text(Text t, SDL_Renderer *renderer)
{
    SDL_RenderCopy(renderer, t.txt_text, NULL, &t.pos);
}

void free_text(Text *t)
{
    SDL_FreeSurface(t->txt_image);
    SDL_DestroyTexture(t->txt_text);
    TTF_CloseFont(t->font);
}

void init_button(Button *b, char *img1, char *img2, char *img3, int x, int y, SDL_Renderer *renderer)
{
    b->img[0] = IMG_LoadTexture(renderer, img1);
    b->img[1] = IMG_LoadTexture(renderer, img2);
    b->img[2] = IMG_LoadTexture(renderer, img3);
    if (!b->img[0] || !b->img[1] || !b->img[2])
        printf("Button image load error: %s\n", IMG_GetError());
    b->current_img = 0;
    SDL_QueryTexture(b->img[0], NULL, NULL, &b->pos.w, &b->pos.h);
    b->pos.x = x;
    b->pos.y = y;
    b->click_sound = Mix_LoadWAV("./assets_load/click.wav");
    b->hover_sound = Mix_LoadWAV("./assets_load/hover.wav");
    b->action = 0;
    /* ADDED: initialise keyboard_selected to 0 (not selected) */
    b->keyboard_selected = 0;
}

void display_button(Button b, SDL_Renderer *renderer)
{
    SDL_RenderCopy(renderer, b.img[b.current_img], NULL, &b.pos);
}

void update_button(Button *b)
{
    int x, y;
    Uint32 mouse = SDL_GetMouseState(&x, &y);
    SDL_Rect rect = b->pos;
    int inside = (x > rect.x &&
                  x < rect.x + rect.w &&
                  y > rect.y &&
                  y < rect.y + rect.h);

    /* Reset action every frame */
    b->action = 0;

    /* ADDED: If this button is keyboard-selected, keep it in hover state
       and let the keyboard handler deal with clicks — skip mouse logic */
    if (b->keyboard_selected)
    {
        b->current_img = 1; /* stay on hover image */
        return;
    }

    if (inside)
    {
        if (mouse & SDL_BUTTON(SDL_BUTTON_LEFT))
        {
            if (b->current_img != 2)
            {
                Mix_HaltChannel(-1);
                Mix_PlayChannel(-1, b->click_sound, 0);
                b->action = 1;
            }
            b->current_img = 2; /* clicked */
        }
        else
        {
            if (b->current_img == 0 && b->hover_sound != NULL)
            {
                Mix_PlayChannel(-1, b->hover_sound, 0);
            }
            b->current_img = 1; /* hover */
        }
    }
    else
    {
        b->current_img = 0; /* idle */
    }
}

void free_button(Button *b)
{
    SDL_DestroyTexture(b->img[0]);
    SDL_DestroyTexture(b->img[1]);
    SDL_DestroyTexture(b->img[2]);
    Mix_FreeChunk(b->click_sound);
    Mix_FreeChunk(b->hover_sound);
}

void init_background(Background *bg, char *img_path, char *music_path, SDL_Renderer *renderer)
{
    bg->img[0] = IMG_LoadTexture(renderer, img_path);
    if (!bg->img[0])
        printf("Background image error: %s\n", IMG_GetError());
    bg->current_img = 0;
    bg->bg_sound = Mix_LoadMUS(music_path);
    if (!bg->bg_sound)
        printf("Music load error: %s\n", Mix_GetError());
    else
        Mix_PlayMusic(bg->bg_sound, -1);
    bg->logo = NULL;
}

void display_background(Background bg, SDL_Renderer *renderer)
{
    SDL_RenderCopy(renderer, bg.img[bg.current_img], NULL, NULL);
}

void free_background(Background *bg)
{
    SDL_DestroyTexture(bg->img[0]);
    Mix_FreeMusic(bg->bg_sound);
}
