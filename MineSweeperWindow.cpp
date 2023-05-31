#include <cmath>
#include <cassert>
#include <initializer_list>
#include "MineSweeperWindow.h"
#include "MineField.h"
#include "Difficulty.h"
#include "initialWindowSizes.h"

Window::Window(std::string const& title, sf::Vector2u const& initialSize)
    : m_sfmlWindow{sf::VideoMode{initialSize.x, initialSize.y}, title},
      m_currentWidth{initialSize.x}, m_currentHeight{initialSize.y}
{
}

Window::~Window()
{
    if(isWindowOpen())
        closeWindow();
}

sf::Vector2u Window::getCorrectInitialWndSizeMainWnd()
{
    switch(Difficulty::get().getDifficultyOption())
    {
    case Difficulty::BEGINNER: return {INITIAL_WINDOWSIZE_BEGINNER_X, INITIAL_WINDOWSIZE_BEGINNER_Y};
    case Difficulty::EASY:     return {INITIAL_WINDOWSIZE_EASY_X,     INITIAL_WINDOWSIZE_EASY_Y};
    case Difficulty::MEDIUM:   return {INITIAL_WINDOWSIZE_MEDIUM_X,   INITIAL_WINDOWSIZE_MEDIUM_Y};
    case Difficulty::HARD:     return {INITIAL_WINDOWSIZE_HARD_X,     INITIAL_WINDOWSIZE_HARD_Y};
    }
}

void Window::drawTiles(MineField const& field)
{
    for(auto const& tile : field.getTiles()) 
        m_sfmlWindow.draw(tile.getSprite());
}

//for keeping the same aspect ratio after a resize window event
void Window::maintainAspectRatio()
{
    //get how much the size of the window changed in the x and y direction
    auto newSize = m_sfmlWindow.getSize();
    int widthDifference  = std::abs(static_cast<int>(newSize.x) - static_cast<int>(m_currentWidth));
    int heightDifference = std::abs(static_cast<int>(newSize.y) - static_cast<int>(m_currentHeight));

    if(widthDifference > heightDifference)//make the new height match the new width
        newSize.y = newSize.x * (static_cast<float>(m_currentHeight) / m_currentWidth);
    else if(heightDifference > widthDifference)//make the new width match the new height
        newSize.x = newSize.y * (static_cast<float>(m_currentWidth) / m_currentHeight);
        
    m_currentWidth = newSize.x;
    m_currentHeight = newSize.y;
    m_sfmlWindow.setSize(newSize);
}