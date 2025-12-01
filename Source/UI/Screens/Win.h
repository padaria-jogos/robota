//
// Created by mateus on 30/11/2025.
//

#pragma once

#include "UIScreen.h"

class Win : public UIScreen
{
public:
    Win(class Game* game);

    void HandleKeyPress(int key) override;
};
