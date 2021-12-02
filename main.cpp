#include "Button.h"
#include "Difficulty.h"
#include "TextStuff.h"
#include "MineField.h"

#include <SFML/Graphics.hpp>
#include <chrono>   
#include <thread>
#include <cmath>     
#include <iostream>

//blank only in this .cpp file (extern in others)
//this way globals will be defined here but only 
//declared everywhere else automatically
#define EXTERN              
#include "declarations.h"

int main()
{   
    for(;;)
    {
        srand((unsigned int)time(0));
        ::diff = createDifficultyWindow();
        mainLoop();
    }
}

void mainLoop()
{  
    sf::RenderWindow msWindow({diff.windowSize_X, diff.windowSize_Y},"Minesweeper");
    sf::Texture tiles;   
    sf::Texture smileyButton;
    sf::Texture titleBar;
    sf::Texture sevenSegments;
    sevenSegments.loadFromFile("textures/7segments.png");
    tiles.loadFromFile("textures/unseperatedTiles.png");
    smileyButton.loadFromFile("textures/resetButtons.png");
    titleBar.loadFromFile(titleBarSelection());
 
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  
    float resetButtonX = ((float)(diff.windowSize_X / 2) - (smileyButton.getSize().x / 10)); //x coord val of the the resetbutton
    Button resetButton(smileyButton, 0, {resetButtonX, 4.0f}, 5, 1); 
    Button backGroundTitleBar(titleBar, 0, {0,0}, 1, 1);  
    Button::createSegments(sevenSegments);
    Button::createTiles(tiles);
    g_mineFieldPtr = new MineField[diff.playSize];  
    uint8_t leftClickResult = waiting;
    uint8_t gameState = firstClick;
    Vec2i_mi location;
    unsigned int count = 0;
    bool programRunning = true;
    bool startClock = false;
    std::thread clockThread(Timer, std::ref(startClock), std::ref(gameState), std::ref(count), std::ref(programRunning));
    
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    while (msWindow.isOpen())
    {
        sf::Event evnt; 
        while (msWindow.pollEvent(evnt))
        {     

            switch (evnt.type)
            {
            case sf::Event::EventType::Closed:
                msWindow.close();
                programRunning = false;
                clockThread.join();
                deleteAllocs();
                exit(0);           
                break;      
            ///////////////////////////////////////////////////////////////////////////////////////
            case sf::Event::Resized:
                keepAspectRatioMain(msWindow);
                break;
            ///////////////////////////////////////////////////////////////////////////////////////
            case sf::Event::EventType::KeyReleased:// R key restarts the diff selection
                if(evnt.key.code == sf::Keyboard::R)
                {
                    startClock = false;
                    programRunning = false;
                    clockThread.join();
                    deleteAllocs();                   
                    return; 
                }
            ///////////////////////////////////////////////////////////////////////////////////////                 
            case sf::Event::MouseButtonPressed: 
                location = getMousePositionAndIndex(msWindow);
                if(evnt.key.code == sf::Mouse::Right)
                {
                    if(location.isTileClick && gameState == playing ||  
                       location.isTileClick && gameState == firstClick)
                        MineField::setFlagHere(location);                    
                }
                else if(evnt.key.code == sf::Mouse::Left)
                {
                    resetButton.resetButtonLeftClick(gameState, msWindow);     
                }
                break;
            ///////////////////////////////////////////////////////////////////////////////////////
            case sf::Event::MouseButtonReleased:
                location = getMousePositionAndIndex(msWindow);

                if(gameState != loss) resetButton.button.setTextureRect({0, 0, 24, 24});
                else if(gameState == loss)resetButton.button.setTextureRect({72, 0, 24, 24});

                if(location.isTileClick && gameState == playing ||                      
                   location.isTileClick && gameState == firstClick)                
                { 
                    if(evnt.key.code == sf::Mouse::Left)
                    {
                        leftClickRoutine(gameState, leftClickResult, location, resetButton, startClock);                            
                    }
                }
                else if (evnt.key.code == sf::Mouse::Left && resetButton.isMouseOver(msWindow))
                {                   
                    reset(gameState, tiles, sevenSegments, startClock, count);
                    resetButton.button.setTextureRect({0, 0, 24, 24});
                }              
                break;
            }
        }       
        updateScreen(msWindow, backGroundTitleBar.button, resetButton.button);
        std::this_thread::sleep_for(std::chrono::milliseconds(25));

        if(gameState == win) 
        {
            updateScreen(msWindow, backGroundTitleBar.button, resetButton.button);
            if(finalWindow(gameState, count))
            {
                deleteAllocs();
                programRunning = false;
                clockThread.join();
                return;                    
            }
        }
    }
}

