//
// Created by Lucas N. Ferreira on 28/05/25.
//

#pragma once

#include <string>
#include <SDL_image.h>
#include "UIElement.h"

class UIImage : public UIElement
{
public:
    UIImage(class Game* game, const Vector2 &offset, float scale = 1.0f, float angle = 0.0f, int drawOrder = 100);
    UIImage(class Game* game, const std::string &imagePath, const Vector2 &offset, float scale = 1.0f, float angle = 0.0f, int drawOrder = 100);

    ~UIImage();

    void Draw(class Shader* shader) override;

protected:
    class Texture* mTexture;
};
