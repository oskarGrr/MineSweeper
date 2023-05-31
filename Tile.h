#pragma once
#include <cstdint>
#include <SFML/Graphics.hpp>

#define INITIAL_TILE_SIZE 16

class Tile
{
public:

    friend class MineField;

    Tile();
    ~Tile()=default;

    enum struct TileTypes
    {
        COVERED,
        UNCOVERED,
        FLAG,
        BOMB,
        RED_BOMB,
        BOMB_CROSSED,
        ONE,
        TWO,
        THREE,
        FOUR,
        FIVE,
        SIX,
        SEVEN,
        EIGHT
    };

    void changeTileBasedOnBombCount();

    void changeTile(TileTypes);
    void setTilePosition(sf::Vector2f const& pos) {m_sprite.setPosition(pos);}
    auto const& getSprite() const {return m_sprite;}
    bool isCovered() const {return m_isCovered;}
    void resetTile();//reset m_bomb, m_isFlagged, and m_isCovered

private:
    sf::Sprite m_sprite;
    bool m_isBomb{false};
    bool m_isFlagged{false};
    bool m_isCovered{true};
    uint8_t m_numSurroundingBombs{0};
};