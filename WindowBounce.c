#include "GameStarter.h"
#include "Constanses.h"

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

Direction getTwoSidedWindowDirection(Direction from, int rotation) {
    //ebben esetben eleg csak 2 allapotot kulonboztetunk meg.
    rotation %= 2;
    if (rotation == 0) {
        if (from == south || from == east)  return from == east ? south : east;
        return from == west ? north : west;
    }
    if (from == south || from == west)  return from == south ? west : south;
    return from == east ? north : east;
}

Direction getWindowDirection(Direction from, int rotation) {
        switch(rotation) {
            case 0:
                if (from == west || from == north) return nowhere;
                return getTwoSidedWindowDirection(from, rotation);
            case 1:
                if (from == north || from == east) return nowhere;
                return getTwoSidedWindowDirection(from, rotation);
            case 2:
               if (from == south || from == east) return nowhere;
                return getTwoSidedWindowDirection(from, rotation);
            case 3:
                if (from == west || from == south) return nowhere;
                return getTwoSidedWindowDirection(from, rotation);
            default:
                return nowhere;
    }
}

Direction findDirection(Direction from, int block_type, int rotation) {
    switch (block_type) {
        case LASER_CANNON:
            return getLaserCannonDirection(rotation);
        case ONE_SIDED_WINDOW:
            return getWindowDirection(from, rotation);
        case TWO_SIDED_WINDOW:
            return getTwoSidedWindowDirection(from, rotation);
        case BRICK:
            return nowhere;
        case GOAL_BLOCK:
               //Mivel a rajzon alapbol kettovel oebb van fotrgatva mint a tükör,
               //és hogy lehessen rajta használni az előző függvényeket. Felfogható egy egyoldalú tükörnek.
               return getWindowDirection(from, (rotation+2)%4);
        case DOUBLE_REFLECTION_WINDOW:
            //Mivel a rajzon egyel odebb van. A masik reflexiot nem ez a fuggveny fogja kezelni.
            return getTwoSidedWindowDirection(from, (rotation+1)%4);
        default:
            return nowhere;
    }
    return nowhere;
}
