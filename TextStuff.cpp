#include "TextStuff.h"
#include <SFML/Graphics.hpp>

TextStuff::TextStuff(const char* text, sf::Vector2f location, const char* fDirectory, const int charSize, sf::Color clr)
{
    f.loadFromFile(fDirectory);
    t.setFont(f);
    t.setString(text);
    t.setPosition(location);
    t.setCharacterSize(charSize);
    t.setFillColor(clr);
}

void TextStuff::setTextColor(sf::Color clr)
{
    t.setFillColor(clr);
}