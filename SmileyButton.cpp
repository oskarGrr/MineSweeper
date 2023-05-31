#include "SmileyButton.h"
#include "Difficulty.h"
#include "MineField.h"
#include "textureWrapper.h"

//scale is defaulted to {1.0f, 1.0f} and starts the smiley as initialSmiley which is defaulted to SMILE_UNPRESSED
SmileyButton::SmileyButton(sf::Vector2f const& startPosition, SmileyButtonTextures initialSmiley, 
        sf::Vector2f const& spriteScale)
{
    auto& texture = TextureWrapper::getTexture(TextureIndicies::SMILEY_FACES);
    m_sprite.setPosition(startPosition);
    
    m_sprite.setTexture(texture);
    changeSmileyFace(initialSmiley);
    m_sprite.setScale(spriteScale);
}

void SmileyButton::changeSmileyFace(const SmileyButtonTextures newSmiley)
{
    auto& texture = TextureWrapper::getTexture(TextureIndicies::SMILEY_FACES);
    
    int smileyWidth = texture.getSize().x / 5;//the width of a smiley before scaling
    int smileyHeight = texture.getSize().y;//the height of a smiley before scaling

    //the rectangle for the SMILE_UNPRESSED smiley in the smiley texture
    sf::IntRect newTextureRectangle{0, 0, smileyWidth, smileyHeight};

    //now all we need to do to navigate to the correct smiley face
    //in the smiley texture is to change the x value of the newTextureRectange
    switch(newSmiley)
    {
    case SMILE_PRESSED: newTextureRectangle.left = smileyWidth;     break;
    case SHOCKED:       newTextureRectangle.left = smileyWidth * 2; break;
    case DEAD:          newTextureRectangle.left = smileyWidth * 3; break;
    case GLASSES:       newTextureRectangle.left = smileyWidth * 4; 
    }

    m_sprite.setTextureRect(newTextureRectangle);
}