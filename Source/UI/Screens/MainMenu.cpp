//
// Created by Lucas N. Ferreira on 06/11/25.
//

#include "MainMenu.h"
#include "../../Game.h"

MainMenu::MainMenu(class Game* game, const std::string& fontName)
        :UIScreen(game, fontName)
{
    // add game logo
    AddImage("../Assets/Logo.png", Vector2(0.0f, 100.0f), 0.4f, 0.0f, 1);

    const float buttonsY = -220.0f;
    const float buttonSpacing = 260.0f;

    // btn start
    UIButton* startButton = AddButton("New Game", [this]() {
        // close main menu and start the game
        SDL_Log("Starting Game");
        this->Close();
        mGame->SetScene(GameScene::Level1);
    }, Vector2(-buttonSpacing * 0.5f, buttonsY), 1.0f, 0.0f, 24, 1024, 10);

    startButton->SetBackgroundColor(Vector4(0.0f, 0.0f, 1.0f, 1.0f));
    startButton->SetTextColor(Vector3(1.0f, 1.0f, 1.0f));
    startButton->SetBackgroundScale(0.5f);
    startButton->SetMargin(Vector2(0.0f, 0.0f));

    // btn quit
    UIButton* quitButton = AddButton("Quit", [this]() {
        // close the game
        mGame->Quit();
    }, Vector2(buttonSpacing * 0.5f, buttonsY), 1.0f, 0.0f, 24, 1024, 20);

    quitButton->SetBackgroundColor(Vector4(0.0f, 0.0f, 1.0f, 1.0f));
    quitButton->SetTextColor(Vector3(1.0f, 1.0f, 1.0f));
    quitButton->SetBackgroundScale(0.5f);
    quitButton->SetMargin(Vector2(0.0f, 0.0f));
    quitButton->SetBackgroundTextures("../Assets/HUD/Rbuttons.png", "../Assets/HUD/holdRbuttons.png");
}

void MainMenu::HandleKeyPress(int key)
{
    if (mButtons.empty())
        return;

    // navega de acordo com o input
    switch (key)
    {
        case SDLK_LEFT:
        case SDLK_a:
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
        case SDLK_d:
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