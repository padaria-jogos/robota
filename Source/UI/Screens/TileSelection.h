//
// Created by mateus on 30/11/2025.
//

#pragma once

#include "UIScreen.h"
#include "Game.h"

class TileSelection : public UIScreen
{
public:
    TileSelection(class Game* game);

    void HandleKeyPress(int key) override;

private:
    Game* mGame;
};
