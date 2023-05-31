#include <cassert>
#include <atomic>
#include <vector>
#include <array>
#include "Difficulty.h"
#include "TextStuff.h"
#include "MineSweeperApp.h"
#include "MineSweeperTimer.h"
#include "FlagCounter.h"
#include "textureWrapper.h"
#include "MineSweeperWindow.h"
#include "initialWindowSizes.h"

void MineSweeperApp::runApp()
{
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    while(m_isAppRunning)
    {
        if(difficultyWindowLoop())
            mainWindowLoop();
    }
}

void MineSweeperApp::mainWindowDraw(Window& win, MineField const& field,
    Timer const& timer, FlagCounter const& fc, auto const&... sprites)
{
    win.clearWindow();
    win.drawTiles(field);
    win.variadicDraw(sprites...);
    win.drawAllSpritesInContainer(fc.getSprites());
    win.drawAllSpritesInContainer(timer.getSprites());
    win.display();
}

void MineSweeperApp::resetGameMainWindow(Timer& timer, FlagCounter& flagCounter, 
    MineField& field, SmileyButton& resetButton)
{
    timer.resetTimer();
    field.resetTiles();
    flagCounter.resetFlagCounter();
    resetButton.changeSmileyFace(SmileyButton::SMILE_UNPRESSED);
    setGameState(GameStates::FIRST_CLICK);
}

void MineSweeperApp::mainWindowLoop()
{
    setGameState(GameStates::FIRST_CLICK);
    auto& diff = Difficulty::get();
    Window mainWnd{"Minesweeper", Window::getCorrectInitialWndSizeMainWnd()};

    sf::Vector2f resetButtonPos{mainWnd.getWidth() / 2.0f -
        TextureWrapper::getTexture(TextureIndicies::SMILEY_FACES).getSize().x / 10.0f, 4.0f};
    SmileyButton resetButton{resetButtonPos};

    std::atomic<bool> isMainWindowOpen{true};
    Timer timer{isMainWindowOpen, mainWnd.getWidth()};

    FlagCounter flagCounter{};
    sf::Sprite titleBar{TextureWrapper::getCorrectTitleBar()};
    MineField field{titleBar.getLocalBounds().height};

    while(mainWnd.isWindowOpen())
    {
        mainWndHandleEvents(mainWnd, timer, resetButton, flagCounter, field);
        mainWindowDraw(mainWnd, field, timer, flagCounter, titleBar, resetButton.getSprite());
        std::this_thread::sleep_for(std::chrono::milliseconds(25));
        if(m_gameState == GameStates::WIN)
        {
            bool doesUserWantToRestart = createGameWonWindow(timer);
            if(!doesUserWantToRestart)
            {
                m_isAppRunning = false;
                mainWnd.closeWindow();
            }
            else resetGameMainWindow(timer, flagCounter, field, resetButton);
        }
    }

    isMainWindowOpen.store(false);
}

//called once per loop in mainWindowLoop() to handle the sfml events
void MineSweeperApp::mainWndHandleEvents(Window& win, Timer& timer, SmileyButton& resetButton, 
    FlagCounter& flagCounter, MineField& field)
{
    sf::Event event;
    while(win.pollEvents(event))
    {
        switch(event.type)
        {
        case sf::Event::EventType::Closed:
        {
            win.closeWindow();
            m_isAppRunning = false;
            return;
        }
        case sf::Event::Resized:
        {
            win.maintainAspectRatio();
            break;
        }
        case sf::Event::EventType::KeyReleased:
        {
            if(event.key.code == sf::Keyboard::R)
            {
                win.closeWindow();
                return;
            }
            break;
        }
        case sf::Event::MouseButtonPressed:
        {
            mainWndMouseButtonPressed(win, event.key.code, resetButton, field, flagCounter);
            break;
        }
        case sf::Event::MouseButtonReleased:            
            mainWndMouseButtonReleased(win, event.key.code, resetButton, timer, flagCounter, field);
            break;
        }
    }
}

//called on mouse button pressed event from mainWndHandleEvents()
void MineSweeperApp::mainWndMouseButtonPressed(Window const& win, sf::Keyboard::Key whichMouseButton, 
    SmileyButton& resetButton, MineField& field, FlagCounter& fc)
{
    auto clickInfo = calcClickInfo(win);

    if(whichMouseButton == sf::Mouse::Right)
    {
        if(clickInfo.isATileClick && m_gameState == GameStates::PLAYING ||
           clickInfo.isATileClick && m_gameState == GameStates::FIRST_CLICK)
        {
            field.setFlagHere(clickInfo.tilePosition, fc);      
        }
    }
    else if(whichMouseButton == sf::Mouse::Left)
    {
        if(isMouseOverSprite(win, resetButton.getSprite()))
            resetButton.changeSmileyFace(SmileyButton::SMILE_PRESSED);
        else if(m_gameState != GameStates::LOSS)
            resetButton.changeSmileyFace(SmileyButton::SHOCKED);
    }
}

