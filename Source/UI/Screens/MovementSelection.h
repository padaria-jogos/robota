//
// Created by mateus on 09/12/2025.
//

#pragma once

#include "UIScreen.h"
#include "Game.h"

class MovementSelection : public UIScreen
{
public:
    MovementSelection(class Game* game);

    void HandleKeyPress(int key) override;

private:
    Game* mGame;
};