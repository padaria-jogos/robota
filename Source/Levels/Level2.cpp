//
// Created by mateus on 20/11/2025.
//

#include "Level2.h"
// #include "Map/GridMap.h"

Level2::Level2(Game* game, HUD *hud) : Level(game, hud)
{
    // Setup do Player
    mPlayer->LoadFromJson("../Saves/PlayerBuild.json");


    // Setup do Enemy para Arena 2
    mEnemy->LoadFromJson("../Assets/Robots/Rosevif/Rosevif.json");

    // Carrega a Arena 2
    LevelConfig config;
    if (LoadLevelConfig("../Assets/Arena/Arena2/Arena2.json", config)) {
        LoadLevel(config);
    } else {
        SDL_Log("ERRO: Falha ao carregar Arena2!");
    }

    // Posiciona cursor inicial
    if (mGrid) {
        MoveInGrid(mCursor, mPlayer->GetGridX(), mPlayer->GetGridY());
    }
}

void Level2::OnUpdate(float deltaTime)
{
    Level::OnUpdate(deltaTime);
}

void Level2::ProcessInput(SDL_Event &event)
{

}

