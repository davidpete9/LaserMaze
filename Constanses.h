#ifndef CONSTANSES_H_INCLUDED
#define CONSTANSES_H_INCLUDED

#define S_WIDTH 900
#define S_HEIGHT 720

extern const char *IS_GAME_INITIALIZED;
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
extern const char * BLOCKS_TEXTURE_FILENAME_FORMAT;
extern const SDL_Rect TABLE_RECT;
extern const SDL_Rect RIGHT_SIDE_RECT;

enum {
    BRICK = 1,
    ONE_SIDED_WINDOW = 2,
    TWO_SIDED_WINDOW = 3,
    DOUBLE_REFLECTION_WINDOW = 4,
    GOAL_BLOCK = 999,
    LASER_CANNON = 111
};

enum {

    GRID_W = 5,
    SQUARE_W = 120, //TABLERECT.w / GRID_W

    BLOCK_TEXTURE_W = 110,
    MAPS_IN_A_LEVEL = 5,

    SMALLEST_FONT_SIZE = 10,
    BIGGEST_FONT_SIZE = 48,

    LEVEL_NUM = 15,

    M_BTN_NUM = 3,
    S_BTN_NUM = 3,
    IG_BTN_NUM = 5,
    G_BTN_NUM = LEVEL_NUM+1,
};

enum {
    //Main menu buttons
    M_GAME_BTN = 101,
    M_SETTINGS_BTN = 102,
    M_EXIT_BTN = 103,

    //Settings buttons
    S_MUTE_BTN = 201,
    S_RESET_BTN = 202,

    //Game menu  buttons
    //all number between: 1 - num of levels


    BACK_TO_GAME_MENU_BTN = 888,
    BACK_TO_MAIN_BTN = 999,

    //In game buttons
    IG_FIRE_BTN = 301,
    IG_RESET_BTN = 302,
    IG_SKIP_BTN = 303,
    IG_PAUSE_BTN = 304,
};


#endif // CONSTANSES_H_INCLUDED
