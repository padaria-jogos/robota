//
// Created by mateus on 10/12/2025.
//

#include "Credits.h"
#include "../../Game.h"

namespace
{
    constexpr float kButtonRowY = -335.0f;
    constexpr float kRectButtonScale = 0.30f;
    constexpr float kSquareButtonScale = 0.30f;
    constexpr int kButtonPointSize = 32;
    constexpr unsigned kRectWrapLength = 360u;
    constexpr unsigned kSquareWrapLength = 120u;
    const Vector2 kRectTextMargin(30.0f, 8.0f);
    const Vector2 kSquareTextMargin(16.0f, 8.0f);

    constexpr float kButtonColumnSpacing = 150.0f;

    const float newOffset = 60.0f;
    const Vector2 kCameraButtonOffset(-1.5f * kButtonColumnSpacing - newOffset, kButtonRowY);
    const Vector2 kBackButtonOffset(-1.0f * kButtonColumnSpacing - newOffset, kButtonRowY);
    const Vector2 kLeftArmButtonOffset(0.37f * kButtonColumnSpacing - newOffset, kButtonRowY);
    const Vector2 kRightArmButtonOffset(1.10f * kButtonColumnSpacing - newOffset, kButtonRowY);
    const Vector2 kCancelArmButtonOffset(2.28f * kButtonColumnSpacing - newOffset, kButtonRowY);

}

Credits::Credits(Game* game)
        :UIScreen(game, "../Assets/Fonts/Arial.ttf"),
        mGame(game)
{
    // add game logo
    AddImage("../Assets/HUD/boxCredits.png", Vector2(0.0f, 0.0f), 1.0f, 0.0f, 1);

    AddImage("../Assets/HUD/Buttons/Level/btnBackgroundSmall.png", Vector2(0.0f, -380.0f), 1.0f, 0.0f, 1);

    // btn esquerdo
    UIButton* leftArmButton = AddButton("Braço Esquerdo", [this]() {
        this->Close();
        mGame->SetScene(GameScene::MainMenu);
    }, kLeftArmButtonOffset, 1.0f, 0.0f, kButtonPointSize, kRectWrapLength, 1000);

    leftArmButton->SetText("");
    leftArmButton->SetAlignment(UITextAlignment::Center);
    leftArmButton->SetBackgroundColor(Vector4(0.0f, 0.0f, 1.0f, 1.0f));
    leftArmButton->SetTextColor(Vector3(1.0f, 1.0f, 1.0f));
    leftArmButton->SetScale(kRectButtonScale);
    leftArmButton->SetBackgroundScale(1.0f);
    leftArmButton->SetBackgroundTextures("../Assets/HUD/Buttons/Level/btnContinueRed.png", "../Assets/HUD/Buttons/Level/btnContinueRedHold.png");
    leftArmButton->SetMargin(kRectTextMargin);


    for (auto* button : mButtons)
    {
        button->SetHighlighted(false);
    }

    mSelectedButtonIndex = 0;
    for (int i = 0; i < static_cast<int>(mButtons.size()); ++i)
    {
        if (mButtons[i] == leftArmButton)
        {
            mSelectedButtonIndex = i;
            break;
        }
    }

    if (mSelectedButtonIndex >= 0 && mSelectedButtonIndex < static_cast<int>(mButtons.size()))
    {
        mButtons[mSelectedButtonIndex]->SetHighlighted(true);
    }
}

void Credits::HandleKeyPress(int key)
{
    if (mButtons.empty())
        return;

    // navega de acordo com o input
    switch (key)
    {
        case SDLK_a:
        {
            // remove highlight
            mButtons[mSelectedButtonIndex]->SetHighlighted(false);

            // move para esquerda (circular)
            mSelectedButtonIndex--;
            if (mSelectedButtonIndex < 0)
                mSelectedButtonIndex = (int)mButtons.size() - 1;

            // highlight
            mButtons[mSelectedButtonIndex]->SetHighlighted(true);
        }
            break;

        case SDLK_d:
        {
            // remove highlight
            mButtons[mSelectedButtonIndex]->SetHighlighted(false);

            // move para direita (circular)
            mSelectedButtonIndex++;
            if (mSelectedButtonIndex >= (int)mButtons.size())
                mSelectedButtonIndex = 0;

            // novo destaque
            mButtons[mSelectedButtonIndex]->SetHighlighted(true);
        }
            break;

        case SDLK_SPACE:
        {
            // click no botão selecionado
            mButtons[mSelectedButtonIndex]->OnClick();
        }
            break;

        default:
            break;

    }
}