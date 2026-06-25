#include "background.h"
#include <stdio.h>

/* ------------------------------------------------------------------ */
/*  Texture helper                                                      */
/* ------------------------------------------------------------------ */

SDL_Texture *load_texture(SDL_Renderer *r, const char *path)
{
    SDL_Surface *s = IMG_Load(path);
    if (!s) {
        fprintf(stderr, "IMG_Load error: %s\n", IMG_GetError());
        return NULL;
    }
    SDL_Texture *t = SDL_CreateTextureFromSurface(r, s);
    SDL_FreeSurface(s);
    return t;
}

/* ------------------------------------------------------------------ */
/*  Platform helper                                                     */
/* ------------------------------------------------------------------ */

void make_platform(Platform *p, SDL_Renderer *r, const char *img,int x, int y, int w, int h,int type, float vx, float vy)
{
    p->texture = load_texture(r, img);
    p->pos     = (SDL_Rect){x, y, w, h};
    p->prev_x  = x;
    p->prev_y  = y;
    p->type    = type;
    p->status  = 1;
    p->vel_x   = vx;
    p->vel_y   = vy;
    p->dir     = 1;
}

/* ------------------------------------------------------------------ */
/*  Background                                                          */
/* ------------------------------------------------------------------ */

int load_background(Background_Level *bg, SDL_Renderer *r,
                    const char *path, int level)
{
    bg->texture = load_texture(r, path);
    if (!bg->texture) {
        fprintf(stderr, "Failed to load background for level %d\n", level);
        return -1;
    }
    bg->current_level = level;

    /* get actual image dimensions */
    int img_w, img_h;
    SDL_QueryTexture(bg->texture, NULL, NULL, &img_w, &img_h);

    /* store real map size in bg so clamping uses actual image bounds */
    bg->map_w = img_w;
    bg->map_h = img_h;

    /* player 1 — left half */
    bg->camera1     = (SDL_Rect){0, 0, HALF_W, SCREEN_H};
    bg->pos_screen1 = (SDL_Rect){0, 0, HALF_W, SCREEN_H};

    /* player 2 — right half */
    bg->camera2     = (SDL_Rect){0, 0, HALF_W, SCREEN_H};
    bg->pos_screen2 = (SDL_Rect){HALF_W, 0, HALF_W, SCREEN_H};

    return 0;
}

void update_camera(Background_Level *bg,
                   int p1x, int p1y, int p1w, int p1h,
                   int p2x, int p2y, int p2w, int p2h)
{
    /* center camera1 on player 1 */
    bg->camera1.x = (p1x + p1w / 2) - HALF_W / 2;
    bg->camera1.y = (p1y + p1h / 2) - SCREEN_H / 2;

    /* center camera2 on player 2 */
    bg->camera2.x = (p2x + p2w / 2) - HALF_W / 2;
    bg->camera2.y = (p2y + p2h / 2) - SCREEN_H / 2;

    /* clamp camera1 against real image size */
    if (bg->camera1.x < 0) bg->camera1.x = 0;
    if (bg->camera1.y < 0) bg->camera1.y = 0;
    if (bg->camera1.x + HALF_W  > bg->map_w) bg->camera1.x = bg->map_w - HALF_W;
    if (bg->camera1.y + SCREEN_H > bg->map_h) bg->camera1.y = bg->map_h - SCREEN_H;

    /* clamp camera2 against real image size */
    if (bg->camera2.x < 0) bg->camera2.x = 0;
    if (bg->camera2.y < 0) bg->camera2.y = 0;
    if (bg->camera2.x + HALF_W  > bg->map_w) bg->camera2.x = bg->map_w - HALF_W;
    if (bg->camera2.y + SCREEN_H > bg->map_h) bg->camera2.y = bg->map_h - SCREEN_H;
}

void render_background(Background_Level *bg, SDL_Renderer *r)
{
    if (!bg->texture) return;

    /* draw left half — player 1's view */
    SDL_RenderSetClipRect(r, &bg->pos_screen1);
    SDL_RenderCopy(r, bg->texture, &bg->camera1, &bg->pos_screen1);

    /* draw right half — player 2's view */
    SDL_RenderSetClipRect(r, &bg->pos_screen2);
    SDL_RenderCopy(r, bg->texture, &bg->camera2, &bg->pos_screen2);

    /* remove clip */
    SDL_RenderSetClipRect(r, NULL);

    /* draw divider line in the middle */
    SDL_SetRenderDrawColor(r, 255, 255, 255, 255);
    SDL_RenderDrawLine(r, HALF_W, 0, HALF_W, SCREEN_H);
}

