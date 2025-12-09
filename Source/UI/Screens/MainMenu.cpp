//
// Created by Lucas N. Ferreira on 06/11/25.
//

#include "MainMenu.h"
#include "../../Game.h"

MainMenu::MainMenu(class Game* game, const std::string& fontName)
        :UIScreen(game, fontName)
{
    // add game logo
    // AddImage("../Assets/MenuBackground.png", Vector2(0.0f, 0.0f), 1.0f, 0.0f, 0);
    AddImage("../Assets/Logo.png", Vector2(0.0f, 100.0f), 0.4f, 0.0f, 1);

    // btn start
    AddButton("New Game", [this]() {
        // close main menu and start the game
        SDL_Log("Starting Game");
        this->Close();
        mGame->SetScene(GameScene::Level0);
    }, Vector2(0.0f, -150.0f), 1.0f, 0.0f, 24, 1024, 100);  // draw order grande para a imagem ficar por baixo

    mButtons.back()->SetBackgroundColor(Vector4(0.0f, 0.0f, 1.0f, 1.0f));
    mButtons.back()->SetTextColor(Vector3(1.0f, 1.0f, 1.0f));

    // btn quit
    AddButton("Quit", [this]() {
        // close the game
        mGame->Quit();
    }, Vector2(0.0f, -200.0f), 1.0f, 0.0f, 24, 1024, 101);

    mButtons.back()->SetBackgroundColor(Vector4(0.0f, 0.0f, 1.0f, 1.0f));
    mButtons.back()->SetTextColor(Vector3(1.0f, 1.0f, 1.0f));
}

void MainMenu::HandleKeyPress(int key)
{
    if (mButtons.empty())
        return;

    // navega de acordo com o input
    switch (key)
    {
        case SDLK_UP:
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

        case SDLK_DOWN:
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