//called on mouse button released event from mainWndHandleEvents()
void MineSweeperApp::mainWndMouseButtonReleased(Window const& win, sf::Keyboard::Key whichMouseButton,
    SmileyButton& resetButton, Timer& timer, FlagCounter& flagCounter, MineField& field)
{
    if(whichMouseButton != sf::Mouse::Left) return;

    if(m_gameState != GameStates::LOSS) 
        resetButton.changeSmileyFace(SmileyButton::SMILE_UNPRESSED);

    auto clickLocation = calcClickInfo(win);

    if(!clickLocation.isATileClick)
    {
        if(isMouseOverSprite(win, resetButton.getSprite()))
        {
            resetGameMainWindow(timer, flagCounter, field, resetButton);
            resetButton.changeSmileyFace(SmileyButton::SMILE_UNPRESSED);
        }
        return;
    }
    //else the click was on a tile...

    if(m_gameState == GameStates::FIRST_CLICK)
    {
        firstClickRoutine(timer, clickLocation.tilePosition, field);
    }
    else if(m_gameState == GameStates::PLAYING)
    {
        if(field.isFlagged(clickLocation.tilePosition))
            return;

        if(field.isABomb(clickLocation.tilePosition))
        {
            field.revealBombs(clickLocation.tilePosition);
            setGameState(GameStates::LOSS);
            timer.pauseTimer();
            resetButton.changeSmileyFace(SmileyButton::DEAD);
        }
        else if(field.isCovered(clickLocation.tilePosition))
        {
            field.floodFill(clickLocation.tilePosition);
            if(calcWinCondition(field))
            {
                timer.pauseTimer();
                setGameState(GameStates::WIN);
                resetButton.changeSmileyFace(SmileyButton::GLASSES);
                field.placeRemainingFlags(flagCounter);
            }
        }
    }
}

//sets up the 7 segment display for createGameWonWindow() texture to
//show how much time it took you to complete the game.
//the 7 segment display will look like  DD : HH : MM : SS : T  (the T segment is tenths of a second)
static void setEndScreenDigits(std::array<sf::Sprite, 9>& segments, uint64_t tenthsOfSecondPassed,
    int segmentWidth, int segmentHeight)
{
    int64_t secondsPassed = tenthsOfSecondPassed / 10;
    std::array<int, 4> timePassed
    {
        secondsPassed / 86400,     //days
        secondsPassed / 3600 % 24, //hours
        secondsPassed / 60 % 60,   //minutes
        secondsPassed % 60,        //seconds
    };

    for(int i = 0; i < segments.size() -1; ++i)
    {
        int xVal = segmentWidth * ((i & 1) ? (timePassed[i/2] % 10) : (timePassed[i/2] / 10));
        segments[i].setTextureRect({xVal, 0, segmentWidth, segmentHeight});
    }
    segments[segments.size()-1].setTextureRect({(int)tenthsOfSecondPassed % 10 * 
        segmentWidth, 0, segmentWidth, segmentHeight});

    float distanceFromTop  = 28.0f;//the y distance of the first segment from the top of window
    float distanceFromLeft = 28.0f;//the x distance of the first segment from the left side of window
    float segmentGapDistance = 5.0f;//the distance between the dots/segment categories
    sf::Vector2f segmentPosition{distanceFromLeft, distanceFromTop};
    for(int i = 0; i < segments.size(); ++i)
    {
        segments[i].setPosition(segmentPosition);
        segmentPosition.x += (i & 1) ? (segmentWidth + segmentGapDistance) : segmentWidth;
    }
}

