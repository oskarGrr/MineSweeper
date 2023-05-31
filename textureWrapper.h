#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <cstdint>
#include <initializer_list>
#include <string_view>

//indicies into the static vector of textures s_textures.
enum struct TextureIndicies : size_t
{
    SEVEN_SEGMENTS,
    TILES,
    SMILEY_FACES,
    SEGMENT_DOTS,
    WIN_SCREEN_BACKGROUND,
    TITLE_BAR_BEGINNER,
    TITLE_BAR_EASY,
    TITLE_BAR_MEDIUM,
    TITLE_BAR_HARD,
    DIFFICULTY_SELECTION_BACKGROUND
};

//this is a singleton class because I want the ctor to be called 
//and textures to be loaded before main().
class TextureWrapper
{
public:

    static sf::Texture const& getTexture(TextureIndicies whichTexture);
    static sf::Texture const& getCorrectTitleBar();

private:

    TextureWrapper();
    ~TextureWrapper()=default;
    TextureWrapper(TextureWrapper const&)=delete;
    TextureWrapper(TextureWrapper&&)=delete;
    TextureWrapper& operator=(TextureWrapper const&)=delete;
    TextureWrapper& operator=(TextureWrapper&&)=delete;

    static TextureWrapper s_textureWrapper;
    std::vector<sf::Texture> m_textures;

    void loadTextures(std::vector<std::string_view> const& filePaths);
};