#ifndef SAVE_MENU_H_INCLUDED
#define SAVE_MENU_H_INCLUDED

#include "common.h"

typedef struct
{
    Background bg;
    Text       question;
    Button     yes_btn;
    Button     no_btn;

    /* ADDED: Array of pointers to buttons for keyboard navigation */
    Button    *buttons[2];

    /* ADDED: Total number of buttons in the array */
    int        button_count;

    /* ADDED: Index of the currently highlighted (pre-selected) button */
    int        selected_index;

    /* ADDED: Interface we came from (7 or 8) so V can return to it */
    int        prev_interface;

} SaveMenu;

/* Text */
void init_text(Text *t, char *font_path, int size, char *content, SDL_Color color, int x, int y, SDL_Renderer *renderer);
void display_text(Text t, SDL_Renderer *renderer);
void free_text(Text *t);

/* Button */
void init_button(Button *b, char *img1, char *img2, char *img3, int x, int y, SDL_Renderer *renderer);
void display_button(Button b, SDL_Renderer *renderer);
void update_button(Button *b);
void free_button(Button *b);

/* Background */
void init_background(Background *bg, char *img_path, char *music_path, SDL_Renderer *renderer);
void display_background(Background bg, SDL_Renderer *renderer);
void free_background(Background *bg);

/* Save Menu */
void init_save_menu(SaveMenu *menu, SDL_Renderer *renderer);
void handle_save_menu_input(SaveMenu *menu, SDL_Event event, int *state);
void update_save_menu(SaveMenu *menu);
void display_save_menu(SaveMenu menu, SDL_Renderer *renderer);
void free_save_menu(SaveMenu *menu);

#endif /* SAVE_MENU_H_INCLUDED */
