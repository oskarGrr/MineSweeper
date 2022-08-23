#include "Button.h"
#include "declarations.h"
#include "Difficulty.h"
#include "MineField.h"

Button::Button() = default;

Button::Button(sf::Texture& t)  // this constructor
{                              // is for uncovered tiles
    button.setTexture(t);                               
    button.setTextureRect({0, 0, INITIAL_TILE_SIZE, INITIAL_TILE_SIZE});
}

Button::Button(const sf::Texture& t, const int& xStart, 
               const sf::Vector2f& position, const short& xdiv, const short& ydiv) 
{
    button.setPosition(position);

    int tSize_x = t.getSize().x / xdiv;
    int tSize_y = t.getSize().y / ydiv;
    
    button.setTexture(t);
    button.setTextureRect({tSize_x * xStart, 0, tSize_x, tSize_y});
}

void Button::changeTileTexture(tileTextures tileTextureType)
{  
    switch(tileTextureType)
    {
    case uncovered:   button.setTextureRect({16, 0,  INITIAL_TILE_SIZE, INITIAL_TILE_SIZE}); break;
    case one:         button.setTextureRect({0,  16, INITIAL_TILE_SIZE, INITIAL_TILE_SIZE}); break;
    case two:         button.setTextureRect({16, 16, INITIAL_TILE_SIZE, INITIAL_TILE_SIZE}); break;
    case three:       button.setTextureRect({32, 16, INITIAL_TILE_SIZE, INITIAL_TILE_SIZE}); break;
    case four:        button.setTextureRect({48, 16, INITIAL_TILE_SIZE, INITIAL_TILE_SIZE}); break;
    case five:        button.setTextureRect({64, 16, INITIAL_TILE_SIZE, INITIAL_TILE_SIZE}); break;
    case six:         button.setTextureRect({80, 16, INITIAL_TILE_SIZE, INITIAL_TILE_SIZE}); break;
    case seven:       button.setTextureRect({96, 16, INITIAL_TILE_SIZE, INITIAL_TILE_SIZE}); break;
    case eight:       button.setTextureRect({112,16, INITIAL_TILE_SIZE, INITIAL_TILE_SIZE}); break;
    case mine:        button.setTextureRect({80, 0,  INITIAL_TILE_SIZE, INITIAL_TILE_SIZE}); break;
    case redBomb:     button.setTextureRect({96, 0,  INITIAL_TILE_SIZE, INITIAL_TILE_SIZE}); break;
    case bombCrossed: button.setTextureRect({112,0,  INITIAL_TILE_SIZE, INITIAL_TILE_SIZE}); break;
    case flag:        button.setTextureRect({32, 0,  INITIAL_TILE_SIZE, INITIAL_TILE_SIZE}); break;
    case hidden:      button.setTextureRect({ 0, 0,  INITIAL_TILE_SIZE, INITIAL_TILE_SIZE}); break;    
    }
}

bool Button::isMouseOver(sf::RenderWindow& win) //only used on diff selection windown and reset buttons
{//returns true if the mouse is over a Button

    float btnPosX = this->button.getPosition().x;
    float btnPosY = this->button.getPosition().y;
    float btnWidth  = this->button.getLocalBounds().width;
    float btnHeight = this->button.getLocalBounds().height;

    sf::Vector2f coords = win.mapPixelToCoords(sf::Mouse::getPosition(win));

    return (
            coords.x >= btnPosX &&
            coords.y >= btnPosY &&
            coords.x <= (btnPosX + btnWidth) &&
            coords.y <= (btnPosY + btnHeight)
           );
}

void Button::createTiles(sf::Texture& t)
{
    //create enough tiles for the selected size of the minefield
    g_tilePtr = new Button* [g_diff.playSize];
    for (int i = 0; i < g_diff.playSize; ++i)
    {
        *(g_tilePtr + i) = new Button(t);  
    }                               

    //set the position for the tiles (has to start and a lower Y coord to accomodate for the title bar)
                                                                           //i doesnt start at zero so the y coords
    for (int i = g_diff.width * 2; i < g_diff.playSize + g_diff.width * 2; ++i)//will be pushed down to make room for titlebar
    {
        sf::Vector2f pos;
        pos.x = (i % g_diff.width) * INITIAL_TILE_SIZE_F;
        pos.y = (i / g_diff.width) * INITIAL_TILE_SIZE_F;
        g_tilePtr[i - g_diff.width * 2]->button.setPosition(pos);
    }
}

