//
// Created by Lucas N. Ferreira on 06/11/25.
//

#include "GameOver.h"
#include "../../Game.h"

GameOver::GameOver(class Game* game, const std::string& fontName)
        :UIScreen(game, fontName)
{
    AddText("Game Over", Vector2(0.0f, 0.0f), 2.0f);

    AddText("Press Enter", Vector2(0.0f, 200.0f), 1.0f);
}

void GameOver::HandleKeyPress(int key)
{
    if (key == SDLK_RETURN)
    {
        // return to main menu
        delete mGame->GetLevel();
        mGame->SetLevel(nullptr);
        this->Close();
        mGame->SetScene(GameScene::MainMenu);
    }
}