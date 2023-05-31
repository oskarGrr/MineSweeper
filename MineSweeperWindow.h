#pragma once
#include <SFML/graphics.hpp>
#include <cstdint>
#include <initializer_list>
#include "Difficulty.h"

class MineField;
class SmileyButton;
enum struct DifficultyOptions : uint32_t;

class Window
{
public:

    Window(std::string const& title, sf::Vector2u const& initialSize);
    ~Window();

    template <typename DrawableType, typename... DrawableTypes>
    void variadicDraw(DrawableType const drawable, DrawableTypes const&... drawables)
    {
        m_sfmlWindow.draw(drawable), (m_sfmlWindow.draw(drawables), ...);
    }

    void drawAllSpritesInContainer(auto const& spriteContainer)
    {
        for(auto const& sprite : spriteContainer) m_sfmlWindow.draw(sprite);
    }

    static sf::Vector2u getCorrectInitialWndSizeMainWnd();

    void changeSize(sf::Vector2u const & newSize){m_sfmlWindow.setSize(newSize);}
    void drawTiles(MineField const& field);
    void clearWindow(){m_sfmlWindow.clear();}
    void display(){m_sfmlWindow.display();}

    Window(Window const&)=delete;
    Window(Window&&)=delete;
    Window& operator=(Window const&)=delete;
    Window& operator=(Window&&)=delete;

    void maintainAspectRatio();//for keeping the same aspect ratio after a resize event
    bool isWindowOpen() const {return m_sfmlWindow.isOpen();}
    bool pollEvents(sf::Event& event) {return m_sfmlWindow.pollEvent(event);}
    void closeWindow() {m_sfmlWindow.close();}

    auto getWidth() const {return m_currentWidth;}
    auto getHeight() const {return m_currentHeight;}
    auto getMousePosition() const {return sf::Mouse::getPosition(m_sfmlWindow);}
    auto getMappedMousePosition() const {return m_sfmlWindow.mapPixelToCoords(getMousePosition());}

private:
    sf::RenderWindow m_sfmlWindow;
    uint32_t m_currentWidth{0};
    uint32_t m_currentHeight{0};
};