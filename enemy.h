#ifndef ENEMY_H_INCLUDED
#define ENEMY_H_INCLUDED
#define ground_level_enemy 1786
#define num_platforms 4 
#define num_weapons 15
#define num_projectiles 15
#define sound_volume 128
#include "player.h"
#include "background.h"
/**
 * @enum STATE
 * @brief Represents the AI behaviour state of an enemy.
 */
typedef enum {
    Waiting,   /**< Enemy is idle, too far from or above the player to react. */
    Moving,    /**< Enemy moves randomly within a bounded range. */
    Following, /**< Enemy runs directly toward the player. */
    Attacking,  /**< Enemy is close enough to throw weapons at the player. */
    Jumping /**< Enemy is jumping to chase the player on a platform, or jump back to ground level.*/
} STATE;

/**
 * @enum Cycles
 * @brief Identifies each animation cycle by direction and action.
 */
typedef enum {
    Idle_Right, Idle_Left,
     Walk_Right, Walk_Left,
     Run_Right, Run_Left,
     Jump_Right, Jump_Left,
     Attack_Right, Attack_Left,
     Hurt_Right, Hurt_Left,
     Death_Right, Death_Left
}Cycles;

/**
 * @struct Entity
 * @brief Represents the a game object such as a coin, projectile, or weapon.
 */
typedef struct {
    SDL_Texture *image;  /**< Texture used to render the entity. */
    int status;          /**< 1 if entity is active, 0 if inactive. */
    int score;           /**< Score value awarded on contact (1 or 2). */
    int contact;         /**< 1 if the entity has made contact with another entity, 0 if otherwise. */
    int direction;       /**< Direction of the weapon's movement: 0 = right, 1 = left. */
    int type;            /**< Type: 0 = health coin, 1 = score coin, 2 = the golden key . */
    SDL_Rect pos;        /**< Position and dimensions of each entity. */
} Entity;
typedef struct {
    int is_jumping;        /**< 1 if the character is currently jumping, 0 if otherwise. */
    int jump_threshold;    /**< Maximum jump height (unused). */
    int pos_init;          /**< Initial position at jump start. */
    int was_on_platform;   /**< 1 if the character was on a platform before jumping, 0 if otherwise. */
    int temp_equation;     /**< 1 if using platform-relative jump equation, or in the cases of side/bottom collision with platforms. */
    int was_on_obstacle;   /**< 1 if the character was on a background obstacle before jumping. */
    float x;               /**< Horizontal parameter in the jump parabola equation. */
    float y;               /**< Vertical parameter in the jump parabola equation. */
} jump;

/**
 * @struct Collision
 * @brief Tracks collision state between a character and another object.
 */
typedef struct {
    int status;         /**< 1 if collision is currently active, 0 if otherwise. */
    int x;              /**< X coordinate of the collision point. */
    int y;              /**< Y coordinate of the collision point. */
    int first_contact;  /**< 1 on the very first frame of contact, 0 otherwise. */
    int is_hurt;        /**< 1 if the collision causes damage this frame, 0 if otherwise. */
    int on_top;         /**< 1 if the sprite landed on top of the platform or obstacle, 0 if otherwise. */
    int platform_type;  /**< Type of platform involved in the collision. 0=static platforms, 1=moving platforms, 2=destructible platforms */
} Collision;

/**
 * @struct random_mv
 * @brief Controls timing and bounds for an enemy's random movement pattern.
 */
typedef struct {
    int last_random_switch_time; /**< Timestamp of the last direction change. */
    int control_off_time;        /**< Timestamp when random mode was activated. */
    int pos_min;                 /**< Left boundary of the random movement range. */
    int pos_max;                 /**< Right boundary of the random movement range. */
    int first_rand_switch;       /**< 1 if random mode was just entered. */
    int random_started;          /**< 1 if movement has begun after the initial delay. */
} random_mv;

/**
 * @struct Enemy
 * @brief Represents an AI enemy, also works as a player struct.
 */
typedef struct {
    char name[10];              /**< Character name (e.g. "Wonka", "Slugworth"). */
    SDL_Texture *sprite[14][6]; /**< Animation sprites: [cycle][frame]. */
    Mix_Chunk *shoot_sound;     /**< Sound played when firing a weapon. */
    Mix_Chunk *hurt_sound;      /**< Sound played when taking damage. */
    Mix_Chunk *death_sound;     /**< Sound played on death. */
    Mix_Chunk *huh_sound;       /**< Sound played once when the enemy enters Moving state. */
    int startup_moving_state;  /**<1 if the Scrubit character is in the initial moving state, 0 if otherwise.*/ //TODO NEW
    Mix_Chunk *hey_sound;       /**< Sound played once when the enemy enters Following state. */
    int sound_played;           /**< Tracks one-shot AI voice sounds: 0=none, 1=Moving sound, 2=Following sound. */
    int cycle;                  /**< Current animation cycle (see Cycles enum). */
    int frame;                  /**< Current frame within the cycle. */
    Cycles cycle_requested; /**< The next animation cycle requested by the program. */
    STATE State;            /**< Current AI state (Waiting, Moving, Following, Attacking). */
    SDL_Rect EnPos;             /**< Position and size in world space. */
    int health_status;          /**< Health from 7 (full) to 0 (dead). */
    SDL_Texture *status_bar[8]; /**< Health bar textures indexed by health_status. */
    int lastFrameSwitchTime;    /**< Timestamp of the last frame change. */
    int cycle_switch;           /**< 1 if the cycle just changed and the frame is reset to 0. Then this variable is reset to 0 */
    int is_attacking;           /**< 1 if the attack animation is active. */
    int hurt_animation;         /**< 1 if the hurt animation is playing. */
    int death_animation;        /**< 1 if the death animation is playing. */
    int is_dead;                /**< 1 if the character has finished the death animation. */
    int weapon_fired;           /**< 1 if at least one weapon is currently in flight. */
    int first_throw_frame;      /**< 1 on the frame the weapon is first launched. */
    int movingStateOnTime;       /**< The time the moving state started. */
    int death_y;                /**< Y position locked in at the moment of death. */
    jump jump_var;              /**< Jump physics variables. */
    Collision player_collision;     /**< Player/enemy collision struct. */
    Collision coin_collision;       /**< Coins collision struct. */
    Collision platform_collision;   /**< Platforms collision struct. */
    Collision projectile_collision; /**< Projectiles collision struct. */
    Collision bg_collision;         /**< Background obstacles collision struct. */
    Entity weapons[num_weapons];    /**< Array of weapon entities. */
    random_mv random_mv_var;        /**< Random movement control variables. */
    float velocity;      /**< Base movement speed. */
    float acceleration;  /**< Current acceleration value for running. */
    int accelerate;      /**< 1 if acceleration is on. */
    int deccelerate;     /**< 1 if deceleration is on. */
    int ai_direction; /**<The direction of the cycle. */
} Enemy;


