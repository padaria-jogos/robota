//
// Created by Lucas N. Ferreira on 22/05/25.
//

#pragma once

#include <string>
#include <SDL.h>
#include "../Math.h"
#include "UIImage.h"

class UIText : public UIImage {
public:
    UIText(class Game* game, const std::string& text, class Font* font, const Vector2 &offset, float scale = 1.0f, float angle = 0.0f,
           int pointSize = 40, const unsigned wrapLength = 1024, int drawOrder = 100);

    ~UIText();

    void Draw(class Shader* shader) override;

    void SetText(const std::string& name);
    void SetTextColor(const Vector3 &color);
    void SetBackgroundColor(const Vector4 &color) { mBackgroundColor = color; }
    void SetMargin(const Vector2 &margin) { mMargin = margin; }

protected:
    std::string mText;
    class Font* mFont;

    unsigned int mPointSize;
    unsigned int mWrapLength;

    Vector2 mMargin;

    Vector3 mTextColor;
    Vector4 mBackgroundColor;
};
