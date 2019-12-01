#include "GameStarter.h"
#include "Constanses.h"

/**
* @param int rotation
* @return Direction dir
*/
Direction getLaserCannonDirection(int rotation) {
    switch (rotation) {
        case 0:
            return west;
        case 1:
            return north;
        case 2:
            return east;
        case 3:
            return south;
        default:
            return nowhere;
    }
}

/** A kétoldalú tükörről való visszapattanásokata kezeli.
* @param Direction from,
* @param int rotation
*/
Direction getTwoSidedMirrorDirection(Direction from, int rotation) {
    //ebben esetben eleg csak 2 allapotot kulonboztetunk meg.
    rotation %= 2;
    if (rotation == 0) {
        if (from == south || from == east)  return from == east ? south : east;
        return from == west ? north : west;
    }
    if (from == south || from == west)  return from == south ? west : south;
    return from == east ? north : east;
}

/**
* Az egyoldalú tükörről való visszapattanásokat kezeli, a kétoldalú függvénynek a segítségével
* @param Direction from
* @param int rotation
*/
Direction getMirrorDirection(Direction from, int rotation) {
        switch(rotation) {
            case 0:
                if (from == west || from == north) return nowhere;
                return getTwoSidedMirrorDirection(from, rotation);
            case 1:
                if (from == north || from == east) return nowhere;
                return getTwoSidedMirrorDirection(from, rotation);
            case 2:
               if (from == south || from == east) return nowhere;
                return getTwoSidedMirrorDirection(from, rotation);
            case 3:
                if (from == west || from == south) return nowhere;
                return getTwoSidedMirrorDirection(from, rotation);
            default:
                return nowhere;
    }
}


/** Megmondja, hogy egy adott irányból egy adott típusú blokkról milyen irányba pattan le a lézersugár.
*@param Direction from
*@param int block_type
*@param int rotation
*@return Direcion next
*/
Direction findDirection(Direction from, int block_type, int rotation) {
    //mivel a blokkok rajzai különbözően elforgatva vannak eltérolva, azért hozzá kellett igazítani a rotationet.
    switch (block_type) {
        case LASER_CANNON:
            return nowhere;
        case ONE_SIDED_MIRROR:
            return getMirrorDirection(from, (rotation+3)%4);
        case TWO_SIDED_MIRROR:
            return getTwoSidedMirrorDirection(from, (rotation+1)%4);
        case BRICK:
            return nowhere;
        case GOAL_BLOCK:
               return getMirrorDirection(from, (rotation+3)%4);
        case DOUBLE_REFLECTION_MIRROR:
            return getTwoSidedMirrorDirection(from, rotation);
        default:
            return nowhere;
    }
    return nowhere;
}
