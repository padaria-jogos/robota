//
// Created by mateus on 30/11/2025.
//

#include "ActionSelection.h"
#include "../../Levels/Level.h"

ActionSelection::ActionSelection(class Game* game)
        :UIScreen(game, "../Assets/Fonts/Arial.ttf"),
        mGame(game)
{
    // add game logo
    // AddImage("../Assets/UIBackground.png", Vector2(0.0f, 0.0f), 0.7f, 0.0f, 1);

    // btn esquerdo
    UIButton* leftArmButton = AddButton("Braço Esquerdo", [this]() {
        auto* level = mGame->GetLevel();
        if (level) {
            level->SetSelectedSlot(PartSlot::LeftArm);
            level->HandleAction();
        }
    }, Vector2(-300.0f, -300.0f), 1.0f, 0.0f, 24, 1024, 10);

    leftArmButton->SetBackgroundColor(Vector4(0.0f, 0.0f, 1.0f, 1.0f));
    leftArmButton->SetTextColor(Vector3(1.0f, 1.0f, 1.0f));
    leftArmButton->SetBackgroundTextures("../Assets/HUD/Ybuttons.png", "../Assets/HUD/holdYbuttons.png");

    // btn direito
    UIButton* rightArmButton = AddButton("Braço Direito", [this]() {
        auto* level = mGame->GetLevel();
        if (level) {
            level->SetSelectedSlot(PartSlot::RightArm);
            level->HandleAction();
        }
    }, Vector2(0.0f, -300.0f), 1.0f, 0.0f, 24, 1024, 20);

    rightArmButton->SetBackgroundColor(Vector4(0.0f, 0.0f, 1.0f, 1.0f));
    rightArmButton->SetTextColor(Vector3(1.0f, 1.0f, 1.0f));
    rightArmButton->SetBackgroundTextures("../Assets/HUD/Pbuttons.png", "../Assets/HUD/holdPbuttons.png");

    // btn desistir
    UIButton* skipButton = AddButton("Pular turno", [this]() {
        auto* level = mGame->GetLevel();
        if (level) {
            level->HandleWait();
        }
    }, Vector2(300.0f, -300.0f), 1.0f, 0.0f, 24, 1024, 20);

    skipButton->SetBackgroundColor(Vector4(0.0f, 0.0f, 1.0f, 1.0f));
    skipButton->SetTextColor(Vector3(1.0f, 1.0f, 1.0f));
    skipButton->SetBackgroundTextures("../Assets/HUD/Rbuttons.png", "../Assets/HUD/holdRbuttons.png");
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