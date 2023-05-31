#pragma once
#include <cstdint>
#include <vector>
#include "SmileyButton.h"
#include "Tile.h"

class  Difficulty;
struct LeftClickInfo;
class  FlagCounter;

class MineField
{
public:

    //title bar height needed in order to know where 
    //to place the tiles on the screen
    MineField(float titleBarHeight);
    ~MineField()=default;

    MineField(MineField const&)=delete;
    MineField(MineField&&)=delete;
    MineField& operator=(MineField const&)=delete;
    MineField& operator=(MineField&&)= delete;

    //rests tiles to default tile values
    void resetTiles();

    auto const& getTiles() const {return m_tiles;}
    void floodFill(sf::Vector2i const& coords);
    bool isFlagged(sf::Vector2i const& coords) const;
    bool isABomb(sf::Vector2i const& coords) const;
    bool isCovered(sf::Vector2i const& coords) const;
    void revealBombs(sf::Vector2i const& clickedBombLocation);
    void calcAndSetBombCounts();//calculate and init all the tiles' m_bombCount after setRngBombLocations() is called
    void setRngBombLocations(sf::Vector2i const& firstTileClickCoords);
    void setFlagHere(sf::Vector2i const& location, FlagCounter& fc);
    void setTilePositions(float titleBarHeight);//puts the tiles in the right place on the screen depending on the feild size

    //if the user won but didnt place all the flags, this finishes 
    //placing all the flags on the uncovered bombs for them
    void placeRemainingFlags(FlagCounter& flagCounter);

private:
    static bool areTileCoordsInRange(sf::Vector2i const& coords);

    std::vector<Tile> m_tiles;
};