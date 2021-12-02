#include "Button.h"
#include "declarations.h"
#include "Difficulty.h"
#include "MineField.h"

Button::Button() = default;

Button::Button(sf::Texture& t)  // this constructor
{                              // is for uncovered tiles
    button.setTexture(t);                               
    button.setTextureRect({0, 0, TILE_SIZE, TILE_SIZE});
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

void Button::changeTileTexture(const uint8_t& tileTextureType)
{   // need to put this in a loop and when i = tileTextureType change texturerect and return
    // (coords will increment by 16 every loop)
    //i might changes this to a loop later...
    if(tileTextureType == uncovered)        button.setTextureRect ({16, 0, TILE_SIZE, TILE_SIZE});
    else if(tileTextureType == one)         button.setTextureRect ({0,  16, TILE_SIZE, TILE_SIZE});
    else if(tileTextureType == two)         button.setTextureRect ({16, 16, TILE_SIZE, TILE_SIZE});
    else if(tileTextureType == three)       button.setTextureRect ({32, 16, TILE_SIZE, TILE_SIZE});
    else if(tileTextureType == four)        button.setTextureRect ({48, 16, TILE_SIZE, TILE_SIZE});
    else if(tileTextureType == five)        button.setTextureRect ({64, 16, TILE_SIZE, TILE_SIZE});
    else if(tileTextureType == six)         button.setTextureRect ({80, 16, TILE_SIZE, TILE_SIZE});
    else if(tileTextureType == seven)       button.setTextureRect ({96, 16, TILE_SIZE, TILE_SIZE});
    else if(tileTextureType == eight)       button.setTextureRect ({112,16, TILE_SIZE, TILE_SIZE});
    else if(tileTextureType == mine)        button.setTextureRect ({80, 0, TILE_SIZE, TILE_SIZE});
    else if(tileTextureType == redBomb)     button.setTextureRect ({96, 0, TILE_SIZE, TILE_SIZE});
    else if(tileTextureType == bombCrossed) button.setTextureRect ({112,0, TILE_SIZE, TILE_SIZE});
    else if(tileTextureType == flag)        button.setTextureRect ({32, 0, TILE_SIZE, TILE_SIZE});
    else if(tileTextureType == hidden)      button.setTextureRect ({0, 0, TILE_SIZE, TILE_SIZE});
}

bool Button::isMouseOver(sf::RenderWindow& win) //only used on diff selection windown and reset buttons
{//returns true if the mouse is over a Button
    int mosX = sf::Mouse::getPosition(win).x;
    int mosY = sf::Mouse::getPosition(win).y;

    float btnPosX = button.getPosition().x;
    float btnPosY = button.getPosition().y;

    float btnWidth = button.getLocalBounds().width;
    float btnHeight = button.getLocalBounds().height;

    if (
        mosX >= btnPosX &&
        mosY >= btnPosY &&
        mosX <= (btnPosX + btnWidth) &&
        mosY <= (btnPosY + btnHeight)
        )
        return true;
    else return false;
}

void Button::createTiles(sf::Texture& t)
{
    //create enough tiles for the selected size of the minefield
    g_tilePtr = new Button* [diff.playSize];
    for (int i = 0; i < diff.playSize; ++i)
    {
        *(g_tilePtr + i) = new Button(t);  
    }                               

    //set the position for the tiles (has to start and a lower Y coord to accomodate for the title bar)
                                                                           //i doesnt start at zero so the y coords
    for (int i = diff.width * 2; i < diff.playSize + diff.width * 2; ++i)//will be pushed down to make room for titlebar
    {
        g_tilePtr[i - diff.width * 2]->button.setPosition({(i % diff.width) * TILE_SIZE_F, ((i / diff.width) * TILE_SIZE_F)}); 
    }
}

void Button::createSegments(sf::Texture& t)
{
    int tSizex = t.getSize().x / 12;
    int tSizey = t.getSize().y;
    float xPos = 6.0f; // first segment display always starts at 6 x coord
    float xOffsetFromRight = 0;

    if(diff.whichDiff == beginner)
    {
        g_segmentsPtr = new Button*[6];       
        diff.numOfSegments = 6;
        xOffsetFromRight = (float)diff.windowSize_X - 45;
    }//num of segment displays is only more on diffs higher than beginner...
    else // on higher diffs you can see there are 8 total segment displays
    {
        g_segmentsPtr = new Button*[8];
        diff.numOfSegments = 8;
        xOffsetFromRight = (float)diff.windowSize_X - 71; 
    }
    for(int i = 0; i < diff.numOfSegments; ++i)
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

    if(diff.whichDiff == beginner)
    {
        whichSegment == 1 ? (textureXstart = 1) : (textureXstart = 0);
        g_segmentsPtr[whichSegment]->button.setTextureRect({13 * textureXstart, 0, 13, 23});
    }
    else if(diff.whichDiff == easy)
    {
        whichSegment == 1 ? textureXstart = 4 : textureXstart = 0;
        g_segmentsPtr[whichSegment]->button.setTextureRect({13 * textureXstart, 0, 13, 23}); 
    }
    else if(diff.whichDiff == medium)
    {
        if(whichSegment == 1) textureXstart = 8;
        else if (whichSegment == 2) textureXstart = 5;
        g_segmentsPtr[whichSegment]->button.setTextureRect({13 * textureXstart, 0, 13, 23});
    } 
    else if(diff.whichDiff == hard)
    {
        if(whichSegment == 0) textureXstart = 2;
        else if(whichSegment == 1) textureXstart = 3;
        else textureXstart = 0;
        g_segmentsPtr[whichSegment]->button.setTextureRect({13 * textureXstart, 0, 13, 23});
    }  
}      

void Button::decrementFlagDisplay(const int& index)
{     
    if(diff.flagCount <= 0) 
    {
        diff.flagCount--;
        return;
    }
    
    int currentSegmentNumber = g_segmentsPtr[index]->button.getTextureRect().left / 13;
    currentSegmentNumber--;

    if(currentSegmentNumber < 0)
    {
        diff.flagCount++;
        currentSegmentNumber = 9;
        decrementFlagDisplay(index - 1);
    }
    g_segmentsPtr[index]->button.setTextureRect({13 * currentSegmentNumber, 0, 13, 23});
    diff.flagCount--;
}

void Button::incrementFlagDisplay(const int& index)                                    
{ 
    if(diff.flagCount < 0)
    {
        diff.flagCount++;
        return;
    }

    int currentSegmentNumber = g_segmentsPtr[index]->button.getTextureRect().left / 13;
    currentSegmentNumber++;

    if(currentSegmentNumber > 9)
    {
        diff.flagCount--;
        currentSegmentNumber = 0;
        incrementFlagDisplay(index - 1);
    }
    g_segmentsPtr[index]->button.setTextureRect({13 * currentSegmentNumber, 0, 13, 23});
    diff.flagCount++;
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
    if(diff.whichDiff == beginner)
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
    diff.flagCount = diff.numOfBombs;
    for(int i = 0; i < 3; ++i)    
        initFlagCounter(i);            
}

void Button::resetButtonLeftClick(uint8_t& gameState, sf::RenderWindow& msWindow)
{
    if( ! this->isMouseOver(msWindow) && gameState != loss)
    {
        this->button.setTextureRect({48, 0, 24, 24});
    }
    else if(this->isMouseOver(msWindow))
    {
       this->button.setTextureRect({24, 0, 24, 24});
    }
}