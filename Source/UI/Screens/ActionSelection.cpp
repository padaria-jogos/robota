//
// Created by mateus on 30/11/2025.
//

#include "ActionSelection.h"
#include "../../Levels/Level.h"
#include "../../Camera.h"

namespace
{
    constexpr float kButtonRowY = -285.0f;
    constexpr float kRectButtonScale = 0.30f;
    constexpr float kSquareButtonScale = 0.30f;
    constexpr int kButtonPointSize = 32;
    constexpr unsigned kRectWrapLength = 360u;
    constexpr unsigned kSquareWrapLength = 120u;
    const Vector2 kRectTextMargin(30.0f, 8.0f);
    const Vector2 kSquareTextMargin(16.0f, 8.0f);

    constexpr float kButtonColumnSpacing = 150.0f; 

    const Vector2 kCameraButtonOffset(-1.5f * kButtonColumnSpacing, kButtonRowY);
    const Vector2 kLeftArmButtonOffset(-0.5f * kButtonColumnSpacing, kButtonRowY);
    const Vector2 kRightArmButtonOffset(0.5f * kButtonColumnSpacing, kButtonRowY);
    const Vector2 kBackButtonOffset(1.5f * kButtonColumnSpacing, kButtonRowY);
}

ActionSelection::ActionSelection(class Game* game)
        :UIScreen(game, "../Assets/Fonts/Arial.ttf"),
        mGame(game)
{
    // add game logo
    // AddImage("../Assets/UIBackground.png", Vector2(0.0f, 0.0f), 0.7f, 0.0f, 1);

    UIButton* cameraButton = AddButton("", [this]() {
        auto* camera = mGame->GetCamera();
        if (camera)
        {
            camera->HandleKeyPress(SDLK_c);
        }
    }, kCameraButtonOffset, 1.0f, 0.0f, kButtonPointSize, kSquareWrapLength, 10);

    cameraButton->SetText("Câmera");
    cameraButton->SetTextColor(Vector3(1.0f, 1.0f, 1.0f));
    cameraButton->SetScale(kSquareButtonScale);
    cameraButton->SetBackgroundScale(1.0f);
    cameraButton->SetBackgroundTextures("../Assets/HUD/cambuttons.png", "../Assets/HUD/camHoldbuttons.png");
    cameraButton->SetMarginTextures("../Assets/HUD/marginBtnQuad.png", "../Assets/HUD/marginBtnhQuad.png");
    cameraButton->SetMargin(kSquareTextMargin);

    // btn esquerdo
    UIButton* leftArmButton = AddButton("Braço Esquerdo", [this]() {
        auto* level = mGame->GetLevel();
        if (level) {
            level->SetSelectedSlot(PartSlot::LeftArm);
            level->HandleAction();
        }
    }, kLeftArmButtonOffset, 1.0f, 0.0f, kButtonPointSize, kRectWrapLength, 10);

    leftArmButton->SetText("Braço Esquerdo\nRobota Braço");
    leftArmButton->SetAlignment(UITextAlignment::Center);
    leftArmButton->SetBackgroundColor(Vector4(0.0f, 0.0f, 1.0f, 1.0f));
    leftArmButton->SetTextColor(Vector3(1.0f, 1.0f, 1.0f));
    leftArmButton->SetScale(kRectButtonScale);
    leftArmButton->SetBackgroundScale(1.0f);
    leftArmButton->SetBackgroundTextures("../Assets/HUD/Ybuttons.png", "../Assets/HUD/holdYbuttons.png");
    leftArmButton->SetMargin(kRectTextMargin);

    // btn direito
    UIButton* rightArmButton = AddButton("Braço Direito", [this]() {
        auto* level = mGame->GetLevel();
        if (level) {
            level->SetSelectedSlot(PartSlot::RightArm);
            level->HandleAction();
        }
    }, kRightArmButtonOffset, 1.0f, 0.0f, kButtonPointSize, kRectWrapLength, 20);

    rightArmButton->SetText("Braço Direito\nRobota Braço");
    rightArmButton->SetAlignment(UITextAlignment::Center);
    rightArmButton->SetBackgroundColor(Vector4(0.0f, 0.0f, 1.0f, 1.0f));
    rightArmButton->SetTextColor(Vector3(1.0f, 1.0f, 1.0f));
    rightArmButton->SetScale(kRectButtonScale);
    rightArmButton->SetBackgroundScale(1.0f);
    rightArmButton->SetBackgroundTextures("../Assets/HUD/Pbuttons.png", "../Assets/HUD/holdPbuttons.png");
    rightArmButton->SetMargin(kRectTextMargin);

    // btn voltar
    UIButton* backButton = AddButton("", [this]() {
        auto* level = mGame->GetLevel();
        if (level)
        {
            level->HandleCancel();
        }
    }, kBackButtonOffset, 1.0f, 0.0f, kButtonPointSize, kSquareWrapLength, 10);

    backButton->SetText("Voltar");
    backButton->SetTextColor(Vector3(1.0f, 1.0f, 1.0f));
    backButton->SetScale(kSquareButtonScale);
    backButton->SetBackgroundScale(1.0f);
    backButton->SetBackgroundTextures("../Assets/HUD/voltbuttons.png", "../Assets/HUD/voltHoldbuttons.png");
    backButton->SetMarginTextures("../Assets/HUD/marginBtnQuad.png", "../Assets/HUD/marginBtnhQuad.png");
    backButton->SetMargin(kSquareTextMargin);

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

void ActionSelection::HandleKeyPress(int key)
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