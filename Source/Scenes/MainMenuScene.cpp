//
// Created by mateus on 29/11/2025.
//

#include "MainMenuScene.h"
#include "../UI/UIImage.h"

MainMenuScene::MainMenuScene(Game* game)
    : Scene(game)
{
    // Load background texture
    // mBackgroundImg = new UIImage(mGame, "../Assets/MenuBackground.png", Vector2(-600.0f, 0.0f), 1.0f, 0.0f, -999);

    // Add the UI screen on top
    mMenuUI = new MainMenu(game, "../Assets/Fonts/Arial.ttf");
}

MainMenuScene::~MainMenuScene()
{
    delete mBackgroundImg;
    // delete mMenuUI;
}

void MainMenuScene::Update(float deltaTime)
{
    // Update scene-specific logic if needed

    // Call base class update to handle UI
    mMenuUI->HandleKeyPress(deltaTime);
}

void MainMenuScene::Draw()
{
    // mGame->GetRenderer()->DrawTexture(mBackgroundImg->GetTexture(), Vector2(0.0f, 0.0f), 1.0f, 0.0f, -999);
}
