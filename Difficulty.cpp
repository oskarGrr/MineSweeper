#include "Difficulty.h"
#include <cassert>

Difficulty Difficulty::s_theDifficulty;

void Difficulty::changeDifficulty(const Options difficultyOption)
{
    switch(difficultyOption)
    {
    case BEGINNER:
        m_bombCount = 10;
        m_fieldWidth = 8;
        m_fieldHeight = 8;
        m_numOfSegments = 6;
        break;
    case EASY:
        m_bombCount = 40;
        m_fieldWidth = 16;
        m_fieldHeight = 16;
        m_numOfSegments = 8;
        break;
    case MEDIUM:
        m_bombCount = 85;
        m_fieldWidth = 30;
        m_fieldHeight = 16;      
        m_numOfSegments = 8;
        break;
    case HARD:
        m_bombCount = 230;
        m_fieldWidth = 48;
        m_fieldHeight = 24;
        m_numOfSegments = 8;
    }

    m_whichDifficultyOption = difficultyOption;
    m_flagCount = m_bombCount;
}