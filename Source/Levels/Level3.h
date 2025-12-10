//
// Created by mateus on 10/12/2025.
//
#pragma once

#include "Level.h"

class Level3 : public Level
{
public:
    Level3(Game* game, HUD *hud);

    void OnUpdate(float deltaTime) override;
    void ProcessInput(SDL_Event &event);

protected:
    void ConfigureCutscene();
};