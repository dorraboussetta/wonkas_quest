#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <string.h>
#include "save_menu.h"
#include "load_menu.h"
#include "player_menu.h"
#include "mainmenu.h"
#include "common.h"
#include "submenu.h"
#include "enemy.h"
#include "player.h"
#include "outfit_menu.h"
#include "background.h"
#include "entities.h"
#include "collision_bb.h"
#include "collision_perfect.h"
#include "minimap.h"

/* =========================================================
   Camera shake counter — set by collision helpers,
   counted down by 1 each frame in the game loop.
   ========================================================= */
static int g_shakeTimer = 0;

/* =========================================================
   ground_world_y[2]
   World-space Y of the player's feet on the platform they
   are standing on.  Set on landing, cleared on jump.
   -1 = player is in the air.
   Needed because pos_init is screen-space and the camera
   moves every frame — without this the parabola anchor
   drifts and the player teleports.
   ========================================================= */
static int   ground_world_y[2]  = {-1, -1};
static float fall_vy[2]         = {0.0f, 0.0f};
static int   floor_screen_y[2]  = {500, 500}; /* set from p[i].pos_screen.y after init */

#define GRAVITY       0.5f
#define MAX_FALL      16.0f

/* sync_player_to_ground
   Call BEFORE update_players each frame.
   Re-derives pos_init from ground_world_y using the current
   camera so the parabola always has the right screen anchor. */
static void sync_player_to_ground(Player *pl, SDL_Rect camera,
                                  int player_index)
{
    if (ground_world_y[player_index] < 0) return;
    if (pl->up != 0) return;
    int screen_y     = ground_world_y[player_index] - camera.y;
    pl->pos_init     = screen_y;
    pl->pos_screen.y = screen_y;
}

/* Forward declaration — apply_platform_collisions is defined below apply_gravity */
static void apply_platform_collisions(Player *pl, Platform *platforms,
                                      int count, SDL_Rect camera,
                                      int player_index);

/* apply_gravity
   Called AFTER apply_platform_collisions each frame.
   If the player is airborne (no platform under them, not in a jump arc),
   accelerate them downward and re-run collision so they land cleanly.
   Also clamps to floor_screen_y so the player never falls through the ground. */
static void apply_gravity(Player *pl, Platform *platforms, int count,
                          SDL_Rect camera, int player_index)
{
    /* Jump arc controls Y — leave it alone */
    if (pl->up != 0) { fall_vy[player_index] = 0.0f; return; }

    /* Standing on a platform — no falling */
    if (ground_world_y[player_index] >= 0) { fall_vy[player_index] = 0.0f; return; }

    int floor_sy = floor_screen_y[player_index];

    /* Already at or below the floor — snap and stop */
    if (pl->pos_screen.y >= floor_sy) {
        pl->pos_screen.y  = floor_sy;
        pl->pos_init      = floor_sy;
        fall_vy[player_index] = 0.0f;
        /* Mark as grounded so next frame's sync is a no-op */
        ground_world_y[player_index] = floor_sy + camera.y;
        return;
    }

    /* Airborne — apply gravity */
    fall_vy[player_index] += GRAVITY;
    if (fall_vy[player_index] > MAX_FALL)
        fall_vy[player_index] = MAX_FALL;

    pl->pos_screen.y += (int)fall_vy[player_index];
    pl->pos_init      = pl->pos_screen.y;   /* keep jump arc anchor current */

    /* Clamp to floor after moving */
    if (pl->pos_screen.y > floor_sy) {
        pl->pos_screen.y  = floor_sy;
        pl->pos_init      = floor_sy;
        fall_vy[player_index] = 0.0f;
        ground_world_y[player_index] = floor_sy + camera.y;
        return;
    }

    /* Re-check platforms — the player may have fallen onto one */
    apply_platform_collisions(pl, platforms, count, camera, player_index);
    if (ground_world_y[player_index] >= 0)
        fall_vy[player_index] = 0.0f;
}

