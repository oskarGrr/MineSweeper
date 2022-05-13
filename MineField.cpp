#include "MineField.h"
#include "Difficulty.h"
#include "declarations.h"
#include "button.h"

void MineField::initMineField()
{
    unsigned short x, y;
    for (int i = 0; i < g_diff.playSize; ++i)
    {
        if (g_mineFieldPtr[i].bomb) continue;
        
        x = i % g_diff.width;
        y = i / g_diff.width;
        
        g_mineFieldPtr[i].bombcount =
        (int)isABomb(x + 1, y) +      // right
        (int)isABomb(x + 1, y - 1) +  // right and up
        (int)isABomb(x, y - 1) +      // up
        (int)isABomb(x - 1, y - 1) +  // up and left
        (int)isABomb(x - 1, y) +      // left
        (int)isABomb(x - 1, y + 1) +  // left and down
        (int)isABomb(x, y + 1) +      // down
        (int)isABomb(x + 1, y + 1);   // down and right
    }
}

void MineField::setRngBombLocations(const short& index)
{      
    short rngLocation; 
    for (int i = 0; i < g_diff.numOfBombs; ++i)
    {       
        rngLocation = rand() % g_diff.playSize;
        if (g_mineFieldPtr[rngLocation].bomb == false && rngLocation != index)
        {
            g_mineFieldPtr[rngLocation].bomb = true;
        }                
        else --i;
    }
}

void MineField::fill(const short& x, const short& y)
{//recursive floodFill function
    if( x < 0 || x >= g_diff.width ) return;

    if( y < 0 || y >= g_diff.height ) return;

    if( g_mineFieldPtr[x + y * g_diff.width].covered == false ) return;

    if( g_mineFieldPtr[x + y * g_diff.width].bombcount > 0 )
    {
        g_mineFieldPtr[x + y * g_diff.width].covered = false;
        tileTextures type = (tileTextures)g_mineFieldPtr[x + y * g_diff.width].bombcount;
        g_tilePtr[x + y * g_diff.width]->changeTileTexture(type);
        return;
    }

    if( g_mineFieldPtr[x + y * g_diff.width].bombcount == 0 )
    {
        g_mineFieldPtr[x + y * g_diff.width].covered = false;
        g_tilePtr[x + y * g_diff.width]->changeTileTexture(uncovered);
        
        fill(x + 1, y);             //right
        fill(x + 1, y - 1);        //top right
        fill(x, y - 1);           //top uwu
        fill(x - 1, y - 1);      //top left
        fill(x - 1, y);         //left
        fill(x - 1, y + 1);    //bottom left
        fill(x, y + 1);       //bottom uwu
        fill(x + 1, y + 1);  //bottom right        
    }
}

bool MineField::winCondition()
{
    unsigned short win = 0;

    for (int i = 0; i < g_diff.playSize; i++) 
        if (g_mineFieldPtr[i].covered == false) 
            win++;
        
    if (win == g_diff.playSize - g_diff.numOfBombs) return true;
 
    return false;   
}

//returns true if given x and y coordinates is a bomb
bool MineField::isABomb(const short& x, const short& y) const
{//returns true if bomb is at that x,y
    short index = x + y * g_diff.width;
    if (y < 0 || y >= g_diff.height) return false;
    if (x < 0 || x >= g_diff.width) return false;
        if(g_mineFieldPtr[index].bomb) 
            return true;
    return false;
}

void MineField::setFlagHere(const Vec2i_mi& location)
{       
    if( g_mineFieldPtr[location.index].flaged )
    {
        g_mineFieldPtr[location.index].flaged = false;
        Button::incrementFlagDisplay(2);        
        g_tilePtr[location.index]->changeTileTexture(hidden);
    }
    else if (g_mineFieldPtr[location.index].covered)
    {
        g_mineFieldPtr[location.index].flaged = true;
        Button::decrementFlagDisplay(2);
        g_tilePtr[location.index]->changeTileTexture(flag);
    }
}

void MineField::revealBombs(Vec2i_mi& location)
{//used to reveal bombs on a loss
    for(int i = 0; i < g_diff.playSize; ++i)
    {
        if(g_mineFieldPtr[i].bomb)
        {
            if(g_mineFieldPtr[i].flaged) continue;
            g_tilePtr[i]->changeTileTexture(mine);            
        }                  
        else if(g_mineFieldPtr[i].flaged)      
            g_tilePtr[i]->changeTileTexture(bombCrossed);//if not a bomb and also a flag then bomb with red cross            
    }
    g_tilePtr[location.index]->changeTileTexture(redBomb); //location is the spot the bomb was clicked
}

void MineField::placeRemainingFlags()
{//places last flags after all the squares that arent bombs have been uncovered
    int c = 0;
    for(int i = 0; i < g_diff.playSize; ++i)
        if(g_mineFieldPtr[i].covered)
        {
            c++;
            g_tilePtr[i]->button.setTextureRect({32, 0, INITIAL_TILE_SIZE, INITIAL_TILE_SIZE});
        }

    for(int i = 0; i < c; i++)
        Button::decrementFlagDisplay(2);
}