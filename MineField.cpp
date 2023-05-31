#include <cassert>
#include "MineField.h"
#include "Difficulty.h"
#include "MineSweeperApp.h"
#include "FlagCounter.h"
#include "textureWrapper.h"

MineField::MineField(float titleBarHeight) : m_tiles{Difficulty::get().getFieldNumOfTiles()}
{
    auto& texture = TextureWrapper::getTexture(TextureIndicies::TILES);
    for(auto& tile : m_tiles)
    {
        tile.m_sprite.setTexture(texture);
        tile.changeTile(Tile::TileTypes::COVERED);
    }

    setTilePositions(titleBarHeight);
}

//calculate and init all the tiles' m_bombCount after setRngBombLocations() is called
void MineField::calcAndSetBombCounts()
{
    auto& diff = Difficulty::get();
    auto numOfTiles = diff.getFieldNumOfTiles(), tileWidth = diff.getFieldWidth();
    int x = 0, y = 0;
    for(int i = 0; i < numOfTiles; ++i)
    {
        if (m_tiles[i].m_isBomb) continue;
        
        x = i % tileWidth;
        y = i / tileWidth;
        
        m_tiles[i].m_numSurroundingBombs =
        isABomb({x + 1, y})     + //right
        isABomb({x + 1, y - 1}) + //top right
        isABomb({x, y - 1})     + //top
        isABomb({x - 1, y - 1}) + //top left
        isABomb({x - 1, y})     + //left
        isABomb({x - 1, y + 1}) + //bottom left
        isABomb({x, y + 1})     + //bottom
        isABomb({x + 1, y + 1});  //bottom right
    }
}

//puts the tiles in the right place on the screen depending on the feild size
void MineField::setTilePositions(float titleBarHeight)
{
    auto const& diff = Difficulty::get();
    auto tileWidth = diff.getFieldWidth();
    const auto tileCount = m_tiles.size();
    for(int i = 0; i < tileCount; ++i)
    {
        sf::Vector2f newPosition
        {
            static_cast<float>(i % tileWidth * INITIAL_TILE_SIZE),
            (i / tileWidth * INITIAL_TILE_SIZE) + titleBarHeight
        };

        m_tiles[i].setTilePosition(newPosition);
    }
}

bool MineField::areTileCoordsInRange(sf::Vector2i const & coords)
{
    auto const& diff = Difficulty::get();
    auto mineFieldWidth = diff.getFieldWidth();
    auto mineFieldHeight = diff.getFieldHeight();

    return coords.x < mineFieldWidth  && coords.x >= 0 &&
           coords.y < mineFieldHeight && coords.y >= 0;
}

void MineField::setRngBombLocations(sf::Vector2i const& firstTileClickCoords)
{
    int index = MineSweeperApp::coordsToIndex(firstTileClickCoords);
    int rngLocation;
    auto& diff = Difficulty::get();
    auto numOfBombs = diff.getBombCount();
    for(int i = 0; i < numOfBombs; ++i)
    {
        rngLocation = rand() % diff.getFieldNumOfTiles();
        if(!m_tiles[rngLocation].m_isBomb && rngLocation != index)
        {
            m_tiles[rngLocation].m_isBomb = true;
        }
        else --i;
    }
}

//put a flag down on the field. does not update the flag counter.
//if there is already a flag at location then removes the flag
void MineField::setFlagHere(sf::Vector2i const& location, FlagCounter& fc)
{
    auto index = MineSweeperApp::coordsToIndex(location);
    if(m_tiles[index].m_isFlagged)
    {
        m_tiles[index].m_isFlagged = false;
        m_tiles[index].changeTile(Tile::TileTypes::COVERED);
        ++fc;
    }
    else if(m_tiles[index].m_isCovered)
    {
        m_tiles[index].m_isFlagged = true;
        m_tiles[index].changeTile(Tile::TileTypes::FLAG);
        --fc;
    }
}

void MineField::floodFill(sf::Vector2i const& coords)
{
    auto const tileHeight = Difficulty::get().getFieldHeight();
    auto const tileWidth = Difficulty::get().getFieldWidth();
    auto const index = MineSweeperApp::coordsToIndex(coords);

    //base cases
    if(coords.x < 0 || coords.x >= tileWidth) return;
    if(coords.y < 0 || coords.y >= tileHeight) return;
    if(!m_tiles[index].m_isCovered) return;
    if(m_tiles[index].m_numSurroundingBombs > 0)
    {
        m_tiles[index].m_isCovered = false;
        m_tiles[index].changeTileBasedOnBombCount();
        return;
    }

    if(m_tiles[index].m_numSurroundingBombs == 0)
    {
        m_tiles[index].m_isCovered = false;
        //g_tilePtr[x + y * g_diff.width]->changeTileTexture(UNCOVERED);
        m_tiles[index].changeTile(Tile::TileTypes::UNCOVERED);
        
        floodFill({coords.x + 1, coords.y    });  //right
        floodFill({coords.x + 1, coords.y - 1});  //top right
        floodFill({coords.x,     coords.y - 1});  //top 
        floodFill({coords.x - 1, coords.y - 1});  //top left
        floodFill({coords.x - 1, coords.y    });  //left
        floodFill({coords.x - 1, coords.y + 1});  //bottom left
        floodFill({coords.x,     coords.y + 1});  //bottom 
        floodFill({coords.x + 1, coords.y + 1});  //bottom right
    }
}

bool MineField::isFlagged(sf::Vector2i const& coords) const
{
    if(!areTileCoordsInRange(coords)) return false;
    return m_tiles[MineSweeperApp::coordsToIndex(coords)].m_isFlagged;
}

//returns true if given x and y coordinates is a bomb and
//false if that tile isnt a bomb or the coords are out of range
bool MineField::isABomb(sf::Vector2i const& coords) const
{
    if(!areTileCoordsInRange(coords)) return false;
    return m_tiles[MineSweeperApp::coordsToIndex(coords)].m_isBomb;
}

bool MineField::isCovered(sf::Vector2i const& coords) const
{
    assert(areTileCoordsInRange(coords));
    return m_tiles[MineSweeperApp::coordsToIndex(coords)].isCovered();
}

//used to reveal all the bombs when the player clicks on a bomb
void MineField::revealBombs(sf::Vector2i const& clickedBombLocation)
{
    for(auto& tile : m_tiles)
    {
        if(tile.m_isBomb)
        {
            if(tile.m_isFlagged) continue;
            tile.changeTile(Tile::TileTypes::BOMB);
        }
        else if(tile.m_isFlagged)
        {
            tile.changeTile(Tile::TileTypes::BOMB_CROSSED);
        }
    }

    auto index = MineSweeperApp::coordsToIndex(clickedBombLocation);
    m_tiles[index].changeTile(Tile::TileTypes::RED_BOMB);
}

//if the user won but didnt place all the flags on the bombs,  
//this finishes placing all the flags on the bombs for them
void MineField::placeRemainingFlags(FlagCounter& flagCounter)
{
    for(auto& tile : m_tiles)
    {
        //if the user forgot to set a flag on a bomb before winning
        if(tile.m_isBomb && !tile.m_isFlagged)
            tile.changeTile(Tile::TileTypes::FLAG);
    }

    //now that all the flags have been placed the flag counter should be zero
    flagCounter.zeroOutCounter();
}

//reset field to default tiles.
void MineField::resetTiles()
{
    for(auto& tile : m_tiles)
    {
        tile.resetTile();
        tile.changeTile(Tile::TileTypes::COVERED);
    }
}