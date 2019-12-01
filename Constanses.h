#ifndef CONSTANSES_H_INCLUDED
#define CONSTANSES_H_INCLUDED

#define S_WIDTH 900
#define S_HEIGHT 720

#define MAPS_IN_A_LEVEL 5

extern const char *IS_GAME_INITIALIZED;
extern const char *USED_SKIP_BUTTON;
extern const char *SELECTED_MAP_INDEXES_ARR;
extern const char *FINISHED_MAPS_ARR;
extern const char *LEVELS_FILE_NAME;
extern const char *ELAPSED_TIME_STR;
extern const char *IS_USER_DEVELOPER;

extern const char *HAS_TIME_BONUS_BADGE;
extern const char *HAS_NO_SKIP_BADGE;
extern const char *BACKGROUND_FILE_NAME;

extern const char *LEVEL_STR;
extern const char *MAP_ID;
extern const char *MAP_BLOCKS_ARRAY;
extern const char *MAP_BLOCKS_ID;
extern const char *MAP_BLOCKS_ROW;
extern const char *MAP_BLOCKS_COL;
extern const char *MAP_BLOCKS_ROTATION;
extern const char *PLACEABLE_BLOCK_ARRAY;

extern const char * ACTUAL_STATUS_FILE_NAME;
extern const char * MAPS_FILENAME_FORMAT;
extern const char * STAR_FILE_NAME;
extern const char * SETTINGS_FILE_NAME;
extern const char * BLOCKS_TEXTURE_FILENAME_FORMAT;
extern const SDL_Rect TABLE_RECT;
extern const SDL_Rect RIGHT_SIDE_RECT;
extern const SDL_Rect FULL_GAME_TABLE_RECT;

enum {
    BRICK = 1,
    ONE_SIDED_MIRROR = 2,
    TWO_SIDED_MIRROR = 3,
    DOUBLE_REFLECTION_MIRROR = 4,
    GOAL_BLOCK = 999,
    LASER_CANNON = 111
};

enum {
    TIME_BADGE_GOT_BELOW = 30,

    GRID_W = 5,
    SQUARE_W = 120, //TABLERECT.w / GRID_W

    BLOCK_TEXTURE_W = 110,

    SMALLEST_FONT_SIZE = 10,
    BIGGEST_FONT_SIZE = 48,

    LEVEL_NUM = 15,

    M_BTN_NUM = 3,
    S_BTN_NUM = 3,
    IG_BTN_NUM = 4,
    LEVEL_FINISHED_BTN_NUM = 2,
    G_BTN_NUM = LEVEL_NUM+1,
    GAME_FINISHED_BTN_NUM = 1
};

enum {
    //Main menu buttons
    M_GAME_BTN = 101,
    M_SETTINGS_BTN = 102,
    M_EXIT_BTN = 103,

    //Settings buttons
    S_DEV_MODE_BTN = 201,
    S_RESET_BTN = 202,

    //Game menu  buttons
    //all number between: 1 - num of levels
    BACK_TO_MAIN_BTN = 999,

    //In game buttons
    IG_FIRE_BTN = 301,
    IG_RESET_BTN = 302,
    IG_SKIP_BTN = 303,
    IG_PAUSE_BTN = 304,
    IG_BACK_BTN =305,

    FIN_GAME_MENU = 401,
    FIN_NEXT_LEVEL = 402
};


#endif // CONSTANSES_H_INCLUDED
