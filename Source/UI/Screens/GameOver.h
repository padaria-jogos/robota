//
// Created by Lucas N. Ferreira on 06/11/25.
//

#pragma once

#include "UIScreen.h"

class GameOver : public UIScreen
{
public:
    GameOver(class Game* game, const std::string& fontName);

    void HandleKeyPress(int key) override;
};
