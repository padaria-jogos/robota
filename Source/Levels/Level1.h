//
// Created by mateus on 20/11/2025.
//

#ifndef ROBOTA_LEVEL1_H
#define ROBOTA_LEVEL1_H

#include "../Game.h"
#include "../UI/Screens/HUD.h"
#include "../Actors/Ship.h"
#include "../Camera.h"
#include "../Actors/Block.h"

class Level1
{
public:
    Level1(class Game* game, HUD *hud);

    Ship* mShip;
    Camera* mCamera;

    void Update(float deltaTime);

private:
    Game* mGame;
    void SpawnFloor();
};


#endif //ROBOTA_LEVEL1_H