/* =========================================================
   apply_platform_collisions

   Works with the ORIGINAL parabola jump in player.c.
   No gravity, no world_y — player.c is untouched.

   HOW THE PARABOLA WORKS:
     jump_player sets:
       pos_screen.y = pos_init - (-0.04 * rel_x² + 100)
     rel_x goes from -50 → +50.
     At rel_x=-50 and rel_x=+50, pos_screen.y == pos_init.
     Peak is at rel_x=0 where offset = 100px above pos_init.
     When rel_x >= 50 the jump ends and pos_screen.y = pos_init.

   THE TRICK FOR LANDING ON PLATFORMS:
     pos_init IS the ground level in screen space.
     To make a platform act as the floor we just set
       pos_init = platform_top_in_screen_space
     and end the arc (rel_x=50, up=0).
     The parabola then stays at that Y — no fighting.

   COORDINATE SPACES:
     pos_screen  = screen space  (0 .. SCREEN_W/H)
     platform.pos = world space
     camera.x/y  = world origin of the current view
     world = screen + camera,  screen = world - camera
   ========================================================= */
static void apply_platform_collisions(Player *pl, Platform *platforms,
                                      int count, SDL_Rect camera,
                                      int player_index)
{
    /* Player rect in world space */
    SDL_Rect pw = {
        pl->pos_screen.x + camera.x,
        pl->pos_screen.y + camera.y,
        pl->pos_screen.w,
        pl->pos_screen.h
    };

    /* Clear ground each frame; re-set below if still touching a platform */
    ground_world_y[player_index] = -1;

    /* Only allow top-face landing when descending or standing */
    int descending = (pl->up == 0) || (pl->rel_x >= 0);

    for (int i = 0; i < count; i++) {
        Platform *p = &platforms[i];
        if (!p->status) continue;
        if (!CheckAABB(pw, p->pos)) continue;

        int ov_top    = (pw.y + pw.h) - p->pos.y;
        int ov_bottom = (p->pos.y + p->pos.h) - pw.y;
        int ov_left   = (pw.x + pw.w) - p->pos.x;
        int ov_right  = (p->pos.x + p->pos.w) - pw.x;

        /* Pick smallest-penetration face, but give vertical faces priority:
           a horizontal wall can only win if it penetrates strictly less.
           Using ov_top >= 0 (not > 0) keeps a flush-touching player grounded
           instead of letting a side-wall push steal the resolution.          */
        int min_ov = 999999;
        int face   = -1;

        if (descending  && ov_top    >= 0 && ov_top    < min_ov)
            { min_ov = ov_top;    face = 0; }
        if (!descending && ov_bottom >= 0 && ov_bottom < min_ov)
            { min_ov = ov_bottom; face = 1; }
        /* Horizontal faces only win when strictly less than the vertical overlap */
        if (ov_left  > 0 && ov_left  < min_ov)
            { min_ov = ov_left;   face = 2; }
        if (ov_right > 0 && ov_right < min_ov)
            { min_ov = ov_right;  face = 3; }

        if (face == -1) continue;

        switch (face)
        {
            case 0: /* TOP -- land on platform */
            {
                /* Store feet in WORLD space so sync_player_to_ground can
                   keep pos_init correct even as the camera moves.        */
                int world_feet = p->pos.y - pl->pos_screen.h;
                ground_world_y[player_index] = world_feet;

                pl->pos_screen.y = world_feet - camera.y;
                pl->pos_init     = pl->pos_screen.y;
                pl->rel_x        = 50;
                pl->up           = 0;

                if (p->type == PLATFORM_MOVING) {
                    int dx = p->pos.x - p->prev_x;
                    pl->pos_screen.x += dx;
                    if (pl->pos_screen.x < 0)
                        pl->pos_screen.x = 0;
                    if (pl->pos_screen.x + pl->pos_screen.w > pl->max_x)
                        pl->pos_screen.x = pl->max_x - pl->pos_screen.w;
                    if (g_shakeTimer == 0) g_shakeTimer = SHAKE_DURATION;
                }
                if (p->type == PLATFORM_DESTRUCTIBLE) {
                    destroy_platform(p);
                    ground_world_y[player_index] = -1;
                    if (g_shakeTimer == 0) g_shakeTimer = SHAKE_DURATION;
                }

                pw.y = pl->pos_screen.y + camera.y;
                break;
            }

            case 1: /* BOTTOM -- hit ceiling while jumping up */
                pl->pos_screen.y = p->pos.y + p->pos.h - camera.y;
                pl->rel_x = 1;
                pw.y = pl->pos_screen.y + camera.y;
                break;

            case 2: /* LEFT WALL */
                pl->pos_screen.x = p->pos.x - pw.w - camera.x;
                pl->velocity     = 0;
                pl->acceleration = 0;
                pw.x             = pl->pos_screen.x + camera.x;
                break;

            case 3: /* RIGHT WALL */
                pl->pos_screen.x = p->pos.x + p->pos.w - camera.x;
                pl->velocity     = 0;
                pl->acceleration = 0;
                pw.x             = pl->pos_screen.x + camera.x;
                break;
        }
    }
}

