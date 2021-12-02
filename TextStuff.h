#pragma once
#include <SFML/Graphics.hpp>

class TextStuff
{
public:
    sf::Text t;
    sf::Font f;

    TextStuff() = delete;

    TextStuff(const char* text, sf::Vector2f location, 
              const char* fDirectory, const int charSize, 
        sf::Color clr);

    void setTextColor(sf::Color clr);
};