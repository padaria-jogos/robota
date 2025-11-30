//
// Created by mateus on 30/11/2025.
//

#include "TileSelection.h"

TileSelection::TileSelection(class Game* game)
        :UIScreen(game, "../Assets/Fonts/Arial.ttf"),
        mGame(game)
{
    // add game logo
    // AddImage("../Assets/UIBackground.png", Vector2(0.0f, 0.0f), 0.7f, 0.0f, 1);

    // btn start
    AddButton("Selecionar Tile", [this]() {
        // virtually press button 2
        SDL_Event event;
        event.type = SDL_KEYDOWN;
        event.key.keysym.sym = SDLK_SPACE;
        event.key.repeat = 0;

        SDL_PushEvent(&event);

    }, Vector2(0.0f, -300.0f), 1.0f, 0.0f, 24, 1024, 10);

    mButtons.back()->SetBackgroundColor(Vector4(0.0f, 0.0f, 1.0f, 1.0f));
    mButtons.back()->SetTextColor(Vector3(1.0f, 1.0f, 1.0f));
}

void TileSelection::HandleKeyPress(int key)
{
    if (mButtons.empty())
        return;

    // navega de acordo com o input
    switch (key)
    {
        case SDLK_LEFT:
        {
            // remove highlight
            mButtons[mSelectedButtonIndex]->SetHighlighted(false);

            // move para cima (circular)
            mSelectedButtonIndex--;
            if (mSelectedButtonIndex < 0)
                mSelectedButtonIndex = (int)mButtons.size() - 1;

            // highlight
            mButtons[mSelectedButtonIndex]->SetHighlighted(true);
        }
            break;

        case SDLK_RIGHT:
        {
            // remove highlight
            mButtons[mSelectedButtonIndex]->SetHighlighted(false);

            // move para baixo (circular)
            mSelectedButtonIndex++;
            if (mSelectedButtonIndex >= (int)mButtons.size())
                mSelectedButtonIndex = 0;

            // novo destaque
            mButtons[mSelectedButtonIndex]->SetHighlighted(true);
        }
            break;

        case SDLK_KP_ENTER:
        case SDLK_RETURN:
        {
            // click no botão selecionado
            mButtons[mSelectedButtonIndex]->OnClick();
        }
            break;

        default:
            break;

    }
}