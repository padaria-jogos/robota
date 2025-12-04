#pragma once

#include "Level.h"

class Level2 : public Level
{
public:
    Level2(Game* game, HUD *hud);

    void OnUpdate(float deltaTime) override;
    void ProcessInput(SDL_Event &event);
};