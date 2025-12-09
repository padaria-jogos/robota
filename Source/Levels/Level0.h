//
// Created by mateus on 08/12/2025.
//

#pragma once

#include "Level.h"

class Level0 : public Level
{
public:
    Level0(Game* game, HUD *hud);

    void OnUpdate(float deltaTime) override;
    void ProcessInput(SDL_Event &event);

protected:
    void ConfigureCutscene();
};