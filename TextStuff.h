#pragma once
#include <SFML/Graphics.hpp>

class TextStuff
{
public:
    sf::Text m_txt;
    static sf::Font& getFont();//same font for all txt

    TextStuff(const char* text, sf::Vector2f location, std::size_t txtSize, sf::Color clr);
    ~TextStuff()=default;

    void setColor(sf::Color clr);
};