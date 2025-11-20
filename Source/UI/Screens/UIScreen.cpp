// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "UIScreen.h"
#include "../../Game.h"
#include "../../Renderer/Shader.h"

UIScreen::UIScreen(Game* game, const std::string& fontName)
	:mGame(game)
	,mPos(0.f, 0.f)
	,mSize(0.f, 0.f)
	,mState(UIState::Active)
    ,mSelectedButtonIndex(-1)
{
    // add this screen to the game's UI screen list
    mGame->PushUI(this);

    // load font
    mFont = mGame->GetRenderer()->GetFont(fontName);
}

UIScreen::~UIScreen()
{
    for (auto t : mTexts) {
        delete t;
    }
    mTexts.clear();

    for (auto b : mButtons) {
        delete b;
    }
	mButtons.clear();

    for (auto img : mImages) {
        delete img;
    }
    mImages.clear();

    for (auto rect : mRects) {
        delete rect;
    }
    mRects.clear();
}

void UIScreen::Update(float deltaTime)
{
	
}

void UIScreen::HandleKeyPress(int key)
{

}

void UIScreen::Close()
{
	mState = UIState::Closing;
}

UIText* UIScreen::AddText(const std::string& name, const Vector2& offset, float scale, float angle, const int pointSize, const int unsigned wrapLength, int drawOrder)
{
    UIText* text = new UIText(mGame, name, mFont, offset, scale, angle, pointSize, wrapLength, drawOrder);
    mTexts.push_back(text);
    return text;
}

UIButton* UIScreen::AddButton(const std::string& name, std::function<void()> onClick, const Vector2& offset, float scale, float angle, const int pointSize, const int unsigned wrapLength, int drawOrder)
{
    UIButton* button = new UIButton(mGame, onClick, name, mFont, offset, scale, angle, pointSize, wrapLength, drawOrder);
    mButtons.push_back(button);

    // if this is the first button, select it
    if (mButtons.size() == 1)
    {
        mSelectedButtonIndex = 0;
        button->SetHighlighted(true);
    }

    return button;
}

UIImage* UIScreen::AddImage(const std::string& imagePath, const Vector2& offset, float scale, float angle, int drawOrder)
{
    UIImage* image = new UIImage(mGame, imagePath, offset, scale, angle, drawOrder);
    mImages.push_back(image);
    return image;
}

UIRect *UIScreen::AddRect(const Vector2 &offset, const Vector2 &size, float scale, float angle, int drawOrder)
{
    UIRect *rect = new UIRect(mGame, offset, size, scale, angle, drawOrder);
    mRects.push_back(rect);
    return rect;
}