/* =========================================================
   apply_perfect_collision_bg
   Per-pixel collision against an optional mask surface.
   Pass NULL to skip silently.
   ========================================================= */
static void apply_perfect_collision_bg(Player *pl, SDL_Surface *mask)
{
    if (!mask) return;
    if (CollisionBottom(pl->pos_screen, mask, &g_shakeTimer)) {
        pl->up       = 0;
        pl->velocity = 0;
    }
    if (CollisionTop(pl->pos_screen, mask, &g_shakeTimer))
        pl->velocity = 0;
    if (CollisionLeft(pl->pos_screen, mask, &g_shakeTimer)) {
        if (pl->direction == LEFT) { pl->velocity = 0; pl->acceleration = 0; }
    }
    if (CollisionRight(pl->pos_screen, mask, &g_shakeTimer)) {
        if (pl->direction == RIGHT) { pl->velocity = 0; pl->acceleration = 0; }
    }
}

/* =========================================================
   render_half
   Draws one viewport (half-screen or full-screen).
   The minimap is NOT drawn here — it is drawn after this
   call returns so it is never clipped by the viewport rect.
   ========================================================= */
//For maram: render_half receives real enemy pointers so rendering cannot freeze copied weapon state
static void render_half(SDL_Renderer *renderer,
                        Background_Level *bg,
                        SDL_Rect camera, SDL_Rect screen,
                        Platform *platforms, int count,
                        Player *players, int num_players,
                        Enemy **enemies, int num_enemies,
                        Entity *coins, int num_coins,
                        Sign *signs, int sign_count,
                        TTF_Font *font,
                        int split_mode, int player_index)
{
    SDL_RenderSetClipRect(renderer, &screen);

    /* Background */
    SDL_RenderCopy(renderer, bg->texture, &camera, &screen);

    /* Platforms */
    for (int i = 0; i < count; i++) {
        if (!platforms[i].status || !platforms[i].texture) continue;
        SDL_Rect dst = {
            screen.x + (platforms[i].pos.x - camera.x),
            screen.y + (platforms[i].pos.y - camera.y),
            platforms[i].pos.w,
            platforms[i].pos.h
        };
        SDL_RenderCopy(renderer, platforms[i].texture, NULL, &dst);
    }

    /* Game objects */
    display_players(players, renderer, num_players, SCREEN_WIDTH, SCREEN_HEIGHT, camera);
    for (int i = 0; i < num_enemies; i++)
        display_enemy(renderer, enemies[i], camera);
    display_ES(renderer, coins, num_coins, camera);

    /* Signs / hints */
    render_signs(signs, sign_count, renderer, camera, screen, font, player_index);

    SDL_RenderSetClipRect(renderer, NULL);
}