void free_background_level(Background_Level *bg)
{
    if (bg->texture) {
        SDL_DestroyTexture(bg->texture);
        bg->texture = NULL;
    }
}

/* ------------------------------------------------------------------ */
/*  Level definitions                                                   */
/* ------------------------------------------------------------------ */

int init_level(Platform *platforms, int *count,
               SDL_Renderer *r, int level)
{
    *count = 0;
    Platform *p = platforms;

    if (level == 1) {
        make_platform(p++, r, "Assets_background/bg_ground.jpeg",
                      840, 650, 200, 24, PLATFORM_FIXED, 0, 0);
        make_platform(p++, r, "Assets_background/bg_ground.jpeg",
                      430, 500, 240, 24, PLATFORM_FIXED, 0, 0);
        make_platform(p++, r, "Assets_background/pl_wood.jpg",
                      200, 400, 180, 24, PLATFORM_MOVING, 130.0f, 0);
        make_platform(p++, r, "Assets_background/pl_cracked.jpg",
                      600, 300, 140, 24, PLATFORM_DESTRUCTIBLE, 0, 0);

    } else if (level == 2) {
        make_platform(p++, r, "Assets_background/bg_ground.jpeg",
                      0, 868, 1100, 32, PLATFORM_FIXED, 0, 0);
        make_platform(p++, r, "Assets_background/bg_ground.jpeg",
                      80, 600, 180, 24, PLATFORM_FIXED, 0, 0);
        make_platform(p++, r, "Assets_background/bg_ground.jpeg",
                      840, 500, 180, 24, PLATFORM_FIXED, 0, 0);
        make_platform(p++, r, "Assets_background/bg_ground.jpeg",
                      420, 300, 260, 24, PLATFORM_FIXED, 0, 0);
        make_platform(p++, r, "Assets_background/pl_wood.jpg",
                      100, 450, 180, 24, PLATFORM_MOVING, 100.0f, 0);
        make_platform(p++, r, "Assets_background/pl_wood.jpg",
                      600, 350, 130, 24, PLATFORM_MOVING, 200.0f, 0);
        make_platform(p++, r, "Assets_background/pl_cracked.jpg",
                      250, 700, 140, 24, PLATFORM_DESTRUCTIBLE, 0, 0);
        make_platform(p++, r, "Assets_background/pl_cracked.jpg",
                      550, 580, 140, 24, PLATFORM_DESTRUCTIBLE, 0, 0);
        make_platform(p++, r, "Assets_background/pl_cracked.jpg",
                      800, 720, 140, 24, PLATFORM_DESTRUCTIBLE, 0, 0);
    }

    *count = (int)(p - platforms);
    return *count;
}

/* ------------------------------------------------------------------ */
/*  Render platforms for both halves                                    */
/* ------------------------------------------------------------------ */

void render_platforms_split(Platform *platforms, int count,SDL_Renderer *r,SDL_Rect camera1, SDL_Rect screen1,SDL_Rect camera2, SDL_Rect screen2)
{
    for (int i = 0; i < count; i++) {
        Platform *p = &platforms[i];
        if (!p->status || !p->texture) continue;

        /* draw in player 1's half */
        SDL_Rect dst1 = {screen1.x + (p->pos.x - camera1.x),screen1.y + (p->pos.y - camera1.y),p->pos.w,p->pos.h};
        SDL_RenderSetClipRect(r, &screen1);
        SDL_RenderCopy(r, p->texture, NULL, &dst1);

        /* draw in player 2's half */
        SDL_Rect dst2 = {screen2.x + (p->pos.x - camera2.x),screen2.y + (p->pos.y - camera2.y),p->pos.w,p->pos.h};
        SDL_RenderSetClipRect(r, &screen2);
        SDL_RenderCopy(r, p->texture, NULL, &dst2);
    }
    SDL_RenderSetClipRect(r, NULL);
}

/* ------------------------------------------------------------------ */
/*  Update                                                              */
/* ------------------------------------------------------------------ */

