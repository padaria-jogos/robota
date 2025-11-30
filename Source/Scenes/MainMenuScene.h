//
// Created by mateus on 29/11/2025.
//

#pragma once

#include "Scene.h"
#include "UI/Screens/MainMenu.h"

class MainMenuScene : public Scene
{
public:
    MainMenuScene(Game* game);
    ~MainMenuScene();

private:
    void Update(float deltaTime);
    void Draw();

    UIImage* mBackgroundImg;
    MainMenu* mMenuUI;
};