void deleteAllocs()
{//this also deletes the segments because this function is called when gameloop exits to reset the difficulty
 // and so new segments are not written over but re created

    int x = 7;

    if(diff.whichDiff == beginner) 
        x = 5;

    for(int i = 0; i < diff.playSize; ++i)
        delete *(g_tilePtr + i); 

    for(int i = 0; i < x; i++)
        delete *(g_segmentsPtr + i);

    delete[] g_segmentsPtr;
    delete[] g_mineFieldPtr;
    delete[] g_tilePtr;
}

void Timer(bool& startClock, uint8_t& gameState, unsigned int& count, bool& programRunning)
{
    int index;
    if(diff.whichDiff == beginner) index = 5;
    else index = 7;
    while(programRunning)
    {  
        while(startClock)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(99));
            Button::incrementClockDisplay(index);
            count++;
        }
        if(gameState == firstClick) Button::refreshClockSegments();       
        std::this_thread::sleep_for(std::chrono::milliseconds(4));
    }   
}

inline void keepAspectRatioMain(sf::RenderWindow& win)
{
    static uint32_t prev_xSize = diff.windowSize_X;
    static uint32_t prev_ySize = diff.windowSize_Y;

    uint32_t new_xSize = win.getSize().x;
    uint32_t new_ySize = win.getSize().y;

    int diff_xSize = abs((int)new_xSize - (int)prev_xSize);
    int diff_ySize = abs((int)new_ySize - (int)prev_ySize);
    
    if(diff_xSize > diff_ySize)    
        new_ySize = new_xSize * diff.ratioYdivX;    
    else if(diff_ySize > diff_xSize)  
        new_xSize = new_ySize * diff.ratioXdivY;
        
    win.setSize({new_xSize, new_ySize});
    prev_xSize = new_xSize;
    prev_ySize = new_ySize;
}

inline void keepAspectRatioDiffSelection(sf::RenderWindow& win, 
    const uint32_t initialSizeX, const uint32_t initialSizeY)
{
    static uint32_t prev_xSize = initialSizeX;
    static uint32_t prev_ySize = initialSizeY;

    uint32_t new_xSize = win.getSize().x;
    uint32_t new_ySize = win.getSize().y;

    int diff_xSize = abs((int)new_xSize - (int)prev_xSize);
    int diff_ySize = abs((int)new_ySize - (int)prev_ySize);
    
    if(diff_xSize > diff_ySize)    
        new_ySize = new_xSize * ((float)initialSizeY/(float)initialSizeX);    
    else if(diff_ySize > diff_xSize)  
        new_xSize = new_ySize * ((float)initialSizeX/(float)initialSizeY);
        
    win.setSize({new_xSize, new_ySize});
    prev_xSize = new_xSize;
    prev_ySize = new_ySize;
}

void keepAspectRatioFinalWindow(sf::RenderWindow& win,
    const uint32_t initialSizeX, const uint32_t initialSizeY)
{
    static uint32_t prev_xSize = initialSizeX;
    static uint32_t prev_ySize = initialSizeY;
    
    uint32_t new_xSize = win.getSize().x;
    uint32_t new_ySize = win.getSize().y;
    
    int diff_xSize = abs((int)new_xSize - (int)prev_xSize);
    int diff_ySize = abs((int)new_ySize - (int)prev_ySize);
    
    if(diff_xSize > diff_ySize)    
        new_ySize = new_xSize * ((float)initialSizeY/(float)initialSizeX);
    else if(diff_ySize > diff_xSize)  
        new_xSize = new_ySize * ((float)initialSizeX/(float)initialSizeY);
        
    win.setSize({new_xSize, new_ySize});
    prev_xSize = new_xSize;
    prev_ySize = new_ySize;
}

inline void leftClickRoutine(uint8_t& gameState, uint8_t& leftClickResult, Vec2i_mi& location, Button& resetButton, bool& startClock)
{
    if(gameState == firstClick)
    {
        startClock = true;
        g_mineFieldPtr->setRngBombLocations(location.index); // minefield is created here so we can know where
        g_mineFieldPtr->initMineField();                    // the first click was as to not generate a bomb there      
        MineField::fill(location.x, location.y);
        gameState = playing;
        return;
    }

    leftClickResult = tileSelection(location.index);

    if(leftClickResult == validTile)
    {
        MineField::fill(location.x, location.y);

        if(MineField::winCondition()) 
        {
            startClock = false;
            resetButton.button.setTextureRect({96, 0, 24, 24});
            MineField::placeRemainingFlags();
            gameState = win;    
        }
        return;
    }
    else if (leftClickResult == clickedBomb)
    {
        MineField::revealBombs(location);
        gameState = loss;
        startClock = false;
        resetButton.button.setTextureRect({72, 0, 24, 24});
    }    
    //if leftClickResult = clickedflag it will just return;  
}

