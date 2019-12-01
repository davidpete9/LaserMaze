#ifndef WINDOWBOUNCE_H_INCLUDED
#define WINDOWBOUNCE_H_INCLUDED

Direction findDirection(Direction from, int block_type,int rotation);
Direction getLaserCannonDirection(int rotation);
Direction getTwoSidedWindowDirection(Direction from, int rotation);
Direction getWindowDirection(Direction from, int rotation);

#endif // WINDOWBOUNCE_H_INCLUDED
