#pragma once
enum tileTextures : unsigned int;
#include <SFML/Graphics.hpp>
class Difficulty;

class Button
{
public:  
    sf::Sprite button;
  
    Button();

    Button(sf::Texture& t);

    Button(const sf::Texture& t,  
           const int& xStart,
           const sf::Vector2f& position,
           const short& xDiv,
           const short& ydiv);

    void changeTileTexture(tileTextures tileTextureType);
    bool isMouseOver(sf::RenderWindow& win);
    static void createTiles(sf::Texture& t);
    static void createSegments(sf::Texture& y);
    static void decrementFlagDisplay(const int& index);
    static void incrementFlagDisplay(const int& whichSegment);
    static void initFlagCounter(const short& whichSegment);
    static void refreshFlagSegments();
    static void refreshClockSegments();
    static void incrementClockDisplay(const short& index);
    void resetButtonLeftClick(uint8_t& gameState, sf::RenderWindow& mainWin);
};