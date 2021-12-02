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
        windowSize_X = 128u;
        windowSize_Y = 128u + TITLE_BAR_SIZEY;
        ratioXdivY = (float)windowSize_X / (float)windowSize_Y;
        ratioYdivX = (float)windowSize_Y / (float)windowSize_X;
        whichDiff = beginner;
        playSize = width * height;
        flagCount = numOfBombs;
        numOfSegments = 6;
        break;
    case easy:
        numOfBombs = 40;
        width = 16;
        height = 16;
        windowSize_X = 256u;
        windowSize_Y = 256u + TITLE_BAR_SIZEY;
        ratioXdivY = (float)windowSize_X / (float)windowSize_Y;
        ratioYdivX = (float)windowSize_Y / (float)windowSize_X;
        whichDiff = easy;
        playSize = width * height;
        flagCount = numOfBombs;
        numOfSegments = 8;
        break;
    case medium:
        numOfBombs = 85;
        width = 30;
        height = 16;
        windowSize_X = 480u;
        windowSize_Y = 256u + TITLE_BAR_SIZEY;
        ratioXdivY = (float)windowSize_X / (float)windowSize_Y;
        ratioYdivX = (float)windowSize_Y / (float)windowSize_X;
        whichDiff = medium;
        playSize = width * height;
        flagCount = numOfBombs;
        numOfSegments = 8;
        break;
    case hard:
        numOfBombs = 230;
        width = 48;
        height = 24;
        windowSize_X = 768u;
        windowSize_Y = 384u + TITLE_BAR_SIZEY;
        ratioXdivY = (float)windowSize_X / (float)windowSize_Y;
        ratioYdivX = (float)windowSize_Y / (float)windowSize_X;
        whichDiff = hard;
        playSize = width * height;
        flagCount = numOfBombs;
        numOfSegments = 8;
    }
}