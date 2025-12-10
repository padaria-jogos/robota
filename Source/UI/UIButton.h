//
// Created by Lucas N. Ferreira on 22/05/25.
//

#pragma once

#include <functional>
#include <string>
#include "UIText.h"
#include "../Math.h"

class Texture;

class UIButton : public UIText
{
public:
    UIButton(class Game* game, std::function<void()> onClick, const std::string& text, class Font* font,
             const Vector2 &offset, float scale = 1.0f, float angle = 0.0f, int pointSize = 40, const unsigned wrapLength = 1024, int drawOrder = 100);

    ~UIButton();

    void Draw(class Shader* shader) override;

    void SetHighlighted(bool sel);
    bool GetHighlighted() const { return mHighlighted; }

    void SetBackgroundScale(float scale) { mBackgroundScale = scale; }
    float GetBackgroundScale() const { return mBackgroundScale; }
    void SetBackgroundTextures(const std::string& normalPath, const std::string& holdPath);
    void SetMarginTextures(const std::string& normalPath, const std::string& holdPath);

    // Returns true if the point is within the button's bounds
    bool ContainsPoint(const Vector2& pt) const;

    // Called when button is clicked
    void OnClick();

private:
    // Callback funtion
    std::function<void()> mOnClick;

    // Check if the button is highlighted
    bool mHighlighted;

    Texture* mNormalTexture;
    Texture* mHoldTexture;
    Texture* mMarginTexture;
    Texture* mMarginHoldTexture;
    float mBackgroundScale;
};