#pragma once
#include <SFML/Graphics.hpp>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <cstdint>
#include <atomic>

enum struct GameStates : uint32_t;

//the timer in the top right corner that looks like a 7 segment display
class Timer
{
public:
    Timer(std::atomic<bool> const& isMainWindowOpen, uint32_t windowWidth);
    ~Timer();

    Timer(Timer const&)=delete;
    Timer(Timer&&)=delete;
    Timer& operator=(Timer const&)=delete;
    Timer& operator=(Timer&&)=delete;

    //wakes up the timer thread
    void startTimer();
    void pauseTimer(){std::lock_guard lk{m_timerMutex}; m_isClockRunning = false;}

    void resetTimer();

    auto getTenthsOfSecondPassed() const {return m_tenthsOfSecondPassed;}
    auto isClockRunning() const {return m_isClockRunning;}
    auto const& getSprites() const {return m_7segmentDisplay;}

private:

    void incrementClock();
    void zeroOutSegments();
    uint32_t getSegmentValue(sf::Sprite const& segment) const;
    void setSegment(sf::Sprite& segment, uint32_t newValue);

    //the timer method ran inside the timer thread
    void RunTimer(std::atomic<bool> const& isMainWindowOpen);

    std::vector<sf::Sprite> m_7segmentDisplay;
    std::thread m_timerThread;
    std::condition_variable m_shouldClockRunCond;
    std::mutex m_timerMutex;
    bool m_isClockRunning{false};
    bool m_wasTimerReset{false};
    uint64_t m_tenthsOfSecondPassed{0};
    uint32_t m_segmentWidth{0};
    uint32_t m_segmentHeight{0};
};