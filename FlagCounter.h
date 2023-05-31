#pragma once
#include <cstddef>
#include <cstdint>
#include <vector>
#include <SFML/Graphics.hpp>

//The flag counter represented as a 7 segment display 
//at the top left of the main window.
class FlagCounter
{
public:
    FlagCounter();
    ~FlagCounter()=default;

    FlagCounter(FlagCounter const&)=delete;
    FlagCounter(FlagCounter&&)=delete;
    FlagCounter& operator=(FlagCounter const&)=delete;
    FlagCounter& operator=(FlagCounter&&)=delete;

    FlagCounter& operator++();
    FlagCounter& operator--();

    auto const& getSprites() const {return m_7segmentSprites;}
    void resetFlagCounter();
    void zeroOutCounter();

private:
    
    //get/set the value that is currently being displayed on a specific segment
    uint32_t getSegmentValue(std::size_t segIndex) const;
    void setSegmentValue(std::size_t segIndex, uint32_t newValue);

    int m_flagCount{0};
    std::size_t m_segmentWidth, m_segmentHeight;
    std::vector<sf::Sprite> m_7segmentSprites;
};