//
// Created by mateus on 09/12/2025.
//

#pragma once

#include "Scene.h"
#include "Camera.h"
#include "Actors/Robot.h"
#include "UI/Screens/GarageScreen.h"

class GarageScene : public Scene
{
public:
    GarageScene(Game* game);
    ~GarageScene();

private:
    const Vector3 TILE_SCALE = Vector3(500.0f, 500.0f, 500.0f);

    void Update(float deltaTime);
    void Draw();

    void CreateScene();

    GarageScreen* mGarageUI;
    Robot* mRobot = nullptr;
    UIImage* mBackgroundImg;

    std::vector<Actor*> mSceneActors;
    Camera* mCamera;
};