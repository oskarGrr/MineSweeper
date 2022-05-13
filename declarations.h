
// holds declarations for functions in main.cpp global variables and definitions for enumerations

#pragma once
class MineField;
class Difficulty;
class Button;
class TextStuff;

#ifndef EXTERN
#define EXTERN extern
#endif

#define INITIAL_TILE_SIZE 16u
#define INITIAL_TILE_SIZE_F 16.0f

#define INITIAL_TITLE_BAR_SIZEY 32u
                              
EXTERN MineField* g_mineFieldPtr;  // global pointer to heap array of type minefield (a given element of array represents one cell)
                              
EXTERN Button** g_tilePtr;       // pointer to array of pointers each pointing to button (sprite)
         
EXTERN Button** g_segmentsPtr; // pointer to array of pointers each pointing to a texture button (sprite)

EXTERN Difficulty g_diff;    // current difficulty being played

struct Vec2i_mi // contains everything about where the mouse was clicked                
{
    short xm, ym,//x and y coordinates in the window
          x, y;//x and y coordinates in the array of cells

    short index;//the index of the cell inside the MineField array that the  
                //mouse position is over(calculated on left click)(y*arraywidth + x)

    bool isTileClick;//wether the left lick was over a tile in the minefeild or not
};
enum gameStates : uint32_t
{
    loss = 0,
    win = 1,
    playing = 2,
    firstClick = 3,
    inFinalWindow = 4
};
enum leftClickResults : uint32_t
{
    waiting,
    clickedFlag,
    clickedBomb,
    validTile
};
enum DiffOptions : uint32_t
{
    beginner,
    easy,
    medium,
    hard
}; 
enum tileTextures : uint32_t
{
    uncovered,
    one,
    two,
    three,
    four,
    five,
    six,
    seven,
    eight,
    mine,
    redBomb,
    bombCrossed,
    flag,
    hidden
};

void deleteAllocs();
inline void keepAspectRatioMain(sf::RenderWindow&);
void setEndScreenSegments(Button* endSegments, unsigned int& count);
bool finalWindow(uint8_t& gameState, unsigned int& count);
void mainLoop();
Difficulty createDifficultyWindow();
inline uint8_t tileSelection(const short& index);
inline void updateScreen(sf::RenderWindow& msWindow, sf::Sprite& titleBar, sf::Sprite& resetButton);
Vec2i_mi getMousePositionAndIndex(sf::RenderWindow& win);
inline const char* titleBarSelection();
inline void leftClickRoutine(uint8_t& gameState, uint8_t& leftClickResult, Vec2i_mi& location, Button& resetButton, bool& startClock);
void reset(uint8_t& gameState, sf::Texture& t, sf::Texture& Segments, bool& startClock, unsigned int& count);
void Timer(bool& startClock, uint8_t& gameState, unsigned int& count, bool& programRunning);
inline void updateDiffScreen(sf::RenderWindow& diffSelectionWindow, 
/*dayummmnnnn*/              sf::Sprite& backGround,
                             sf::Sprite& beginnerButton,
                             sf::Sprite& ezButton,
                             sf::Sprite& mediumButton,
                             sf::Sprite& hardButton,
                             TextStuff& beginnerText,
                             TextStuff& easyText,
                             TextStuff& mediumText,
                             TextStuff& hardText);