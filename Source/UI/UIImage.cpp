//
// Created by Lucas N. Ferreira on 28/05/25.
//

#include "UIImage.h"
#include "../Renderer/Texture.h"
#include "../Renderer/Shader.h"
#include "../Renderer/Renderer.h"
#include "../Actors/Actor.h"
#include "../Game.h"


UIImage::UIImage(class Game* game, const Vector2 &offset, const float scale, const float angle, int drawOrder)
        :UIElement(game, offset, scale, angle, drawOrder)
        ,mTexture(nullptr)
{

}

UIImage::UIImage(class Game* game, const std::string &imagePath, const Vector2 &offset, const float scale, const float angle, int drawOrder)
        :UIElement(game, offset, scale, angle)
{
    mTexture = GetGame()->GetRenderer()->GetTexture(imagePath);
}

UIImage::~UIImage()
{

}

void UIImage::Draw(class Shader* shader)
{
    if(!mTexture || !mIsVisible)
        return;

    // Scale the quad by the width/height of texture
    Matrix4 scaleMat = Matrix4::CreateScale(static_cast<float>(mTexture->GetWidth()) * mScale,
                                            static_cast<float>(mTexture->GetHeight()) * mScale, 1.0f);

    Matrix4 rotMat = Matrix4::CreateRotationZ(mAngle);

    // Translate to position on screen
    Matrix4 transMat = Matrix4::CreateTranslation(Vector3(mOffset.x, mOffset.y, 0.0f));

    // Set world transform
    Matrix4 world = scaleMat * rotMat * transMat;
    shader->SetMatrixUniform("uWorldTransform", world);

    // Set uTextureFactor
    shader->SetFloatUniform("uTextureFactor", 1.0f);

    // Set current texture
    mTexture->SetActive();

    // Draw quad
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
}
void UIImage::SetTexture(const std::string &imagePath)
{
    mTexture = GetGame()->GetRenderer()->GetTexture(imagePath);
}