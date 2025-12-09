//
// Created by Lucas N. Ferreira on 22/05/25.
//

#include "UIButton.h"
#include "../Renderer/Texture.h"
#include "../Renderer/Renderer.h"
#include "../Game.h"

UIButton::UIButton(class Game* game, std::function<void()> onClick, const std::string& text, class Font* font,
                   const Vector2 &offset, float scale, float angle, int pointSize, const unsigned wrapLength, int drawOrder)
        :UIText(game, text, font, offset, scale, angle, pointSize, wrapLength, drawOrder)
        ,mOnClick(onClick)
        ,mHighlighted(false)
        ,mNormalTexture(nullptr)
        ,mHoldTexture(nullptr)
        ,mMarginTexture(nullptr)
        ,mMarginHoldTexture(nullptr)
        ,mBackgroundScale(0.6f)
{
    auto renderer = game->GetRenderer();
    if(renderer)
    {
        mNormalTexture = renderer->GetTexture("../Assets/HUD/vbuttons.png");
        mHoldTexture = renderer->GetTexture("../Assets/HUD/holdvbuttons.png");
        mMarginTexture = renderer->GetTexture("../Assets/HUD/marginBtn.png");
        mMarginHoldTexture = renderer->GetTexture("../Assets/HUD/marginBtnh.png");
    }

}

void UIButton::SetBackgroundTextures(const std::string& normalPath, const std::string& holdPath)
{
    auto renderer = GetGame()->GetRenderer();
    if(!renderer)
    {
        return;
    }

    if(!normalPath.empty())
    {
        mNormalTexture = renderer->GetTexture(normalPath);
    }

    if(!holdPath.empty())
    {
        mHoldTexture = renderer->GetTexture(holdPath);
    }
}

void UIButton::SetMarginTextures(const std::string& normalPath, const std::string& holdPath)
{
    auto renderer = GetGame()->GetRenderer();
    if(!renderer)
    {
        return;
    }

    if(!normalPath.empty())
    {
        mMarginTexture = renderer->GetTexture(normalPath);
    }

    if(!holdPath.empty())
    {
        mMarginHoldTexture = renderer->GetTexture(holdPath);
    }
}

UIButton::~UIButton()
{

}


void UIButton::OnClick()
{
    // Call attached handler, if it exists
    if (mOnClick) {
        mOnClick();
    }
}

void UIButton::Draw(class Shader* shader)
{
    if(!mIsVisible)
    {
        return;
    }

    Texture* bgTexture = nullptr;
    if(mHighlighted && mHoldTexture)
    {
        bgTexture = mHoldTexture;
    }
    else if(mNormalTexture)
    {
        bgTexture = mNormalTexture;
    }

    Matrix4 rotMat = Matrix4::CreateRotationZ(mAngle);
    Matrix4 transMat = Matrix4::CreateTranslation(Vector3(mOffset.x, mOffset.y, 0.0f));

    if(bgTexture)
    {
        const float scaled = mScale * mBackgroundScale;
        Matrix4 scaleMat = Matrix4::CreateScale(static_cast<float>(bgTexture->GetWidth()) * scaled,
                            static_cast<float>(bgTexture->GetHeight()) * scaled, 1.0f);
        Matrix4 world = scaleMat * rotMat * transMat;
        shader->SetMatrixUniform("uWorldTransform", world);
        shader->SetFloatUniform("uTextureFactor", 1.0f);
        bgTexture->SetActive();
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
    }
    else if(mTexture)
    {
        const float scaled = mScale * mBackgroundScale;
        Matrix4 scaleMat = Matrix4::CreateScale((static_cast<float>(mTexture->GetWidth()) + mMargin.x) * scaled,
                            (static_cast<float>(mTexture->GetHeight()) + mMargin.y) * scaled, 1.0f);
        Matrix4 world = scaleMat * rotMat * transMat;
        shader->SetMatrixUniform("uWorldTransform", world);
        shader->SetFloatUniform("uTextureFactor", 0.0f);
        shader->SetVectorUniform("uBaseColor", mBackgroundColor);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
    }

    Texture* marginTexture = nullptr;
    if(mHighlighted && mMarginHoldTexture)
    {
        marginTexture = mMarginHoldTexture;
    }
    else if(mMarginTexture)
    {
        marginTexture = mMarginTexture;
    }

    if(marginTexture)
    {
        const float scaled = mScale * mBackgroundScale;
        Matrix4 scaleMat = Matrix4::CreateScale(static_cast<float>(marginTexture->GetWidth()) * scaled,
                             static_cast<float>(marginTexture->GetHeight()) * scaled, 1.0f);
        Matrix4 world = scaleMat * rotMat * transMat;
        shader->SetMatrixUniform("uWorldTransform", world);
        shader->SetFloatUniform("uTextureFactor", 1.0f);
        marginTexture->SetActive();
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
    }

    if(mTexture)
    {
        // Draw the rendered text texture atop the background sprite
        UIImage::Draw(shader);
    }
}