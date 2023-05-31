#include <SFML/Graphics.hpp>
#include <condition_variable>
#include <mutex>
#include <cassert>
#include <ranges>
#include "MineSweeperApp.h"
#include "MineSweeperTimer.h"
#include "Difficulty.h"
#include "textureWrapper.h"

Timer::Timer(std::atomic<bool> const& isMainWindowOpen, uint32_t windowWidth)
    : m_timerThread{&Timer::RunTimer, this, std::cref(isMainWindowOpen)}
{
    auto diffOption = Difficulty::get().getDifficultyOption();
    m_7segmentDisplay.resize(diffOption == Difficulty::BEGINNER ? 3 : 5);

    auto& texture = TextureWrapper::getTexture(TextureIndicies::SEVEN_SEGMENTS);

    auto textureSize = texture.getSize();
    m_segmentWidth = textureSize.x / 12;
    m_segmentHeight = textureSize.y;

    for(auto& sprite : m_7segmentDisplay)
        sprite.setTexture(texture);

    float distanceFromRight = 6.0f + m_segmentWidth;
    float distanceFromTop = 4.0f;
    sf::Vector2f segPos{windowWidth - distanceFromRight, distanceFromTop};

    for(auto& segment : std::views::reverse(m_7segmentDisplay))
    {
        segment.setPosition(segPos);
        segPos.x -= m_segmentWidth;
    }

    zeroOutSegments();
}

Timer::~Timer()
{
    m_isClockRunning = true;
    m_shouldClockRunCond.notify_one();
    m_timerThread.join();
}

//the timer method that runs inside the timer thread
void Timer::RunTimer(std::atomic<bool> const& isMainWindowOpen)
{
    while(isMainWindowOpen.load())
    {
        std::unique_lock lock{m_timerMutex};
        while(!m_isClockRunning)
        {
            //was the timer reset as opposed to just paused 
            //or a spurious condition variable signal
            if(m_wasTimerReset)
            {
                zeroOutSegments();
                m_wasTimerReset = false;
            }

            m_shouldClockRunCond.wait(lock);
        }
        lock.unlock();

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        incrementClock();
    }
}

//wakes up the timer thread
void Timer::startTimer()
{
    std::lock_guard lk{m_timerMutex};
    m_isClockRunning = true;
    m_shouldClockRunCond.notify_one();
}

void Timer::resetTimer()
{
    std::lock_guard lk{m_timerMutex};
    m_isClockRunning = false;
    m_tenthsOfSecondPassed = 0;
    m_wasTimerReset = true;
    m_shouldClockRunCond.notify_one();
}

//go up by one tenth of a second / 100ms
void Timer::incrementClock()
{
    ++m_tenthsOfSecondPassed;
   
    //go in reverse since the least significant segment is at the back of vector
    for(auto& segment : std::views::reverse(m_7segmentDisplay))
    {
        auto currentValue = getSegmentValue(segment);
        auto newValue = currentValue + 1;
        setSegment(segment, newValue % 10);
        if(newValue <= 9) break;//if we dont need to carry over
    }
}

void Timer::zeroOutSegments()
{
    for(auto& segment : m_7segmentDisplay)
        setSegment(segment, 0);
}

uint32_t Timer::getSegmentValue(sf::Sprite const& segment) const
{
    return segment.getTextureRect().left / m_segmentWidth;
}

//set the value of a segment 0-9
void Timer::setSegment(sf::Sprite& segment, uint32_t newValue)
{
    segment.setTextureRect({static_cast<int>(newValue * m_segmentWidth), 0, 
        static_cast<int>(m_segmentWidth), static_cast<int>(m_segmentHeight)});
}