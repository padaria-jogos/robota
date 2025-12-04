//
// Created by mateus on 20/11/2025.
//

#include "Level1.h"
// #include "Map/GridMap.h"

Level1::Level1(Game* game, HUD *hud) : Level(game, hud)
{
    // Teste Setup do Player
    mPlayer->LoadFromJson("../Assets/Robots/Robota/Robota.json");
    
    // Trocamos
    mPlayer->EquipPartFromJson("../Assets/Parts/Arms/LeftClaw.json");
    
    // Salvar build atual do player
    mPlayer->SaveToJson("../Saves/PlayerBuild.json");
    
    // Recarregar build
    mPlayer->LoadFromJson("../Saves/PlayerBuild.json");
    
    // Setup do Enemy - carrega do JSON
    mEnemy->LoadFromJson("../Assets/Robots/Rosevif/Rosevif.json");
    
    // Carrega a Arena 1
    LevelConfig config;
    if (LoadLevelConfig("../Assets/Arena/Arena1/Arena1.json", config)) {
        LoadLevel(config);
    } else {
        SDL_Log("ERRO: Falha ao carregar Arena1!");
    }
    
    // Posiciona cursor inicial
    if (mGrid) {
        MoveInGrid(mCursor, mPlayer->GetGridX(), mPlayer->GetGridY());
    }
}

void Level1::OnUpdate(float deltaTime)
{
    Level::OnUpdate(deltaTime);
}

void Level1::ProcessInput(SDL_Event &event)
{

}

