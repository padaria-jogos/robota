//
// Created by mateus on 29/11/2025.
//

#pragma once

#include "Scene.h"
#include "UI/Screens/MainMenu.h"
#include "Actors/Block.h"
#include "Camera.h"

const Vector3 TILE_SCALE = Vector3(500.0f, 500.0f, 500.0f);

class MainMenuScene : public Scene
{
public:
    MainMenuScene(Game* game);
    ~MainMenuScene();

private:
    void Update(float deltaTime);
    void Draw();

    void CreateScene();

    UIImage* mBackgroundImg;
    MainMenu* mMenuUI;

    std::vector<Actor*> mSceneActors;
    Camera* mCamera;
};