void update_mobile(Platform *platforms, int count, float delta_t, int map_w)
{
    for (int i = 0; i < count; i++) {
        Platform *p = &platforms[i];
        if (p->type != PLATFORM_MOVING || !p->status) continue;

        p->prev_x = p->pos.x;
        p->prev_y = p->pos.y;

        p->pos.x += (int)(p->vel_x * p->dir * delta_t);

        if (p->pos.x <= 0) {
            p->pos.x = 0;
            p->dir   = 1;
        } else if (p->pos.x + p->pos.w >= map_w) {
            p->pos.x = map_w - p->pos.w;
            p->dir   = -1;
        }
    }
}

/* ------------------------------------------------------------------ */
/*  Destroy / free                                                      */
/* ------------------------------------------------------------------ */

void destroy_platform(Platform *p) { p->status = 0; }

void free_platforms(Platform *platforms, int count)
{
    for (int i = 0; i < count; i++) {
        if (platforms[i].texture) {
            SDL_DestroyTexture(platforms[i].texture);
            platforms[i].texture = NULL;
        }
    }
}

/* ------------------------------------------------------------------ */
/*  Load all assets                                                     */
/* ------------------------------------------------------------------ */

int load_all_assets(SDL_Renderer *r,Background_Level *bg1, Background_Level *bg2,Platform *lvl1, int *count1,Platform *lvl2, int *count2)
{
    if (load_background(bg1, r, "Assets_background/bg_level1.png", 1) != 0) return -1;

    /* try bg_level2, fall back to bg_level1 if missing */
    if (load_background(bg2, r, "Assets_background/bg_level2.jpg", 2) != 0) {
        fprintf(stderr, "bg_level2 missing, reusing bg_level1 for level 2\n");
        if (load_background(bg2, r, "Assets_background/bg_level1.png", 2) != 0) return -1;
    }

    init_level(lvl1, count1, r, 1);
    init_level(lvl2, count2, r, 2);
    return 0;
}

/* ------------------------------------------------------------------ */
/*  Timer                                                               */
/* ------------------------------------------------------------------ */

Timer *create_timer(const char *font_path, int font_size)
{
    if (TTF_Init() != 0) {
        fprintf(stderr, "TTF_Init: %s\n", TTF_GetError());
        return NULL;
    }

    Timer *t = malloc(sizeof(Timer));
    if (!t) return NULL;

    t->font = TTF_OpenFont(font_path, font_size);
    if (!t->font) {
        fprintf(stderr, "TTF_OpenFont: %s\n", TTF_GetError());
        free(t);
        return NULL;
    }

    t->start_ticks = (int)SDL_GetTicks();  //EXPLAIN THIS LATERRRRRRRRRRRRRRRRRRR
    t->r = 255;
    t->g = 255;
    t->b = 255;
    t->a = 255;
    return t;
}

void render_timer(Timer *t, SDL_Renderer *r, int x, int y)
{
    if (!t || !t->font) return;

    int elapsed = (int)SDL_GetTicks() - t->start_ticks;
    int minutes = (elapsed / 1000) / 60;
    int seconds = (elapsed / 1000) % 60;
    int millis  = elapsed % 1000;

    char buf[32];
    SDL_snprintf(buf, sizeof(buf), "%02d:%02d:%03d", minutes, seconds, millis);

    SDL_Color color = {(Uint8)t->r, (Uint8)t->g, (Uint8)t->b, (Uint8)t->a};

    SDL_Surface *surf = TTF_RenderText_Solid(t->font, buf, color);
    if (!surf) return;

    SDL_Texture *tex = SDL_CreateTextureFromSurface(r, surf);
    SDL_FreeSurface(surf);
    if (!tex) return;

    int w, h;
    SDL_QueryTexture(tex, NULL, NULL, &w, &h);
    SDL_Rect dst = {x, y, w, h};
    SDL_RenderCopy(r, tex, NULL, &dst);
    SDL_DestroyTexture(tex);
}

void free_timer(Timer *t)
{
    if (!t) return;
    if (t->font) {
        TTF_CloseFont(t->font);
        t->font = NULL;
    }
    TTF_Quit();
    free(t);
}

/* ------------------------------------------------------------------ */
/*  Signs                                                               */
/* ------------------------------------------------------------------ */

