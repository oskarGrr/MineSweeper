#pragma once
#include <SFML/Graphics.hpp>

class Difficulty
{
public:
    short numOfBombs = 0, width = 0, height = 0, playSize = 0, numOfSegments = 0,
          whichDiff = 0, flagCount = 0;

    float ratioXdivY = 0.0f, ratioYdivX = 0.0f;

    uint32_t windowSize_X = 0;
    uint32_t windowSize_Y = 0;

    Difficulty() = default;
    Difficulty(uint8_t diffOption);
};