/**
 * @struct Platform_temp
 * @brief Temporary platform structure (to be removed after full integration).
 */
typedef struct {
    SDL_Texture *image;  /**< Platform texture. */
    SDL_Rect pos;        /**< Current position and size. */
    SDL_Rect pos_init;   /**< Initial position (used for moving platforms). */
    int status;          /**< 1 if active, 0 if destroyed. */
    int prev_x;          /**< Previous X position for delta calculation. The delta calculation is used to keep the enemy on the moving platform. */
    int prev_y;          /**< Previous Y position for delta calculation. The delta calculation is used to keep the enemy on the moving platform. */
    int move_back;       /**< 1 if the platform is moving backward, 0 if otherwise. */
    int move_forward;    /**< 1 if the platform is moving forward, 0 if otherwise. */
    int type;            /**< Platform type: 0=static, 1=moving, 2=destructible. */
} Platform_temp;

/**
 * @struct Background_temp
 * @brief Temporary background structure (to be removed after full integration).
 */
typedef struct {
    SDL_Texture *image;         /**< Visible background texture. */
    SDL_Texture *image_mask;    /**< Rendered version of the collision mask. */
    SDL_Surface *mask;          /**< Pixel-readable surface used for background collision. */
    Platform_temp platforms[4]; /**< Array of platforms. */
} Background_temp;


int initialize_enemy(Enemy *enemy, SDL_Renderer *renderer, char name[]); 
void display_enemy(SDL_Renderer *renderer, Enemy *enemy, SDL_Rect camera); 
int initialize_ES_level1(SDL_Renderer *renderer, Entity coins[], int *num_coins_level1); 
int initialize_ES_level2(SDL_Renderer *renderer, Entity coins[], int *num_coins_level2,Enemy *enemy1,Enemy *enemy2, Background_temp *bg); 
void update_enemy_State(Enemy *enemy, Player player, Platform platforms[], SDL_Rect camera,int level);
void navigate_to_platform(Enemy *enemy, Platform platforms[], SDL_Rect player_pos);
void update_enemy(Enemy *enemy, SDL_Rect player_pos, int level, SDL_Rect camera);

void display_ES(SDL_Renderer *renderer, Entity coins[], int num_coins, SDL_Rect camera); 

void enemy_events(SDL_Event event, Enemy *enemy); 
void enemy_keyboard_events(Enemy *enemy, const Uint8 *keyboard_keys); 

void random_movement_level1(Enemy *enemy); 
void random_movement_level2(Enemy *enemy); 
int choose_enemy_target(Enemy *enemy, Player players[], int num_players, int split_mode, SDL_Rect camera1, SDL_Rect camera2);
void set_enemy_cycle(Enemy *enemy); 
void move_enemy(Enemy *enemy, int dt); 
void idle_enemy(Enemy *enemy); 
void walk_enemy(Enemy *enemy); 
void run_enemy(Enemy *enemy, int dt); 
void jump_enemy(Enemy *enemy, int dt); 
void attack_enemy(Enemy *enemy); 
void collision_enemy_platforms (Enemy *enemy, Platform platforms[]); 
void collision_player_enemy(Enemy *enemy, Player player, SDL_Rect camera); //Modify the parameter depending on miriam's work
void collision_enemy_entities(Enemy *enemy, weapon Projectile[], Entity coins[], int num_coins); 
SDL_Color GetPixel(SDL_Surface *bg, SDL_Rect pos);
void collision_enemy_bg(Enemy *enemy, Background_temp bg);  
void health_management_enemy(Enemy *enemy, Player *player, int level, Entity coins[], int num_coins, SDL_Rect camera);
void hurt_enemy(Enemy *enemy); 
void death_enemy(Enemy *enemy);  
/* [CAMERA INTEGRATION] Added SDL_Rect camera parameter — weapons render at pos - camera offset, boundary check uses world coords */
void update_enemy_weapons(Enemy *enemy, int map_w);
void fire_weapon_enemy(Enemy *enemy, SDL_Renderer *renderer, SDL_Rect camera); //To modify after integration. Collision with a player can make the weapon's status equal to 0 
void free_enemy(Enemy *enemy); 
void free_ES(Entity coins[], int num_coins); 

//To remove after integration 
int init_bg(SDL_Renderer *renderer, Background_temp *bg);
void display_bg(SDL_Renderer *renderer, Background_temp *bg);
void free_bg(Background_temp *bg);    
int switch_levels(Entity coins[], int num_coins);  



#endif // ENEMY_H_INCLUDED
