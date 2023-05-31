#include "textureWrapper.h"
#include "Difficulty.h"
#include <cassert>
#include <thread>
#include <mutex>

TextureWrapper TextureWrapper::s_textureWrapper;

TextureWrapper::TextureWrapper()
{
    //these filepaths need to stay in this order so they line up with 
    //the index values (in enum struct TextureIndicies) to
    //allow for easy enum mapping to the right textures.
    //alternatively you could use a hash table or some other associative data structure
    loadTextures
    ({
        "textures/7segments.png",
        "textures/tiles.png",
        "textures/smileyButtons.png",
        "textures/7segmentsDots.png",
        "textures/winScreen.png",
        "textures/titlebarBeginner.png",
        "textures/titlebarEasy.png",
        "textures/titlebarMedium.png",
        "textures/titlebarHard.png",
        "textures/diffSelectionBackground.png"
    });
}

void TextureWrapper::loadTextures(std::vector<std::string_view> const& filePaths)
{
    if(static bool isInit = false; !isInit) isInit = true;
    else assert(false && "tried to initialize textures more than once!");

    m_textures.resize(filePaths.size());
    for(int i = 0; i < filePaths.size(); ++i)
        m_textures[i].loadFromFile(filePaths[i].data());
}

sf::Texture const& TextureWrapper::getTexture(TextureIndicies whichTexture)
{
    return s_textureWrapper.m_textures[static_cast<size_t>(whichTexture)];
}

sf::Texture const& TextureWrapper::getCorrectTitleBar()
{
    auto whichDiff = Difficulty::get().getDifficultyOption();
    assert(whichDiff != Difficulty::INVALID);

    switch(whichDiff)
    {
    using enum TextureIndicies;
    case Difficulty::BEGINNER: 
        return s_textureWrapper.m_textures[static_cast<size_t>(TITLE_BAR_BEGINNER)];
    case Difficulty::EASY: 
        return s_textureWrapper.m_textures[static_cast<size_t>(TITLE_BAR_EASY)];
    case Difficulty::MEDIUM:
        return s_textureWrapper.m_textures[static_cast<size_t>(TITLE_BAR_MEDIUM)];
    case Difficulty::HARD:
        return s_textureWrapper.m_textures[static_cast<size_t>(TITLE_BAR_HARD)];
    }
}
