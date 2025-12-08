//
// Created by mateus on 20/11/2025.
//

/*
 * Level1
 * Define as especificidades de cada level básico definido
 * por Level
 */

#pragma once

#include "Level.h"

class Level1 : public Level
{
public:
    Level1(Game* game, HUD *hud);
    // ~Level1();

    void OnUpdate(float deltaTime) override;
    void ProcessInput(SDL_Event &event);

protected:
    void ConfigureCutscene();
};