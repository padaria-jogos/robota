//
// Created by mateus on 10/12/2025.
//

#pragma once

#include "UIScreen.h"

class Credits : public UIScreen
{
public:
    Credits(class Game* game);

    void HandleKeyPress(int key) override;

private:
    Game* mGame;
};