void reset(uint8_t& gameState, sf::Texture& tilesTexture, sf::Texture& Segments, bool& startClock, unsigned int& count)
{
    startClock = false;

    for(int i = 0; i < diff.playSize; ++i) 
    {                 
        delete *(g_tilePtr + i); 
    }//doesnt delete g_segments ptr array or what they point to. this is because if reset is called it still uses the old ones
    delete[] g_mineFieldPtr;
    delete[] g_tilePtr;

    g_mineFieldPtr = new MineField[diff.playSize];
    
    Button::createTiles(tilesTexture);

    Button::refreshFlagSegments(); 
    count = 0;
    gameState = firstClick;
}

Vec2i_mi getMousePositionAndIndex(sf::RenderWindow& win)
{    
    Vec2i_mi tempLocation;
   
    tempLocation.xm = sf::Mouse::getPosition(win).x;
    tempLocation.ym = sf::Mouse::getPosition(win).y;

    tempLocation.x = tempLocation.xm / TILE_SIZE;
    tempLocation.y = (tempLocation.ym - TITLE_BAR_SIZEY ) / TILE_SIZE;

    tempLocation.isTileClick = isMouseInsideField(tempLocation.xm, tempLocation.ym);

    tempLocation.index = tempLocation.x + tempLocation.y * diff.width;

    return tempLocation;
}

inline bool isMouseInsideField(short& xm, short& ym)
{
     if(xm <= diff.windowSize_X && ym <= diff.windowSize_Y + TITLE_BAR_SIZEY && ym >= TITLE_BAR_SIZEY) return true;
     return false; 
}

void setEndScreenSegments(Button* endSegments, unsigned int& count)
{
    int justSeconds = count / 10;   
    unsigned int milliseconds = count % 10;
    unsigned int seconds = justSeconds % 60;
    unsigned int minutes = justSeconds / 60 % 60;
    unsigned int hours = justSeconds / (60 * 60) % 24;
    unsigned int days = justSeconds / (60 * 60 * 24);

    endSegments[0].button.setTextureRect( {(int)days / 10 * 13, 0, 13, 23} );
    endSegments[1].button.setTextureRect( {(int)days % 10 * 13, 0, 13, 23} );
    endSegments[2].button.setTextureRect( {(int)hours / 10 * 13, 0, 13, 23} );
    endSegments[3].button.setTextureRect( {(int)hours % 10 * 13, 0, 13, 23} );
    endSegments[4].button.setTextureRect( {(int)minutes / 10 * 13, 0, 13, 23} );
    endSegments[5].button.setTextureRect( {(int)minutes % 10 * 13, 0, 13, 23} );
    endSegments[6].button.setTextureRect( {(int)seconds / 10 * 13, 0, 13, 23} );
    endSegments[7].button.setTextureRect( {(int)seconds % 10 * 13, 0, 13, 23} );
    endSegments[8].button.setTextureRect( {(int)milliseconds * 13, 0, 13, 23} );

    endSegments[0].button.setPosition({33, 28});
    endSegments[1].button.setPosition({46, 28});
    endSegments[2].button.setPosition({64, 28});
    endSegments[3].button.setPosition({77, 28});
    endSegments[4].button.setPosition({98, 28});
    endSegments[5].button.setPosition({111, 28});
    endSegments[6].button.setPosition({129, 28});
    endSegments[7].button.setPosition({142, 28});
    endSegments[8].button.setPosition({161, 28});
}

