#pragma once
#include <SFML/Graphics.hpp>
#include  <cstdint>

class Difficulty
{
public:

    enum Options : uint32_t
    {
        INVALID,
        BEGINNER,
        EASY,
        MEDIUM,
        HARD
    };

    static auto& get() {return s_theDifficulty;}
    
    auto getFieldWidth() const {return m_fieldWidth;}
    auto getFieldHeight() const {return m_fieldHeight;}
    auto getFieldNumOfTiles() const {return m_fieldHeight * m_fieldWidth;}
    auto getBombCount() const {return m_bombCount;}
    auto getDifficultyOption() const {return m_whichDifficultyOption;}

    void changeDifficulty(const Options);

private:

    Difficulty()=default;
    ~Difficulty()=default;

    Difficulty(Difficulty const&)=delete;
    Difficulty(Difficulty&&)=delete;
    Difficulty& operator=(Difficulty const&)=delete;
    Difficulty& operator=(Difficulty&&)=delete;

    static Difficulty s_theDifficulty;

    Options m_whichDifficultyOption {INVALID};
    int m_bombCount     {0};
    int m_fieldWidth    {0};
    int m_fieldHeight   {0};
};