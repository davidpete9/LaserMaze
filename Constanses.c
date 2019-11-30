#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL2_gfxPrimitives.h>

const char *IS_GAME_INITIALIZED = "isGameInitialized";
const char *USED_SKIP_BUTTON = "hasSkipButtonUsed";
const char *SELECTED_MAP_INDEXES_ARR = "selectedMapIndexes";
const char *FINISHED_MAPS_ARR = "finishedMaps";

const char *LEVEL_STR = "currentLevel";
const char *MAP_ID = "id";
const char *MAP_BLOCKS_ARRAY = "blocks_on_grid";
const char *MAP_BLOCKS_ID = "block_id";
const char *MAP_BLOCKS_ROW = "block_row";
const char *MAP_BLOCKS_COL = "block_col";
const char *MAP_BLOCKS_ROTATION = "block_rotation";
const char *PLACEABLE_BLOCK_ARRAY = "placeable_blocks";

const char * ACTUAL_STATUS_FILE_NAME = "actual.json";
const char * MAPS_FILENAME_FORMAT = "maps/%d_level.json";
const char * LEVELS_FILE_NAME = "levels.json";
const char * BLOCKS_TEXTURE_FILENAME_FORMAT = "blocks/%d.png";

const SDL_Rect TABLE_RECT = (SDL_Rect) {120,50,600,600};
const SDL_Rect RIGHT_SIDE_RECT = (SDL_Rect) {750,50,120,600};
const SDL_Rect FULL_GAME_TABLE_RECT = (SDL_Rect) {0,50,900,670};