void init_signs(Sign *signs, int *count, SDL_Renderer *r, int level)
{
    *count = 0;
    Sign *s = signs;

    if (level == 1) {
        s->texture   = load_texture(r, "Assets_background/sign.png");
        s->pos       = (SDL_Rect){300, 620, 48, 48};
        s->hint_text = "Use arrow keys to move!";
        s->active    = 0;
        s++; (*count)++;

        s->texture   = load_texture(r, "Assets_background/sign.png");
        s->pos       = (SDL_Rect){600, 620, 48, 48};
        s->hint_text = "Watch out for cracked platforms!";
        s->active    = 0;
        s++; (*count)++;

    } else if (level == 2) {
        s->texture   = load_texture(r, "Assets_background/sign.png");
        s->pos       = (SDL_Rect){200, 820, 48, 48};
        s->hint_text = "Moving platforms carry you along!";
        s->active    = 0;
        s++; (*count)++;
    }
}

void update_signs(Sign *signs, int count,
                  int px, int py, int pw, int ph)
{
    /* trigger distance — how close the player needs to be */
    int trigger = 80;

    for (int i = 0; i < count; i++) {
        Sign *s = &signs[i];

        /* check if player overlaps the trigger zone around the sign */
        int near_x = px + pw > s->pos.x - trigger && px < s->pos.x + s->pos.w + trigger;
        int near_y = py + ph > s->pos.y - trigger &&py  < s->pos.y + s->pos.h + trigger;

        s->active = (near_x && near_y) ? 1 : 0;
    }
}

void render_signs(Sign *signs, int count, SDL_Renderer *r,
                  SDL_Rect camera, SDL_Rect screen_area,
                  TTF_Font *font)
{
    for (int i = 0; i < count; i++) {
        Sign *s = &signs[i];

        /* draw sign graphic */
        if (s->texture) {
            SDL_Rect dst = {screen_area.x + (s->pos.x - camera.x),screen_area.y + (s->pos.y - camera.y),s->pos.w,s->pos.h};
            SDL_RenderSetClipRect(r, &screen_area);
            SDL_RenderCopy(r, s->texture, NULL, &dst);
        }

        /* draw hint box if player is near */
        if (s->active && font) {
            int sign_screen_x = screen_area.x + (s->pos.x - camera.x);
            int sign_screen_y = screen_area.y + (s->pos.y - camera.y);

            /* render text to get its size */
            SDL_Color white = {255, 255, 255, 255};
            SDL_Surface *surf = TTF_RenderText_Solid(font, s->hint_text, white);
            if (!surf) continue;

            SDL_Texture *tex = SDL_CreateTextureFromSurface(r, surf);
            SDL_FreeSurface(surf);
            if (!tex) continue;

            int tw, th;
            SDL_QueryTexture(tex, NULL, NULL, &tw, &th);

            /* hint box dimensions with padding */
            int pad    = 10;
            int box_w  = tw + pad * 2;
            int box_h  = th + pad * 2;

            /* position box above the sign, centered */
            int box_x  = sign_screen_x + s->pos.w / 2 - box_w / 2;
            int box_y  = sign_screen_y - box_h - 8;

            /* keep box inside screen area */
            if (box_x < screen_area.x)
                box_x = screen_area.x;
            if (box_x + box_w > screen_area.x + screen_area.w)
                box_x = screen_area.x + screen_area.w - box_w;

            /* draw dark background box */
            SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND);
            SDL_SetRenderDrawColor(r, 0, 0, 0, 200);
            SDL_Rect box = {box_x, box_y, box_w, box_h};
            SDL_RenderSetClipRect(r, &screen_area);
            SDL_RenderFillRect(r, &box);

            /* draw white border */
            SDL_SetRenderDrawColor(r, 255, 255, 255, 255);
            SDL_RenderDrawRect(r, &box);

            /* draw text */
            SDL_Rect text_dst = {box_x + pad, box_y + pad, tw, th};
            SDL_RenderCopy(r, tex, NULL, &text_dst);
            SDL_DestroyTexture(tex);

            SDL_RenderSetClipRect(r, NULL);
        }
    }
    SDL_RenderSetClipRect(r, NULL);
}

void free_signs(Sign *signs, int count)
{
    for (int i = 0; i < count; i++) {
        if (signs[i].texture) {
            SDL_DestroyTexture(signs[i].texture);
            signs[i].texture = NULL;
        }
    }
}
