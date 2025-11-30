//
// Created by mateus on 29/11/2025.
//

#pragma once

#include "Game.h"

class Scene
{
public:
    Scene(Game* game);
    ~Scene();

protected:

    Game* mGame;

    void Update(float deltaTime);
    void Draw();
};
