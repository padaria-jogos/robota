//
// Created by mateus on 20/11/2025.
//

#include "Level1.h"
// #include "Map/GridMap.h"

Level1::Level1(Game* game, HUD *hud) : Level(game, hud)
{
    // ---------- CAMERA ----------
    // default camera
    Vector3 eye(0.0f, -3500.0f, 1500.0f);
    Vector3 target(50.0f, 0.0f, 0.0f);
    Vector3 up(0.0f, 0.0f, 1.0f);
    mCamera = new Camera(game, eye, target, up, 70.0f, 10.0f, 10000.0f);

    // aditional camera poses
    mCamera->AddCameraPose({ Vector3(3500.0f, 0.0f, 1500.0f),   Vector3(50.0f, 0.0f, 0.0f), Vector3::UnitZ });
    mCamera->AddCameraPose({ Vector3(0.0f, 3500.0f, 1500.0f),   Vector3(-11.1f, -0.3f, 0.0f), Vector3::UnitZ });
    mCamera->AddCameraPose({ Vector3(-3500.0f, 0.0f, 1500.0f),  Vector3(50.0f, 0.0f, 0.0f), Vector3::UnitZ });
    mCamera->AddCameraPose({ Vector3(30.0f, -1140.0f, 5000.0f), Vector3(23.3f, 65.7f, 1078.6f), Vector3::UnitZ });

    mGame->SetCamera(mCamera);

    // ---------- OPENING CUTSCENE ----------
    ConfigureCutscene();

    // ---------- LIGHT ----------
    SetWorldLightIntensity(1.0f);

    // ---------- ROBOTS ----------
    // Teste Setup do Player
    mPlayer->LoadFromJson("../Assets/Robots/Robota/Robota.json");
    
    // Trocamos
    mPlayer->EquipPartFromJson("../Assets/Parts/Arms/LeftClaw.json");
    
    // Salvar build atual do player
    mPlayer->SaveToJson("../Saves/PlayerBuild.json");
    
    // Recarregar build
    mPlayer->LoadFromJson("../Saves/PlayerBuild.json");
    
    // Setup do Enemy - carrega do JSON
    mEnemy->LoadFromJson("../Assets/Robots/BeaBee/BeaBee.json");

    // ---------- ARENA ----------
    mSkybox = new Skybox(mGame);
    mSkybox->SetTexture("../Assets/Textures/SkyboxLevel1.png");

    // Carrega a Arena 1
    LevelConfig config;
    if (LoadLevelConfig("../Assets/Arena/Arena1/Arena1.json", config)) {
        LoadLevel(config);
    } else {
        SDL_Log("ERRO: Falha ao carregar Arena1!");
    }

    // ---------- CURSOR ----------
    // Posiciona cursor inicial
    if (mGrid) {
        MoveInGrid(mCursor, mPlayer->GetGridX(), mPlayer->GetGridY());
    }
}

void Level1::ConfigureCutscene()
{
    std::vector<TransictionKeyframe> cutscenesKeyFrames;

    // Position (1208.9, -1812.7, 458.4) Target (1208.4, -1811.8, 458.1);
    // Position (447.3, -2108.1, 458.4)  Target (447.3, -2107.1, 458.1)
    cutscenesKeyFrames.push_back({
        { Vector3(1208.9f, -1812.7f, 458.4f), Vector3(1208.4f, -1811.8f, 458.1f), Vector3::UnitZ },
        { Vector3(447.3f,  -2108.1f, 458.4f), Vector3(447.3f,  -2107.1f, 458.1f), Vector3::UnitZ },
        2.0f
    });

    // stand still
    cutscenesKeyFrames.push_back({
        { Vector3(447.3f,  -2108.1f, 458.4f), Vector3(447.3f,  -2107.1f, 458.1f), Vector3::UnitZ },
        { Vector3(447.3f,  -2108.1f, 458.4f), Vector3(447.3f,  -2107.1f, 458.1f), Vector3::UnitZ },
        2.0f
    });

    // Position (-1783.0, -90.1, 505.5) Target (-1782.2, -89.6, 505.3);
    // Position (-1779.1, 557.5, 499.4) Target (-1778.2, 557.1, 499.2)
    cutscenesKeyFrames.push_back({
        { Vector3(-1783.0f, -90.1f, 505.5f), Vector3(-1782.2f, -89.6f, 505.3f), Vector3::UnitZ },
        { Vector3(-1779.1f, 557.5f, 499.4f), Vector3(-1778.2f, 557.1f, 499.2f), Vector3::UnitZ },
        2.0f
    });

    // stand still
    cutscenesKeyFrames.push_back({
        { Vector3(-1779.1f, 557.5f, 499.4f), Vector3(-1778.2f, 557.1f, 499.2f), Vector3::UnitZ },
        { Vector3(-1779.1f, 557.5f, 499.4f), Vector3(-1778.2f, 557.1f, 499.2f), Vector3::UnitZ },
        2.0f
    });

    // Position (-972.3, 422.2, 322.7) Target (-972.3, 421.3, 322.6);
    // Position (0.0, -3500.0, 1500.0) Target (50.0, 0.0, 0.0)
    cutscenesKeyFrames.push_back({
        { Vector3(-980.2f, 32.1f, 381.2f), Vector3(-1282.7f, -3605.6f, -131.2f), Vector3::UnitZ },
        { Vector3(0.0f,   -3500.0f, 1500.0f), Vector3(50.0f, 0.0f, 0.0f), Vector3::UnitZ },
        3.0f
    });

    mCamera->SetCutsceneKeyframes(cutscenesKeyFrames);
}

void Level1::OnUpdate(float deltaTime)
{
    Level::OnUpdate(deltaTime);
}

void Level1::ProcessInput(SDL_Event &event)
{

}

