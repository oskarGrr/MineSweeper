#pragma once
#include <cstdint>
#include <SFML/Graphics.hpp>
class Difficulty;

bool isMouseOverSprite(sf::RenderWindow const& whichWindow, sf::Sprite const&);

class SmileyButton
{
public:
    enum SmileyButtonTextures : uint32_t
    {
        SMILE_UNPRESSED,
        SMILE_PRESSED,
        SHOCKED,
        GLASSES,
        DEAD
    };

    SmileyButton(sf::Vector2f const& startPosition, SmileyButtonTextures initialSmiley = SMILE_UNPRESSED, 
        sf::Vector2f const& spriteScale = {1.0f, 1.0f});
    ~SmileyButton()=default;

    SmileyButton(SmileyButton const&)=delete;
    SmileyButton(SmileyButton&&)=delete;
    SmileyButton& operator=(SmileyButton const&)=delete;
    SmileyButton& operator=(SmileyButton&&)=delete;

    void changeSmileyFace(SmileyButtonTextures newTextureEnum);
    auto const& getSprite() const {return m_sprite;}

private:
    sf::Sprite m_sprite;
    sf::Vector2f m_spriteScale;
};