bool finalWindow(uint8_t& gameState, unsigned int& count)
{
    sf::Texture d;
    d.loadFromFile("textures/dots.png");
    sf::Sprite dots1(d);
    sf::Sprite dots2(d);
    dots1.setPosition({60, 34});
    dots2.setPosition({125, 34});

    sf::Texture segments;
    segments.loadFromFile("textures/7segments.png");

    Button endSegments[9];
    for(int i = 0; i < 9; i++)    
        endSegments[i].button.setTexture(segments);

    sf::Texture t;
    t.loadFromFile("textures/test.png");
    sf::Sprite backGround(t);
    
    sf::Texture reset;
    reset.loadFromFile("textures/diffSelectionButtons.png");
    Button resetButton;
    resetButton.button.setTexture(reset);
    resetButton.button.setTextureRect({0, 0, 50, 50});
    resetButton.button.setPosition({90, 160});

    setEndScreenSegments(endSegments, count);

    const uint32_t windowSizeX = 220u, windowSizeY = 220u;
    sf::RenderWindow winWindow({windowSizeX, windowSizeY}, "congratz");
    bool flag = 0;
    while(winWindow.isOpen())
    {
        sf::Event e;
        while(winWindow.pollEvent(e))
        {
            switch(e.type) 
            {
            case sf::Event::Closed:
                deleteAllocs();
                exit(0);
                break;
            case sf::Event::Resized:
                keepAspectRatioFinalWindow(winWindow, windowSizeX, windowSizeY);
                break;
            case sf::Event::KeyPressed:
                if(e.key.code == sf::Keyboard::R)               
                    return true;               
            case sf::Event::MouseButtonPressed:
                if(resetButton.isMouseOver(winWindow))
                    resetButton.button.setTextureRect({50, 0, 50, 50});
                break;
            case sf::Event::MouseButtonReleased:
                if(resetButton.isMouseOver(winWindow))                                  
                    flag = 1;                    
                resetButton.button.setTextureRect({0, 0, 50, 50});
                break;
            }           
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(50));

        winWindow.clear();
        
        winWindow.draw(backGround);
        bool x = 1;
        for(int i = 0; i < 9; i++)
        {
            if( x && endSegments[i].button.getTextureRect().left == 0)             
                continue;            
            winWindow.draw(endSegments[i].button);
            x = 0;
        }
        if(endSegments[1].button.getTextureRect().left > 0) winWindow.draw(dots1);
        if(endSegments[5].button.getTextureRect().left > 0) winWindow.draw(dots2);
        winWindow.draw(resetButton.button);

        winWindow.display();
        if(flag == 1)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
            return true;
        }
    }
}

inline const char* titleBarSelection()
{
    if(diff.whichDiff == beginner) return "textures/titlebar.png";
    if(diff.whichDiff == easy) return "textures/titlebarEASY.png";
    if(diff.whichDiff == medium) return "textures/titlebarMEDIUM.png";
    return "textures/titlebarHARD.png";
}

uint8_t tileSelection(const short& index)                              
{
    if(g_mineFieldPtr[index].flaged) return clickedFlag;   
    if(g_mineFieldPtr[index].bomb) return clickedBomb;

    return validTile;   
}

inline void updateScreen(sf::RenderWindow& msWindow, sf::Sprite& titleBar, sf::Sprite& resetButton)
{
    msWindow.clear();

    for (int i = 0; i < diff.playSize; ++i)
        msWindow.draw(g_tilePtr[i]->button); 

    msWindow.draw(titleBar);

    for (int i = 0; i < diff.numOfSegments; ++i)
        msWindow.draw(g_segmentsPtr[i]->button);

    msWindow.draw(resetButton);

    msWindow.display();           
}

