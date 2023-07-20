#pragma once
#include <cstdint>
#include "MineField.h"
#include "SFML/Graphics.hpp"
#include "Tile.h"
#include "SmileyButton.h"
#include "MineSweeperWindow.h"
#include "Difficulty.h"

class Timer;
class FlagCounter;

enum struct GameStates : uint32_t
{
    LOSS,
    WIN,
    PLAYING,
    FIRST_CLICK,
    IN_FINAL_WINDOW
};

class MineSweeperApp
{
public:
    
    MineSweeperApp()=default;
    ~MineSweeperApp()=default;

    MineSweeperApp(MineSweeperApp const&)=delete;
    MineSweeperApp(MineSweeperApp&&)=delete;
    MineSweeperApp& operator=(MineSweeperApp const&)=delete;
    MineSweeperApp& operator=(MineSweeperApp&&)=delete;

    void runApp();

    void mainWindowDraw(Window& win, MineField const & field, 
        Timer const & timer, FlagCounter const & fc, auto const & ...sprites);

    bool isMouseOverSprite(Window const& win, sf::Sprite const& sprite);

    //auto const& getDifficultySettings() {return m_currentDifficulty;}
    //auto getGameState() {return m_gameState;}
    void setGameState(GameStates gs) {m_gameState = gs;}

    static int coordsToIndex(sf::Vector2i const& coords);

private:

    struct LeftClickInfo //contains everything about where the mouse was clicked and what was clicked on              
    {
        sf::Vector2i mousePosition{0, 0}, tilePosition{0, 0};//mouse and tile coordinates
        bool isATileClick{false};//wether the left lick was over a tile in the minefeild or not 
    };
    LeftClickInfo calcClickInfo(Window const& win);//calculate where the mouse is and which tile (if any) was clicked

    //opens a new window congratulating the player for beating the game,
    //showing the time it took for them to win, and providing a button to restart the game
    //which when pressed takes the player back to the difficulty selection window.
    bool createGameWonWindow(Timer& timer);

    void resetGameMainWindow(Timer& timer, FlagCounter& flagCounter, MineField& field, 
        SmileyButton& resetButton);
    void firstClickRoutine(Timer&, sf::Vector2i const& firstTileClickCoords, MineField& field);
    void mainWindowLoop();
    bool mainWndHandleEvents(Window& win, Timer& timer, 
        SmileyButton& resetButton, FlagCounter& flagCounter, MineField& field);
    bool difficultyWindowLoop();

    //figure out if the player won.
    //Does not change the game state just informs if a win happened
    bool calcWinCondition(MineField const& field);

    void difficultyWindowDraw(Window& win, auto const& ...drawables);

    //called on mouse button down/up events in the main window
    void mainWndMouseButtonPressed(Window const& win, sf::Keyboard::Key whichMouseButton, 
        SmileyButton& resetButton, MineField& field, FlagCounter& fc);
    void mainWndMouseButtonReleased(Window const& win, sf::Keyboard::Key whichMouseButton, SmileyButton& resetButton, 
        Timer& timer, FlagCounter& flagCounter, MineField& field);

    GameStates m_gameState;
    bool m_isAppRunning{true};
};