#pragma once
#include <cstdint>
class Difficulty;
struct Vec2i_mi;

class MineField
{
public:

    bool bomb = false;
    bool covered = true;
    bool flaged = false;
    uint32_t bombcount = 0;

    void initMineField();

    void setRngBombLocations(const short& index);
    static void fill(const short& x, const short& y);
    static bool winCondition();
    bool isABomb(const short& x, const short& y) const;
    static void setFlagHere(const Vec2i_mi& location);
    static void revealBombs(Vec2i_mi& location);
    static void placeRemainingFlags();
};