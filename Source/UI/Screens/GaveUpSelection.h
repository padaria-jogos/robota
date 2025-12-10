//
// Created by mateus on 10/12/2025.
//

#pragma once

#include "UIScreen.h"
#include "Game.h"

class GaveUpSelection : public UIScreen
{
public:
    GaveUpSelection(class Game* game);

    void HandleKeyPress(int key) override;

private:
    Game* mGame;
};