//opens a new window congratulating the player for beating the game,
//showing the time it took for them to win, and providing a button to restart the game
//which when pressed takes the player back to the difficulty selection window.
//returns false if user closed window and true if they press R or clicked the restart button
bool MineSweeperApp::createGameWonWindow(Timer& timer)
{
    SmileyButton restartButton{{90, 160}, SmileyButton::SMILE_UNPRESSED, {2.08f, 2.08f}};
    sf::Sprite backGround(TextureWrapper::getTexture(TextureIndicies::WIN_SCREEN_BACKGROUND));
    sf::Sprite dots1(TextureWrapper::getTexture(TextureIndicies::SEGMENT_DOTS));
    sf::Sprite dots2(TextureWrapper::getTexture(TextureIndicies::SEGMENT_DOTS));
    dots1.setPosition({60, 34});
    dots2.setPosition({125, 34});

    auto const& segmentTexture = TextureWrapper::getTexture(TextureIndicies::SEVEN_SEGMENTS);
    const auto textureSize = segmentTexture.getSize();
    std::array<sf::Sprite, 9> segmentDisplay{};
    for(auto& sprite : segmentDisplay)
        sprite.setTexture(segmentTexture);

    setEndScreenDigits(segmentDisplay, timer.getTenthsOfSecondPassed(),
        textureSize.x / 12, textureSize.y);

    Window gameWonWindow{"congratulations", {INITIAL_GAME_WON_WINDOW_SIZE_X,
        INITIAL_GAME_WON_WINDOW_SIZE_Y}};

    while(gameWonWindow.isWindowOpen())
    {
        sf::Event event{};
        while(gameWonWindow.pollEvents(event))
        {
            switch(event.type)
            {
            case sf::Event::Closed:
            {
                gameWonWindow.closeWindow();
                return false;
            }
            case sf::Event::EventType::KeyReleased:
            {
                if(event.key.code == sf::Keyboard::R)
                {
                    gameWonWindow.closeWindow();
                    return true;
                }
                break;
            }
            case sf::Event::Resized:
            {
                gameWonWindow.maintainAspectRatio();
                break;
            }
            case sf::Event::MouseButtonPressed:
            {
                if(isMouseOverSprite(gameWonWindow, restartButton.getSprite()))
                {
                    restartButton.changeSmileyFace(SmileyButton::SMILE_PRESSED);
                }
                break;
            }
            case sf::Event::MouseButtonReleased:
            {
                if(isMouseOverSprite(gameWonWindow, restartButton.getSprite()))
                {
                    gameWonWindow.closeWindow();
                    return true;
                }
            }
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        gameWonWindow.clearWindow();
        gameWonWindow.variadicDraw(backGround, restartButton.getSprite());
        gameWonWindow.drawAllSpritesInContainer(segmentDisplay);
        gameWonWindow.display();
    }
}

//calculate where the mouse is and which tile (if any) was clicked
MineSweeperApp::LeftClickInfo MineSweeperApp::calcClickInfo(Window const& win)
{
    LeftClickInfo ret{};
    auto& diff = Difficulty::get();
    ret.mousePosition = win.getMousePosition();

    const auto initialWindowHeight = Window::getCorrectInitialWndSizeMainWnd().y;
    const float ratio = initialWindowHeight / static_cast<float>(INITIAL_TITLEBAR_HEIGHT);
    const float currentTitleBarHeight = win.getHeight() / ratio;
    const float currentTileSize = win.getWidth() / static_cast<float>(diff.getFieldWidth());

    ret.isATileClick = (ret.mousePosition.y >= currentTitleBarHeight);
    ret.tilePosition.x = ret.mousePosition.x / currentTileSize;
    ret.tilePosition.y = (ret.mousePosition.y - currentTitleBarHeight) / currentTileSize;

    return ret;
}

//the routine for the first tile left click. 
//the bombs are generated now so that the first tile clicked on is not a bomb
void MineSweeperApp::firstClickRoutine(Timer& timer, 
    sf::Vector2i const& firstTileClickCoords, MineField& field)
{
    timer.startTimer();
    field.setRngBombLocations(firstTileClickCoords); 
    field.calcAndSetBombCounts();
    field.floodFill(firstTileClickCoords);
    m_gameState = GameStates::PLAYING;
    return;
}

bool MineSweeperApp::isMouseOverSprite(Window const& win, sf::Sprite const& sprite)
{
    auto& pos = sprite.getPosition();
    auto width  = sprite.getGlobalBounds().width;
    auto height = sprite.getGlobalBounds().height;
    sf::Vector2f coords = win.getMappedMousePosition();

    return (coords.x >= pos.x && coords.y >= pos.y && 
        coords.x <= (pos.x + width) && coords.y <= (pos.y + height));
}

int MineSweeperApp::coordsToIndex(sf::Vector2i const& coords)
{
    return coords.x + coords.y * Difficulty::get().getFieldWidth();
}

//figure out if the player won. 
//Does not change the game state just informs if a win happened
bool MineSweeperApp::calcWinCondition(MineField const& field)
{
    uint32_t win = 0;
    auto& diff = Difficulty::get();
    auto const tileCount = diff.getFieldNumOfTiles(), bombCount = diff.getBombCount();

    for(auto const& tile : field.getTiles())
        if(!tile.isCovered()) win++;
     
    return win == (tileCount - bombCount);
}

void MineSweeperApp::difficultyWindowDraw(Window& win, auto const&... drawables)
{
    win.clearWindow();
    win.variadicDraw(drawables...);
    win.display();
}

//switches m_currentWindow to the difficulty selection window.
//returns true if user picked a difficulty and false if they closed the window
bool MineSweeperApp::difficultyWindowLoop()
{   
    auto& diff = Difficulty::get();
    Window diffWnd{"select a difficulty", {INITIAL_DIFF_SELECTION_WINDOW_SIZE_X, 
        INITIAL_DIFF_SELECTION_WINDOW_SIZE_Y}};

    sf::Sprite bkgGround(TextureWrapper::getTexture(TextureIndicies::DIFFICULTY_SELECTION_BACKGROUND));

    TextStuff beginnerTxt{"beginner", {150.0f, 24.0f}, 24, sf::Color::Transparent};
    TextStuff easyTxt    {"easy",     {150.0f, 16.0f}, 31, sf::Color::Transparent};
    TextStuff mediumTxt  {"medium",   {150.0f, 20.0f}, 26, sf::Color::Transparent};
    TextStuff hardTxt    {"hard",     {150.0f, 20.0f}, 27, sf::Color::Transparent};

    sf::Vector2f buttonScale{2.08f, 2.08f};
    SmileyButton beginnerButton{{6, 10}, SmileyButton::SMILE_UNPRESSED, buttonScale};
    SmileyButton easyButton{{66, 10}, SmileyButton::SHOCKED, buttonScale};
    SmileyButton mediumButton{{184.0f, 10.0f}, SmileyButton::GLASSES, buttonScale};
    SmileyButton hardButton{{244.0f, 10.0f}, SmileyButton::DEAD, buttonScale};

    while(diffWnd.isWindowOpen())
    {
        sf::Event event{};
        while(diffWnd.pollEvents(event))
        {
            switch(event.type)
            {
            case sf::Event::EventType::Closed:
                m_isAppRunning = false;
                return false;

            case sf::Event::Resized:
                diffWnd.maintainAspectRatio();
                break;

            case sf::Event::MouseButtonPressed:
                if(isMouseOverSprite(diffWnd, beginnerButton.getSprite()))
                    beginnerButton.changeSmileyFace(SmileyButton::SMILE_PRESSED);
                else if(isMouseOverSprite(diffWnd, easyButton.getSprite()))
                    easyButton.changeSmileyFace(SmileyButton::SMILE_PRESSED);
                else if(isMouseOverSprite(diffWnd, mediumButton.getSprite()))
                    mediumButton.changeSmileyFace(SmileyButton::SMILE_PRESSED);
                else if(isMouseOverSprite(diffWnd, hardButton.getSprite()))
                    hardButton.changeSmileyFace(SmileyButton::SMILE_PRESSED);
                break;

            case sf::Event::MouseButtonReleased:
                beginnerButton.changeSmileyFace(SmileyButton::SMILE_UNPRESSED);
                easyButton.changeSmileyFace(SmileyButton::SHOCKED);
                mediumButton.changeSmileyFace(SmileyButton::GLASSES);
                hardButton.changeSmileyFace(SmileyButton::DEAD);

                difficultyWindowDraw(diffWnd, bkgGround, beginnerButton.getSprite(), easyButton.getSprite(), 
                    mediumButton.getSprite(), hardButton.getSprite(), beginnerTxt.m_txt, 
                    easyTxt.m_txt, mediumTxt.m_txt, hardTxt.m_txt);
                std::this_thread::sleep_for(std::chrono::milliseconds(150));

                if(isMouseOverSprite(diffWnd, beginnerButton.getSprite()))
                    {diff.changeDifficulty(Difficulty::BEGINNER); return true;}
                if(isMouseOverSprite(diffWnd, easyButton.getSprite()))
                    {diff.changeDifficulty(Difficulty::EASY); return true;}
                if(isMouseOverSprite(diffWnd, mediumButton.getSprite()))
                    {diff.changeDifficulty(Difficulty::MEDIUM); return true;}
                if(isMouseOverSprite(diffWnd, hardButton.getSprite()))
                    {diff.changeDifficulty(Difficulty::HARD); return true;}
                break;

            case sf::Event::MouseMoved:
                beginnerTxt.setColor(isMouseOverSprite(diffWnd, beginnerButton.getSprite()) ?
                    sf::Color::Blue : sf::Color::Transparent);
                easyTxt.setColor(isMouseOverSprite(diffWnd, easyButton.getSprite()) ?
                    sf::Color::Blue : sf::Color::Transparent);
                mediumTxt.setColor(isMouseOverSprite(diffWnd, mediumButton.getSprite()) ?
                    sf::Color::Blue : sf::Color::Transparent);
                hardTxt.setColor(isMouseOverSprite(diffWnd, hardButton.getSprite()) ?
                    sf::Color::Blue : sf::Color::Transparent);
                break;
            }
        }

        difficultyWindowDraw(diffWnd, bkgGround, beginnerButton.getSprite(), easyButton.getSprite(), 
            mediumButton.getSprite(), hardButton.getSprite(), beginnerTxt.m_txt, 
            easyTxt.m_txt, mediumTxt.m_txt, hardTxt.m_txt);
    }
}