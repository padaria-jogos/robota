//
// Created by Lucas N. Ferreira on 08/12/23.
//

#pragma once

#include <string>

#include "UIScreen.h"

class HUD : public UIScreen
{
public:
    HUD(class Game* game, const std::string& fontName);

    void SetHealth(int health);
    void SetScore(int score);

private:

    // HUD elements
    UIImage *mHealth1;
    UIImage *mHealth2;
    UIImage *mHealth3;
    UIText  *mScore;
};
