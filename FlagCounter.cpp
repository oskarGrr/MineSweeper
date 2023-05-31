#include "FlagCounter.h"
#include "Difficulty.h"
#include "textureWrapper.h"
#include <cassert>
#include <ranges>

FlagCounter::FlagCounter()
{
    m_7segmentSprites.resize(3);

    auto& texture = TextureWrapper::getTexture(TextureIndicies::SEVEN_SEGMENTS);

    for(auto& sprite : m_7segmentSprites)
        sprite.setTexture(texture);

    auto textureSize = texture.getSize();
    m_segmentWidth = textureSize.x / 12;
    m_segmentHeight = textureSize.y;

    resetFlagCounter();
    
    float distanceFromLeft = 6.0f;//distance from the left side of the window
    float distanceFromTop = 4.0f;//distance from the top of the window
    for(int i = 0; i < m_7segmentSprites.size(); ++i)
    {
        float xPosition = (i == 0) ? distanceFromLeft : (i * m_segmentWidth + 6);
        m_7segmentSprites[i].setPosition({xPosition, distanceFromTop});
    }
}

FlagCounter& FlagCounter::operator++()
{
    ++m_flagCount;
    //if the flag count is negative the unsigned segment display 
    //will show zero until it goes positive again.
    if(m_flagCount < 0)
        return *this;
    //else the flag count is positive

    //start at the back of the segments vector since 
    //that is where the least significant segment is
    std::size_t segIndex = m_7segmentSprites.size() - 1;
    for(auto const& sprites : std::views::reverse(m_7segmentSprites))
    {
        auto currValue = getSegmentValue(segIndex);
        auto newValue = currValue + 1;
        setSegmentValue(segIndex, newValue % 10);
        //if we dont need to carry over to the more significant segment
        if(newValue <= 9) break;

        --segIndex;
    }

    return *this;
}

FlagCounter& FlagCounter::operator--()
{
    //if the flag count is negative the unsigned segment display 
    //will show zero until it goes positive again.
    if(m_flagCount <= 0)
    {
        --m_flagCount;
        return *this;
    }
    //else the flag count is positive

    //start at the back of the segments vector since 
    //that is where the least significant segment is
    std::size_t segIndex = m_7segmentSprites.size() - 1;
    for(auto const& sprites : std::views::reverse(m_7segmentSprites))
    {
        auto currValue = getSegmentValue(segIndex);
        int newValue = currValue - 1;

        if(newValue >= 0)
        {
            setSegmentValue(segIndex, newValue);
            --m_flagCount;
            break;
        }

        setSegmentValue(segIndex, 9);
        --segIndex;
    }

    return *this;
}

void FlagCounter::resetFlagCounter()
{
    m_flagCount = Difficulty::get().getBombCount();
    setSegmentValue(m_7segmentSprites.size() - 1, m_flagCount % 10);//ones place
    setSegmentValue(m_7segmentSprites.size() - 2, m_flagCount % 100 / 10);//tens place
    setSegmentValue(m_7segmentSprites.size() - 3, m_flagCount / 100);//hundreds
}

void FlagCounter::zeroOutCounter()
{
    m_flagCount = 0;
    for(int i = 0; i < m_7segmentSprites.size(); ++i)
        setSegmentValue(i, 0);
}

uint32_t FlagCounter::getSegmentValue(std::size_t segIndex) const
{
    return m_7segmentSprites[segIndex].getTextureRect().left / m_segmentWidth;
}

//set the segment to 0-9
void FlagCounter::setSegmentValue(std::size_t segIndex, uint32_t newValue)
{
    assert(newValue >=0 && newValue <= 9);
    m_7segmentSprites[segIndex].setTextureRect
    ({
        static_cast<int>(newValue * m_segmentWidth), 
        0, static_cast<int>(m_segmentWidth), static_cast<int>(m_segmentHeight)
    });
}