int main()
{
    SDL_Window   *window   = NULL;
    SDL_Renderer *renderer = NULL;
    SDL_Event event;

    SaveMenu         saveMenu;
    LoadMenu         loadMenu;
    MainMenu         main_menu;
    player_mode      pm;
    player_selection ps;

    image      opt_bg, opt_text[2];
    button     opt_btn[5];
    Mix_Music *opt_music  = NULL;
    Mix_Chunk *opt_effect = NULL;
    int        opt_volume = MIX_MAX_VOLUME;
    int        opt_prev_interface = 0;

    Enemy  enemy1, enemy2, enemy3;
    Entity coins[100];
    int    num_coins_level1 = 0, num_coins_level2 = 0;
    Background_temp bg; 

    Uint32 dt_player, t_prev;
    int    num_players = 1;
    Player p[2];

    player_outfit po[2];
    outfit_menu   om;
    int players_initialized = 0;

    int running   = 1, level = 1;
    int interface = 7;
    int click = 0, motion = 0, num_button_modified = -1, mx = 0, my = 0, click_time;
    int last_hover  = -1;
    int dt, current_time, previous_time = SDL_GetTicks();
    const Uint8 *keyboard_keys = SDL_GetKeyboardState(NULL);

    Background_Level bg1, bg2;
    Platform         lvl1[MAX_PLATFORMS], lvl2[MAX_PLATFORMS];
    int              count1 = 0, count2 = 0;

    Sign signs[MAX_SIGNS];
    int  sign_count = 0;

    Timer       *timer = NULL;
    SDL_Texture *highscore_img;
    SDL_Texture *history_img;
    Background_temp bg_temp;

    int split_mode = 0;

    /* Minimap — one per level */
    minimap mm1, mm2;
    int     mm1_ok = 0, mm2_ok = 0;

    srand(SDL_GetTicks());

    if (SDL_Systems_init() == 1) return 1;

    window = create_window("Wonka", SCREEN_WIDTH, SCREEN_HEIGHT);
    if (!window)   { printf("Window creation failed.\n");   return 1; }

    renderer = create_renderer(window);
    if (!renderer) { printf("Renderer creation failed.\n"); return 1; }

    /* Load backgrounds and platforms */
    if (load_all_assets(renderer, &bg1, &bg2, lvl1, &count1, lvl2, &count2) != 0) {
        printf("Error loading background assets\n");
        return 1;
    }

    init_signs(signs, &sign_count, renderer, 1);
    timer = create_timer("Assets_background/font.otf", 24);

    /* ── Init minimaps ─────────────────────────────────────────────────
       Create a small BMP thumbnail of each level background and place it
       in Assets_background/.  In GIMP: open the full background image,
       go to Image > Scale Image, set width to ~20% of the original,
       then File > Export As > minimap_level1.bmp  (or level2).
       If the file is missing InitMinimap prints an error and the game
       continues normally without the minimap.
    ──────────────────────────────────────────────────────────────────── */
    mm1_ok = (InitMinimap(&mm1, renderer,
                          "Assets_background/minimap_level1.bmp", NULL,
                          SCREEN_WIDTH, SCREEN_HEIGHT,
                          bg1.map_w, bg1.map_h) == 0);

    mm2_ok = (InitMinimap(&mm2, renderer,
                          "Assets_background/minimap_level2.bmp", NULL,
                          SCREEN_WIDTH, SCREEN_HEIGHT,
                          bg2.map_w, bg2.map_h) == 0);

    if (init_main_menu(renderer, &main_menu) == 1) {
        printf("Main menu initialisation failed.\n"); return 1; }

    init_save_menu(&saveMenu, renderer);
    init_load_menu(&loadMenu, renderer);
    init_player_mode(&pm, renderer);
    init_player_selection(&ps, renderer);

    if (init_option_menu(renderer, &opt_bg, opt_text, opt_btn,
                         &opt_music, &opt_effect) == 1) {
        printf("Options menu initialisation failed.\n"); return 1; }

    /* players initialized after outfit selection — see case 6 */
    t_prev = SDL_GetTicks();

    if (initialize_enemy(&enemy1, renderer, "Scrubitt") != 0) {
        printf("Error enemy1 initialization.\n"); return 1; }
    if (initialize_enemy(&enemy2, renderer, "Slugworth") != 0) {
        printf("Error enemy2 initialization.\n"); return 1; }
    if (initialize_enemy(&enemy3, renderer, "Noodle") != 0) {
        printf("Error enemy3 initialization.\n"); return 1; }
    if (initialize_ES_level1(renderer, coins, &num_coins_level1) == 1) {
        printf("Error entities level 1 initialization.\n"); return 1; }

    highscore_img = init_highscore_menu(renderer);
    history_img   = init_history_menu(renderer);

    /* Default player init so interface=7 works immediately */
    init_players(p, renderer, num_players, SCREEN_WIDTH, SCREEN_HEIGHT, 0, level, 0, 0);
    floor_screen_y[0] = p[0].pos_screen.y;
    floor_screen_y[1] = p[0].pos_screen.y;
    players_initialized = 1;


    /* ================================================================
       MAIN LOOP
    ================================================================ */
    while (running)
    {
        current_time   = SDL_GetTicks();
        dt             = current_time - previous_time;
        previous_time  = current_time;

        dt_player = SDL_GetTicks() - t_prev;
        t_prev    = SDL_GetTicks();

        SDL_RenderClear(renderer);

        /* Tick down camera shake */
        if (g_shakeTimer > 0) g_shakeTimer--;

        SDL_Rect full_screen = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};

        /* ── EVENT POLLING ──────────────────────────────────────────── */
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT) running = 0;

            if (event.type == SDL_KEYDOWN &&
                event.key.keysym.sym == SDLK_m &&
                (interface == 7 || interface == 8))
                split_mode = !split_mode;

            switch (interface)
            {
                case 0:
                    event_main_menu(main_menu, &click_time, &interface, &click,
                                    &motion, &mx, &my, &num_button_modified,
                                    event, &last_hover);
                    if (interface == 9) running = 0;
                    break;
                case 1:
                    event_options(event, opt_btn, opt_music, opt_effect,
                                  window, &opt_volume, &interface, &opt_prev_interface);
                    break;
                case 2: handle_save_menu_input(&saveMenu, event, &interface);    break;
                case 3: handle_load_menu_input(&loadMenu, event, &interface);    break;
                case 4:
                    handle_player_mode_input(&pm, event, &interface);
                    if(interface == 5 && pm.num_players == 2){
                        num_players = 2;
                        init_outfit_menu(renderer, po, &om, num_players, 0, level);
                        interface = 6;
                    }
                    break;
                case 5:
                    handle_player_selection_input(&ps, event, &interface);
                    if(interface == 6 && !players_initialized){
                        num_players = pm.num_players;
                        init_outfit_menu(renderer, po, &om, num_players, ps.player, level);
                    }
                    break;
                case 6:
                    handle_outfit_menu(po, event, &om, num_players, &interface);
                    if(interface == 7){
                        free_players(p, num_players);
                        init_players(p, renderer, num_players, SCREEN_WIDTH, SCREEN_HEIGHT,
                                     po[0].player, level, po[0].outfit, po[1].outfit);
                        floor_screen_y[0] = p[0].pos_screen.y;
                        floor_screen_y[1] = (num_players==2) ? p[1].pos_screen.y : p[0].pos_screen.y;
                        players_initialized = 1;
                    }
                    break;
                case 7:
                case 8:
                    event_players(p, event, num_players);
                    if (event.type == SDL_KEYDOWN) {
                        if (event.key.keysym.sym == SDLK_v) {
                            saveMenu.prev_interface = interface;
                            interface = 2;
                        }
                        if (event.key.keysym.sym == SDLK_b) {
                            opt_prev_interface = interface;
                            interface = 1;
                        }
                    }
                    break;
            }
        }

        /* ── UPDATE (menus) ─────────────────────────────────────────── */
        switch (interface)
        {
            case 0:
                update_main_menu(&main_menu, click_time, &running, &click,
                                 motion, interface, num_button_modified);
                break;
            case 1: update_options_menu(renderer, opt_bg, opt_text, opt_btn); break;
            case 2: update_save_menu(&saveMenu);  break;
            case 3: update_load_menu(&loadMenu);  break;
            case 4: update_player_mode(&pm);      break;
            case 5: update_player_selection(&ps); break;
            case 6: break;  /* no update needed for outfit menu */
        }

        /* ── RENDER (menus) ─────────────────────────────────────────── */
        switch (interface)
        {
            case 0:
                display_main_menu(renderer, main_menu, num_button_modified);
                if (!Mix_PlayingMusic()) Mix_PlayMusic(main_menu.bg.bg_sound, -1);
                break;
            case 1:
            {
                static int opt_music_started = 0;
                if (!opt_music_started) {
                    Mix_HaltMusic();
                    Mix_PlayMusic(opt_music, -1);
                    opt_music_started = 1;
                }
                display_options_menu(renderer);
                break;
            }
            case 2:
                if (Mix_PlayingMusic()) Mix_HaltMusic();
                display_save_menu(saveMenu, renderer);
                break;
            case 3:
                if (Mix_PlayingMusic()) Mix_HaltMusic();
                display_load_menu(loadMenu, renderer);
                break;
            case 4:
                if (Mix_PlayingMusic()) Mix_HaltMusic();
                display_player_mode(&pm, renderer);
                break;
            case 5:
                if (Mix_PlayingMusic()) Mix_HaltMusic();
                display_player_selection(&ps, renderer);
                break;
            case 6:
                if (Mix_PlayingMusic()) Mix_HaltMusic();
                display_outfit_menu(renderer, po, &om, num_players);
                break;
        }

        /* ── GAMEPLAY ───────────────────────────────────────────────── */
        switch (interface)
        {
            /* ========================================================= */
            case 7:  /* LEVEL 1                                           */
            /* ========================================================= */
            {
                if (Mix_PlayingMusic()) Mix_HaltMusic();

                /* Sync pos_init from world ground BEFORE physics so the jump
                   arc anchor is correct even when the camera has scrolled. */
                sync_player_to_ground(&p[0], bg1.camera1, 0);
                if (num_players == 2)
                    sync_player_to_ground(&p[1], bg1.camera2, 1);

                /* Update players */
                update_players(p, dt_player, num_players, SCREEN_WIDTH, SCREEN_HEIGHT);

                /* Move platforms, resolve collisions, then apply gravity */
                float delta_sec = (float)dt / 1000.0f;
                update_mobile(lvl1, count1, delta_sec, bg1.map_w);

                apply_platform_collisions(&p[0], lvl1, count1, bg1.camera1, 0);
                apply_gravity(&p[0], lvl1, count1, bg1.camera1, 0);
                if (num_players == 2) {
                    apply_platform_collisions(&p[1], lvl1, count1, bg1.camera2, 1);
                    apply_gravity(&p[1], lvl1, count1, bg1.camera2, 1);
                }

                /* Enemy logic */
                int target1_index = choose_enemy_target(&enemy1, p, num_players, split_mode, bg1.camera1, bg1.camera2);
                int target2_index = choose_enemy_target(&enemy2, p, num_players, split_mode, bg1.camera1, bg1.camera2);
                SDL_Rect target_camera1 = (target1_index==1) ? bg1.camera2 : bg1.camera1;
                SDL_Rect target_camera2 = (target2_index==1) ? bg1.camera2 : bg1.camera1;
                

		//collision_enemy_bg(&enemy1, bg);
		//collision_enemy_bg(&enemy2, bg); 
               	update_enemy_State(&enemy1, p[target1_index],lvl1, target_camera1, level);
		update_enemy(&enemy1, p[target1_index].pos_screen, level, target_camera1);  
		set_enemy_cycle(&enemy1);
		
		update_enemy_State(&enemy2, p[target2_index], lvl1, target_camera2, level);
		update_enemy(&enemy2, p[target2_index].pos_screen, level, target_camera2); 
		set_enemy_cycle(&enemy2); 
		
                move_enemy(&enemy1, dt);
                move_enemy(&enemy2, dt);
                update_enemy_weapons(&enemy1, bg1.map_w);
                update_enemy_weapons(&enemy2, bg1.map_w);
                health_management_enemy(&enemy1, &p[target1_index], level, coins, num_coins_level1, target_camera1);
		health_management_enemy(&enemy2, &p[target2_index], level, coins, num_coins_level1, target_camera2);

                /* Camera */
                update_camera(&bg1,
                    p[0].pos_screen.x, p[0].pos_screen.y,
                    p[0].pos_screen.w, p[0].pos_screen.h,
                    p[1].pos_screen.x, p[1].pos_screen.y,
                    p[1].pos_screen.w, p[1].pos_screen.h,
                    split_mode);

                /* Signs */
                update_signs(signs, sign_count,
                    p[0].pos_screen.x + bg1.camera1.x,
                    p[0].pos_screen.y + bg1.camera1.y,
                    p[0].pos_screen.w, p[0].pos_screen.h, 0);
                update_signs(signs, sign_count,
                    p[1].pos_screen.x + bg1.camera2.x,
                    p[1].pos_screen.y + bg1.camera2.y,
                    p[1].pos_screen.w, p[1].pos_screen.h, 1);

                /* Render */
                {
                    //HERE render both viewports from the same real enemy objects
                    Enemy *enemies_l1[] = {&enemy1, &enemy2};

                    if (split_mode)
                    {
                        render_half(renderer, &bg1,
                            bg1.camera1, bg1.pos_screen1,
                            lvl1, count1, p, num_players,
                            enemies_l1, 2, coins, num_coins_level1,
                            signs, sign_count,
                            timer ? timer->font : NULL, split_mode, 0);

                        render_half(renderer, &bg1,
                            bg1.camera2, bg1.pos_screen2,
                            lvl1, count1, p, num_players,
                            enemies_l1, 2, coins, num_coins_level1,
                            signs, sign_count,
                            timer ? timer->font : NULL, split_mode, 1);

                        /* Dividing line */
                        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                        SDL_RenderDrawLine(renderer,
                            SCREEN_WIDTH / 2, 0, SCREEN_WIDTH / 2, SCREEN_HEIGHT);

                        /* Minimap — AFTER render_half so clip rect is NULL.
                           Shown on the left half for player 0.             */
                        if (mm1_ok) {
                            MAJMinimap(p[0].pos_screen, &mm1, bg1.camera1);
                            DisplayMinimap(&mm1, renderer);
                        }

                        if (timer) {
                            render_timer(timer, renderer, 20, 20);
                            render_timer(timer, renderer, SCREEN_WIDTH / 2 + 20, 20);
                        }
                    }
                    else
                    {
                        SDL_Rect cam_full = {bg1.camera1.x, bg1.camera1.y,
                                             SCREEN_WIDTH, SCREEN_HEIGHT};

                        render_half(renderer, &bg1,
                            cam_full, full_screen,
                            lvl1, count1, p, 1,
                            enemies_l1, 2, coins, num_coins_level1,
                            signs, sign_count,
                            timer ? timer->font : NULL, split_mode, 0);

                        /* Minimap — AFTER render_half, clip rect is NULL */
                        if (mm1_ok) {
                            MAJMinimap(p[0].pos_screen, &mm1, bg1.camera1);
                            DisplayMinimap(&mm1, renderer);
                        }

                        if (timer) render_timer(timer, renderer, 20, 20);
                    }
                }
                break;
            }

            /* ========================================================= */
            case 8:  /* LEVEL 2                                           */
            /* ========================================================= */
            {
                if (Mix_PlayingMusic()) Mix_HaltMusic();

                /* One-time level init */
                if (level != 2) {
                    level = 2;
                    free_ES(coins, num_coins_level1);
                    if (initialize_ES_level2(renderer, coins, &num_coins_level2,
                                             &enemy1, &enemy2, &bg_temp) == 1) {
                        printf("Error level 2 init\n");
                        return 1;
                    }
                    free_signs(signs, sign_count);
                    init_signs(signs, &sign_count, renderer, 2);
                }

                /* Sync pos_init from world ground BEFORE physics. */
                sync_player_to_ground(&p[0], bg2.camera1, 0);
                if (num_players == 2)
                    sync_player_to_ground(&p[1], bg2.camera2, 1);

                /* Update players */
                update_players(p, dt_player, num_players, SCREEN_WIDTH, SCREEN_HEIGHT);

                /* Move platforms, resolve collisions, then apply gravity */
                float delta_sec = (float)dt / 1000.0f;
                update_mobile(lvl2, count2, delta_sec, bg2.map_w);

                apply_platform_collisions(&p[0], lvl2, count2, bg2.camera1, 0);
                apply_gravity(&p[0], lvl2, count2, bg2.camera1, 0);
                if (num_players == 2) {
                    apply_platform_collisions(&p[1], lvl2, count2, bg2.camera2, 1);
                    apply_gravity(&p[1], lvl2, count2, bg2.camera2, 1);
                }

                /* Enemy logic */
                int target3_index = choose_enemy_target(&enemy3, p, num_players, split_mode, bg2.camera1, bg2.camera2);
                SDL_Rect target_camera3 = (target3_index==1) ? bg2.camera2 : bg2.camera1;

		//collision_enemy_bg(&enemy3, bg); 
		collision_enemy_platforms(&enemy3, lvl2);
                update_enemy_State(&enemy3,p[target3_index],lvl2, target_camera3, level);
		update_enemy(&enemy3, p[target3_index].pos_screen, level, target_camera3); 
		set_enemy_cycle(&enemy3);
                move_enemy(&enemy3, dt);
                update_enemy_weapons(&enemy3, bg2.map_w);
                health_management_enemy(&enemy3, &p[target3_index], level, coins, num_coins_level2, target_camera3);

                /* Camera */
                update_camera(&bg2,
                    p[0].pos_screen.x, p[0].pos_screen.y,
                    p[0].pos_screen.w, p[0].pos_screen.h,
                    p[1].pos_screen.x, p[1].pos_screen.y,
                    p[1].pos_screen.w, p[1].pos_screen.h,
                    split_mode);

                /* Signs */
                update_signs(signs, sign_count,
                    p[0].pos_screen.x + bg2.camera1.x,
                    p[0].pos_screen.y + bg2.camera1.y,
                    p[0].pos_screen.w, p[0].pos_screen.h, 0);
                update_signs(signs, sign_count,
                    p[1].pos_screen.x + bg2.camera2.x,
                    p[1].pos_screen.y + bg2.camera2.y,
                    p[1].pos_screen.w, p[1].pos_screen.h, 1);

                /* Render */
                {
                    //HERE render both viewports from the same real enemy object
                    Enemy *enemies_l2[] = {&enemy3};

                    if (split_mode)
                    {
                        render_half(renderer, &bg2,
                            bg2.camera1, bg2.pos_screen1,
                            lvl2, count2, p, num_players,
                            enemies_l2, 1, coins, num_coins_level2,
                            signs, sign_count,
                            timer ? timer->font : NULL, split_mode, 0);

                        render_half(renderer, &bg2,
                            bg2.camera2, bg2.pos_screen2,
                            lvl2, count2, p, num_players,
                            enemies_l2, 1, coins, num_coins_level2,
                            signs, sign_count,
                            timer ? timer->font : NULL, split_mode, 1);

                        /* Dividing line */
                        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                        SDL_RenderDrawLine(renderer,
                            SCREEN_WIDTH / 2, 0, SCREEN_WIDTH / 2, SCREEN_HEIGHT);

                        /* Minimap */
                        if (mm2_ok) {
                            MAJMinimap(p[0].pos_screen, &mm2, bg2.camera1);
                            DisplayMinimap(&mm2, renderer);
                        }

                        if (timer) {
                            render_timer(timer, renderer, 20, 20);
                            render_timer(timer, renderer, SCREEN_WIDTH / 2 + 20, 20);
                        }
                    }
                    else
                    {
                        SDL_Rect cam_full = {bg2.camera1.x, bg2.camera1.y,
                                             SCREEN_WIDTH, SCREEN_HEIGHT};
                        render_half(renderer, &bg2,
                            cam_full, full_screen,
                            lvl2, count2, p, 1,
                            enemies_l2, 1, coins, num_coins_level2,
                            signs, sign_count,
                            timer ? timer->font : NULL, split_mode, 0);

                        /* Minimap */
                        if (mm2_ok) {
                            MAJMinimap(p[0].pos_screen, &mm2, bg2.camera1);
                            DisplayMinimap(&mm2, renderer);
                        }

                        if (timer) render_timer(timer, renderer, 20, 20);
                    }
                }
                break;
            }
        }

        SDL_RenderPresent(renderer);
    }

    /* ── CLEANUP ──────────────────────────────────────────────────────── */
    free_main_menu(main_menu);
    free_save_menu(&saveMenu);
    free_load_menu(&loadMenu);
    free_player_mode(&pm);
    free_player_selection(&ps);
    if(players_initialized) free_outfit_menu(po, &om, num_players);
    free_options_menu(opt_bg, opt_btn, opt_text, opt_music, opt_effect);

    /* free_players(p, num_players); */
    free_enemy(&enemy1);
    free_enemy(&enemy2);
    free_enemy(&enemy3);

    if (level == 1) free_ES(coins, num_coins_level1);
    else            free_ES(coins, num_coins_level2);

    free_background_level(&bg1);
    free_background_level(&bg2);
    free_platforms(lvl1, count1);
    free_platforms(lvl2, count2);
    free_signs(signs, sign_count);
    free_timer(timer);

    if (mm1_ok) FreeMinimap(&mm1);
    if (mm2_ok) FreeMinimap(&mm2);

    SDL_DestroyTexture(highscore_img);
    SDL_DestroyTexture(history_img);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    Mix_CloseAudio();
    IMG_Quit();
    TTF_Quit();
    SDL_Quit();

    return 0;
}
