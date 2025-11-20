//
// Created by Lucas N. Ferreira on 05/11/25.
//

#pragma once


#include "UIElement.h"
#include "../Math.h"

class UIRect : public UIElement
{
public:
    UIRect(class Game* game, const Vector2 &offset, const Vector2 &size, float scale = 1.0f, float angle = 0.0f, int drawOrder = 100);
    ~UIRect();

    void Draw(class Shader* shader) override;
    void SetColor(const Vector4 &color) { mColor = color; }

protected:
    Vector2 mSize;
    Vector4 mColor;
};