void Button::createSegments(sf::Texture& t)
{
    int tSizex = t.getSize().x / 12;
    int tSizey = t.getSize().y;
    float xPos = 6.0f; // first segment display always starts at 6 x coord
    float xOffsetFromRight = 0;

    if(g_diff.whichDiff == beginner)
    {
        g_segmentsPtr = new Button*[6];       
        g_diff.numOfSegments = 6;
        xOffsetFromRight = (float)g_diff.initialWindowSize_X - 45;
    }//num of segment displays is only more on diffs higher than beginner...
    else // on higher diffs you can see there are 8 total segment displays
    {
        g_segmentsPtr = new Button*[8];
        g_diff.numOfSegments = 8;
        xOffsetFromRight = (float)g_diff.initialWindowSize_X - 71; 
    }
    for(int i = 0; i < g_diff.numOfSegments; ++i)
    {
        *(g_segmentsPtr + i) = new Button();
        g_segmentsPtr[i]->button.setTexture(t);

        if(i < 3) initFlagCounter(i);//the first three segments textures will be init to flag count
        else g_segmentsPtr[i]->button.setTextureRect({0, 0, 13, 23});//if i < 3 these segments will represent the clock
                                                                    //so they start at zero texture
        g_segmentsPtr[i]->button.setPosition({xPos, 4.0f});

        if(i == 2) xPos = xOffsetFromRight;//after placing the third segment display the x will 
        else xPos += 13.0f;               //jump to where the first clock segment diplay will be
    }
}

void Button::initFlagCounter(const short& whichSegment)
{
    int textureXstart = 0;

    if(g_diff.whichDiff == beginner)
    {
        whichSegment == 1 ? (textureXstart = 1) : (textureXstart = 0);
        g_segmentsPtr[whichSegment]->button.setTextureRect({13 * textureXstart, 0, 13, 23});
    }
    else if(g_diff.whichDiff == easy)
    {
        whichSegment == 1 ? textureXstart = 4 : textureXstart = 0;
        g_segmentsPtr[whichSegment]->button.setTextureRect({13 * textureXstart, 0, 13, 23}); 
    }
    else if(g_diff.whichDiff == medium)
    {
        if(whichSegment == 1) textureXstart = 8;
        else if (whichSegment == 2) textureXstart = 5;
        g_segmentsPtr[whichSegment]->button.setTextureRect({13 * textureXstart, 0, 13, 23});
    } 
    else if(g_diff.whichDiff == hard)
    {
        if(whichSegment == 0) textureXstart = 2;
        else if(whichSegment == 1) textureXstart = 3;
        else textureXstart = 0;
        g_segmentsPtr[whichSegment]->button.setTextureRect({13 * textureXstart, 0, 13, 23});
    }  
}      

void Button::decrementFlagDisplay(const int& index)
{     
    if(g_diff.flagCount <= 0) 
    {
        g_diff.flagCount--;
        return;
    }
    
    int currentSegmentNumber = g_segmentsPtr[index]->button.getTextureRect().left / 13;
    currentSegmentNumber--;

    if(currentSegmentNumber < 0)
    {
        g_diff.flagCount++;
        currentSegmentNumber = 9;
        decrementFlagDisplay(index - 1);
    }
    g_segmentsPtr[index]->button.setTextureRect({13 * currentSegmentNumber, 0, 13, 23});
    g_diff.flagCount--;
}

void Button::incrementFlagDisplay(const int& index)                                    
{ 
    if(g_diff.flagCount < 0)
    {
        g_diff.flagCount++;
        return;
    }

    int currentSegmentNumber = g_segmentsPtr[index]->button.getTextureRect().left / 13;
    currentSegmentNumber++;

    if(currentSegmentNumber > 9)
    {
        g_diff.flagCount--;
        currentSegmentNumber = 0;
        incrementFlagDisplay(index - 1);
    }
    g_segmentsPtr[index]->button.setTextureRect({13 * currentSegmentNumber, 0, 13, 23});
    g_diff.flagCount++;
    return;   
}

void Button::incrementClockDisplay(const short& index)
{
    if(index < 3) return;
    
    int currentClockDisplayNum = g_segmentsPtr[index]->button.getTextureRect().left / 13;  
    currentClockDisplayNum++;

    if(currentClockDisplayNum > 9)
    {
        currentClockDisplayNum = 0;
        incrementClockDisplay(index - 1);
    }
    g_segmentsPtr[index]->button.setTextureRect({13 * currentClockDisplayNum, 0, 13, 23});
}

void Button::refreshClockSegments()
{
    if(g_diff.whichDiff == beginner)
    {
        for(int i = 3; i < 6; i++)
        {
            g_segmentsPtr[i]->button.setTextureRect({0, 0, 13, 23});
        }
        return;
    }  
    for(int i = 3; i < 8; i++)
    {
        g_segmentsPtr[i]->button.setTextureRect({0, 0, 13, 23});
    }      
}

void Button::refreshFlagSegments()
{
    g_diff.flagCount = g_diff.numOfBombs;
    for(int i = 0; i < 3; ++i)    
        initFlagCounter(i);            
}

void Button::resetButtonLeftClick(uint8_t& gameState, sf::RenderWindow& msWindow)
{
    if(!this->isMouseOver(msWindow) && gameState != loss)
    {
        this->button.setTextureRect({48, 0, 24, 24});
    }
    else if(this->isMouseOver(msWindow))
    {
       this->button.setTextureRect({24, 0, 24, 24});
    }
}