Difficulty createDifficultyWindow()
{
    const uint32_t windowSizeX = 300u, windowSizeY = 300u;
    sf::RenderWindow diffSelectionWindow({windowSizeX, windowSizeY}, "choose difficulty");
    diffSelectionWindow.setFramerateLimit(20);
    
    TextStuff beginnerT("beginner", {150.0f, 24.0f}, "fonts/FRSCRIPT.TTF", 24, sf::Color::Transparent);
    TextStuff easyT("easy", {150.0f, 16.0f}, "fonts/FRSCRIPT.TTF", 31, sf::Color::Transparent);
    TextStuff mediumT("medium", {150.0f, 20.0f},  "fonts/FRSCRIPT.TTF", 26, sf::Color::Transparent);
    TextStuff hardT("hard", {150.0f, 20.0f},  "fonts/FRSCRIPT.TTF", 27, sf::Color::Transparent);

    beginnerT.t.setOrigin({ beginnerT.t.getLocalBounds().width / 2, beginnerT.t.getLocalBounds().height / 2 });
    easyT.t.setOrigin({ easyT.t.getLocalBounds().width / 2, easyT.t.getLocalBounds().height / 2 });
    mediumT.t.setOrigin({ mediumT.t.getLocalBounds().width / 2, mediumT.t.getLocalBounds().height / 2 });
    hardT.t.setOrigin({ hardT.t.getLocalBounds().width / 2, hardT.t.getLocalBounds().height / 2 });

    sf::Texture bGroundT;
    bGroundT.loadFromFile("textures/backGround.png");
    sf::Sprite backGround(bGroundT);

    sf::Texture t;
    t.loadFromFile("textures/diffSelectionButtons.png");
    Button beginnerButton(t, 0,   { 6,  10.0f}, 5, 1);   
    Button easyButton(t,     2,   {66.0f,  10.0f}, 5, 1);  
    Button mediumButton(t,   3,   { 184.0f, 10.0f}, 5, 1);
    Button hardButton(t,     4,   {244.0f, 10.0f}, 5, 1);
    
    
    while (diffSelectionWindow.isOpen())
    {           
        sf::Event evnt;
        updateDiffScreen(diffSelectionWindow, backGround, beginnerButton.button, easyButton.button, mediumButton.button,
                              hardButton.button, beginnerT, easyT, mediumT, hardT); 
        while (diffSelectionWindow.pollEvent(evnt))
        {  
            switch (evnt.type)
            {              
            case sf::Event::EventType::Closed:
                std::exit(0);
                break;
            case sf::Event::Resized:
                keepAspectRatioDiffSelection(diffSelectionWindow, windowSizeX, windowSizeY);
                break;
            case sf::Event::MouseButtonPressed:
                if( beginnerButton.isMouseOver(diffSelectionWindow) )
                    beginnerButton.button.setTextureRect({50, 0, 50, 50});
           
                else if( easyButton.isMouseOver(diffSelectionWindow) )      
                    easyButton.button.setTextureRect({50, 0, 50, 50});

                else if( mediumButton.isMouseOver(diffSelectionWindow) )             
                    mediumButton.button.setTextureRect({50, 0, 50, 50});
   
                else if( hardButton.isMouseOver(diffSelectionWindow) ) 
                    hardButton.button.setTextureRect({50, 0, 50, 50});

                break;
            case sf::Event::MouseButtonReleased: 

                beginnerButton.button.setTextureRect({0, 0, 50, 50});
                easyButton.button.setTextureRect({100, 0, 50, 50});
                mediumButton.button.setTextureRect({150, 0, 50, 50});
                hardButton.button.setTextureRect({200, 0, 50, 50});

                updateDiffScreen(diffSelectionWindow, backGround, beginnerButton.button, easyButton.button, mediumButton.button,
                              hardButton.button, beginnerT, easyT, mediumT, hardT); 
                std::this_thread::sleep_for(std::chrono::milliseconds(150));

                if( beginnerButton.isMouseOver(diffSelectionWindow) )
                {                      
                    Difficulty d(beginner);
                    return d;                  
                }               
                if( easyButton.isMouseOver(diffSelectionWindow) )
                {                   
                    Difficulty d(easy);
                    return d;
                }
                if( mediumButton.isMouseOver(diffSelectionWindow) ) 
                {                        
                    Difficulty d(medium);
                    return d;
                }
                if( hardButton.isMouseOver(diffSelectionWindow) ) 
                {                   
                    Difficulty d(hard);
                    return d;
                }
               break;
           
           case sf::Event::MouseMoved:

                if( beginnerButton.isMouseOver(diffSelectionWindow) )  
                {   
                    beginnerT.setTextColor(sf::Color::Blue);                  
                } else beginnerT.setTextColor(sf::Color::Transparent);

                if( easyButton.isMouseOver(diffSelectionWindow) )
                {          
                     easyT.setTextColor(sf::Color::Blue);                     
                } else easyT.setTextColor(sf::Color::Transparent);

                if( mediumButton.isMouseOver(diffSelectionWindow) )
                {
                    mediumT.setTextColor(sf::Color::Blue);
                }else mediumT.setTextColor(sf::Color::Transparent);
                   
                if( hardButton.isMouseOver(diffSelectionWindow) )
                {
                    hardT.setTextColor(sf::Color::Blue);
                }else hardT.setTextColor(sf::Color::Transparent);                  
              break;             
            }
        }      
    }
}

inline void updateDiffScreen(sf::RenderWindow& diffSelectionWindow, 
                             sf::Sprite& backGround,
                             sf::Sprite& beginnerButton,
                             sf::Sprite& ezButton,
                             sf::Sprite& mediumButton,
                             sf::Sprite& hardButton,                             
                             TextStuff& beginnerText,
                             TextStuff& easyText,
                             TextStuff& mediumText,
                             TextStuff& hardText)
{
        diffSelectionWindow.clear();  
       
        diffSelectionWindow.draw(backGround);

        diffSelectionWindow.draw(beginnerText.t);
        diffSelectionWindow.draw(easyText.t);
        diffSelectionWindow.draw(mediumText.t);
        diffSelectionWindow.draw(hardText.t);

        diffSelectionWindow.draw(ezButton);
        diffSelectionWindow.draw(beginnerButton);
        diffSelectionWindow.draw(mediumButton);
        diffSelectionWindow.draw(hardButton);

        diffSelectionWindow.display();           
}