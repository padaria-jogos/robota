//
// Created by mateus on 10/12/2025.
//

#pragma once

#include "UIScreen.h"
#include "Game.h"
#include "Actors/Robot.h"

class GarageScreen : public UIScreen
{
public:
    GarageScreen(class Game* game, Robot* robot);

    void HandleKeyPress(int key) override;

private:
    Game* mGame;
    Robot* mRobot;

    GameScene mNextGameScene;

    std::string part1;
    std::string part2;
};
