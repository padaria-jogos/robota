//
// Created by Lucas N. Ferreira on 22/05/25.
//

#include "UIElement.h"
#include "../Game.h"

UIElement::UIElement(class Game* game, const Vector2 &offset, const float scale, const float angle, int drawOrder)
        :mGame(game)
        ,mOffset(offset)
        ,mScale(scale)
        ,mAngle(angle)
        ,mIsVisible(true)
        ,mDrawOrder(drawOrder)
{
    mGame->GetRenderer()->AddUIElement(this);
}

UIElement::~UIElement()
{
    mGame->GetRenderer()->RemoveUIElement(this);
}