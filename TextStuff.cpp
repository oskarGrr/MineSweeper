#include "TextStuff.h"
#include <SFML/Graphics.hpp>

sf::Font& TextStuff::getFont()
{
    static sf::Font font;
  
    if(static bool isInit = false; !isInit)
    {
        font.loadFromFile("fonts/FRSCRIPT.TTF");
        isInit = true;
    }

    return font;
}

TextStuff::TextStuff(const char* text, sf::Vector2f location, std::size_t txtSize, sf::Color clr)
    //: m_txt{text, getFont(), }
{
    m_txt.setFont(getFont());
    m_txt.setString(text);
    m_txt.setPosition(location);
    m_txt.setCharacterSize(txtSize);
    m_txt.setFillColor(clr);
    auto const localBounds = m_txt.getLocalBounds();
    m_txt.setOrigin({localBounds.width / 2, localBounds.height / 2});
}

void TextStuff::setColor(sf::Color clr)
{
    m_txt.setFillColor(clr);
}