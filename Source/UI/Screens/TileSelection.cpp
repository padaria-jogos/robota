//
// Created by mateus on 30/11/2025.
//

#include "TileSelection.h"
#include "../../Levels/Level.h"

namespace
{
    constexpr float kSelectButtonRowY = -295.0f;
    constexpr float kSelectButtonScale = 0.36f;
    constexpr int kSelectButtonPointSize = 12;
    constexpr unsigned kSelectWrapLength = 360u;
}

TileSelection::TileSelection(class Game* game)
        :UIScreen(game, "../Assets/Fonts/Arial.ttf"),
        mGame(game)
{
    // add game logo
    // AddImage("../Assets/UIBackground.png", Vector2(0.0f, 0.0f), 0.7f, 0.0f, 1);

    UIButton* selectButton = AddButton("Selecionar Robô/Mira", [this]() {
        auto* level = mGame->GetLevel();
        if (level) {
            level->HandleAction();
        }
    }, Vector2(0.0f, kSelectButtonRowY), 1.0f, 0.0f, kSelectButtonPointSize, kSelectWrapLength, 10);

    selectButton->SetBackgroundScale(kSelectButtonScale);
    selectButton->SetBackgroundColor(Vector4(0.0f, 0.0f, 1.0f, 1.0f));
    selectButton->SetTextColor(Vector3(1.0f, 1.0f, 1.0f));

    for (auto* button : mButtons)
    {
        button->SetHighlighted(false);
    }

    selectButton->SetHighlighted(true);
    mSelectedButtonIndex = 0;
}

void TileSelection::HandleKeyPress(int key)
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