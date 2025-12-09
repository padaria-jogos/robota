//
// Created by mateus on 09/12/2025.
//

#pragma once

#include "Scene.h"
#include "Camera.h"

class Garage : public Scene
{
public:
    Garage(Game* game);
    ~Garage();

private:
    const Vector3 TILE_SCALE = Vector3(500.0f, 500.0f, 500.0f);

    void Update(float deltaTime);
    void Draw();

    void CreateScene();

    UIImage* mBackgroundImg;

    std::vector<Actor*> mSceneActors;
    Camera* mCamera;
};