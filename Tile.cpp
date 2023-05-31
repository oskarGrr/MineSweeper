#include <SFML/Graphics.hpp>
#include "textureWrapper.h"
#include "Tile.h"

Tile::Tile()
{
    m_sprite.setTexture(TextureWrapper::getTexture(TextureIndicies::TILES));
}

//switches the texture rectangle to the correct based on the bomb count of this tile
void Tile::changeTileBasedOnBombCount()
{
    changeTile(static_cast<TileTypes>(m_numSurroundingBombs + 5));
}

void Tile::changeTile(TileTypes tileType)
{
    m_sprite.setTextureRect({static_cast<int>(tileType) * INITIAL_TILE_SIZE, 0,
        INITIAL_TILE_SIZE, INITIAL_TILE_SIZE});
}

void Tile::resetTile()
{
    m_isBomb = false;
    m_isFlagged = false;
    m_isCovered = true;
}
