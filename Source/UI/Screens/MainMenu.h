//
// Created by Lucas N. Ferreira on 06/11/25.
//

#pragma once

#include "UIScreen.h"

class MainMenu : public UIScreen
{
public:
    MainMenu(class Game* game, const std::string& fontName);

    void HandleKeyPress(int key) override;
};