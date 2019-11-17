#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL2_gfxPrimitives.h>

const char *IS_GAME_INITIALIZED = "isGameInitialized";
const char *LEVEL_STR = "currentLevel";
const char *MAP_ID = "id";
const char *MAP_BLOCKS_ARRAY = "blocks_on_grid";
const char *MAP_BLOCKS_ID = "block_id";
const char *MAP_BLOCKS_ROW = "block_row";
const char *MAP_BLOCKS_COL = "block_col";
const char *MAP_BLOCKS_ROTATION = "block_rotation";

const char * ACTUAL_STATUS_FILE_NAME = "actual.json";
const char * MAPS_FILENAME_FORMAT = "maps/%d_level.json";
const char * BLOCKS_TEXTURE_FILENAME_FORMAT = "blocks/%d.png";

const SDL_Rect TABLE_RECT = (SDL_Rect) {120,50,600,600};
