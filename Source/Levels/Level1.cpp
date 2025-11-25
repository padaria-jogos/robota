//
// Created by mateus on 20/11/2025.
//

#include "Level1.h"

Level1::Level1(Game* game, HUD *hud)
{
    mGame = game;
    // log controls
    SDL_Log("E/Q: forward/backward; W/S: up/down; A/D: right/left; SPACE: shoot");

    // hud
    hud = new HUD(mGame, "../Assets/Fonts/Arial.ttf");
    hud->SetScore(0);

    // create player
    mShip = new Ship(mGame);

    // create camera
    Vector3 eye(-300.0f, 0.0f, 0.0f);
    Vector3 target(20.0f, 0.0f, 0.0f);
    Vector3 up(0.0f, 0.0f, 1.0f);
    mCamera = new Camera(mGame, eye, target, up, 70.0f, 10.0f, 10000.0f);
    mCamera->SetTargetActor(mShip);
    mGame->SetCamera(mCamera);

    // spawn floor
    SpawnFloor();
}

void Level1::Update(float deltaTime)
{

}

void Level1::SpawnFloor()
{
    // spawn the floor using multiple cubes
    const float spacing = 500.0f;
    const Vector3 center(spacing, 0.0f, -500.0f);
    int n = 2; // size scale

    // construct a grid of n x n blocks centered at 'center'
    float offset = (n - 1) * 0.5f;

    for (int y = 0; y < n; y++)
    {
        for (int x = 0; x < n; x++)
        {
            Block* wall = new Block(mGame);
            wall->SetScale(Vector3(500.0f, 500.0f, 500.0f));

            Vector3 pos;
            pos.x = center.x + (x - offset) * spacing;
            pos.y = center.y + (y - offset) * spacing;
            pos.z = center.z;

            wall->SetPosition(pos);
            wall->SetTexture(0);
        }
    }

}