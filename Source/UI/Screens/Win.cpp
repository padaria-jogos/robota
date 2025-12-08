//
// Created by mateus on 30/11/2025.
//

#include "Win.h"
#include "HUD.h"
#include "Game.h"

Win::Win(class Game* game) : UIScreen(game, "../Assets/Fonts/Arial.ttf")
{
    if (game->GetHUD())
    {
        game->GetHUD()->SetVisible(false);
    }

    AddText("Você venceu!", Vector2(0.0f, 220.0f), 3.0f);

    AddText("Você pode roubar uma peça do adversario e upgradar seu robo! Mas a demo acaba aqui :(", Vector2(0.0f, 50.0f), 0.75f);

    AddText("Pressione Enter", Vector2(0.0f, 120.0f), 1.0f);

    AddImage("../Assets/future.png", Vector2(0.0f, -170.0f), 0.6f, 0.0f, 1);
}

void Win::HandleKeyPress(int key)
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