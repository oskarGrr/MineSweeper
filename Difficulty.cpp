#include "Difficulty.h"
#include "Declarations.h"

Difficulty::Difficulty(uint8_t diffOption) 
{
    switch(diffOption)
    {
    case beginner:
        numOfBombs = 10;
        width = 8;
        height = 8;
        initialWindowSize_X = 128u;
        initialWindowSize_Y = 128u + INITIAL_TITLE_BAR_SIZEY;
        ratioXdivY = (float)initialWindowSize_X / (float)initialWindowSize_Y;
        ratioYdivX = (float)initialWindowSize_Y / (float)initialWindowSize_X;
        whichDiff = beginner;
        playSize = width * height;
        flagCount = numOfBombs;
        numOfSegments = 6;
        break;
    case easy:
        numOfBombs = 40;
        width = 16;
        height = 16;
        initialWindowSize_X = 256u;
        initialWindowSize_Y = 256u + INITIAL_TITLE_BAR_SIZEY;
        ratioXdivY = (float)initialWindowSize_X / (float)initialWindowSize_Y;
        ratioYdivX = (float)initialWindowSize_Y / (float)initialWindowSize_X;
        whichDiff = easy;
        playSize = width * height;
        flagCount = numOfBombs;
        numOfSegments = 8;
        break;
    case medium:
        numOfBombs = 85;
        width = 30;
        height = 16;
        initialWindowSize_X = 480u;
        initialWindowSize_Y = 256u + INITIAL_TITLE_BAR_SIZEY;
        ratioXdivY = (float)initialWindowSize_X / (float)initialWindowSize_Y;
        ratioYdivX = (float)initialWindowSize_Y / (float)initialWindowSize_X;
        whichDiff = medium;
        playSize = width * height;
        flagCount = numOfBombs;
        numOfSegments = 8;
        break;
    case hard:
        numOfBombs = 230;
        width = 48;
        height = 24;
        initialWindowSize_X = 768u;
        initialWindowSize_Y = 384u + INITIAL_TITLE_BAR_SIZEY;
        ratioXdivY = (float)initialWindowSize_X / (float)initialWindowSize_Y;
        ratioYdivX = (float)initialWindowSize_Y / (float)initialWindowSize_X;
        whichDiff = hard;
        playSize = width * height;
        flagCount = numOfBombs;
        numOfSegments = 8;
    }
}