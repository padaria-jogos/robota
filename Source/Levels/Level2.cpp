//
// Created by mateus on 20/11/2025.
//

#include "Level2.h"
// #include "Map/GridMap.h"

Level2::Level2(Game* game, HUD *hud) : Level(game, hud)
{
    // ---------- CAMERA ----------
    // default camera
    Vector3 eye(3501.7f, 100.0f, 1650.0f);
    Vector3 target(294.6f, 156.0f, -315.5f);
    Vector3 up(0.0f, 0.0f, 1.0f);
    mCamera = new Camera(game, eye, target, up, 70.0f, 10.0f, 10000.0f);

    // aditional camera poses
    // mCamera->AddCameraPose({ Vector3(3500.0f, 0.0f, 1500.0f),   Vector3(50.0f, 0.0f, 0.0f), Vector3::UnitZ });
    mCamera->AddCameraPose({ Vector3(124.8f, 3434.1f, 1563.4f),   Vector3(114.5f, 176.7f, -409.1f), Vector3::UnitZ });
    mCamera->AddCameraPose({ Vector3(-2967.4f, 150.0f, 1848.1f),  Vector3(192.6f, 94.8f, -357.3f), Vector3::UnitZ });
    mCamera->AddCameraPose({ Vector3(315.8f, -3091.3f, 1864.8f), Vector3(296.9f, 313.5f, -423.9f), Vector3::UnitZ });
    mCamera->AddCameraPose({ Vector3(1551.6f, 159.1f, 4381.8f), Vector3(546.3f, 176.6f, 756.8f), Vector3::UnitZ });


    mGame->SetCamera(mCamera);

    // ---------- OPENING CUTSCENE ----------
    ConfigureCutscene();

    // ---------- LIGHT ----------
    SetWorldLightIntensity(1.0f);

    // ---------- ROBOTS ----------
    // Recarregar build
    mPlayer->LoadFromJson("../Saves/PlayerBuild.json");
    
    // Setup do Enemy - carrega do JSON
    mEnemy->LoadFromJson("../Assets/Robots/Rosevif/Rosevif.json");

    // ---------- ARENA ----------
    mSkybox = new Skybox(mGame);
    mSkybox->SetTexture("../Assets/Textures/SkyboxLevel2.png");

    // Carrega a Arena 2
    LevelConfig config;
    if (LoadLevelConfig("../Assets/Arena/Arena2/Arena2.json", config)) {
        LoadLevel(config);
    } else {
        SDL_Log("ERRO: Falha ao carregar Arena2!");
    }

    // ---------- CURSOR ----------
    // Posiciona cursor inicial
    if (mGrid) {
        MoveInGrid(mCursor, mPlayer->GetGridX(), mPlayer->GetGridY());
    }

    // ---------- IA ----------
    InitializeIA();

    // define level para garagem
    mGame->SetLastLevelCompleted(1);

    // cutscene
    // mCamera->StartCutscene();

    SDL_Log("\n\n========== LEVEL 2 INICIADO ========== ");

    NotifyPlayer("Terminal de controle RBT ativado!\n\nSelecione sua unidade.");
    NotifyEnemy("Acesso remoto ativado! Bea Bee utiliza um protocolo de segurança ultrapassado.");
}

void Level2::ConfigureCutscene()
{
    std::vector<TransictionKeyframe> cutscenesKeyFrames;

    // (-335.3, 71.9, 5688.3) Target (-105.5, 71.9, 1933.3)
    // (-22.4, 73.2, 3841.7) Target (-22.4, -156.6, 86.8)
    cutscenesKeyFrames.push_back({
        { Vector3(-335.3f, 71.9f, 5688.3f), Vector3(-105.5f, 71.9f, 1933.3f), Vector3::UnitZ },
        { Vector3(-22.4f, 73.2f, 3841.7f), Vector3(-22.4f, -156.6f, 86.8f), Vector3::UnitZ },
        2.0f
    });

    // (-141.0, 3107.7, 366.7) Target (1124.1, -368.2, -318.7)
    // (1646.4, 579.3, 207.8) Target (-1144.1, -1933.2, -21.8)
    cutscenesKeyFrames.push_back({
        { Vector3(-141.0f, 3107.7f, 366.7f), Vector3(1124.1f, -368.2f, -318.7f), Vector3::UnitZ },
        { Vector3(1646.4f, 579.3f, 207.8f),  Vector3(-1144.1f, -1933.2f, -21.8f), Vector3::UnitZ },
        4.0f
    });

    // (1646.4, 579.3, 207.8) Target (-1144.1, -1933.2, -21.8)
    // (1646.4, 579.3, 207.8) Target (-1144.1, -1933.2, -21.8)
    cutscenesKeyFrames.push_back({
        { Vector3(1646.4f, 579.3f, 207.8f), Vector3(-1144.1f, -1933.2f, -21.8f), Vector3::UnitZ },
        { Vector3(1646.4f, 579.3f, 207.8f), Vector3(-1144.1f, -1933.2f, -21.8f), Vector3::UnitZ },
        3.0f
    });

    // Position (-1757.6, -2819.2, 235.9) Target (-727.3, 773.7, -189.9)
    // (-1053.9, -391.0, 167.4) Target (-274.1, 3277.4, -127.6)
    cutscenesKeyFrames.push_back({
        { Vector3(-1757.6f, -2819.2f, 235.9f), Vector3(-727.3f, 773.7f, -189.9f), Vector3::UnitZ },
        { Vector3(-1053.9f, -391.0f, 167.4f),  Vector3(-274.1f, 3277.4f, -127.6f), Vector3::UnitZ },
        2.0f
    });

    // (-1053.9, -391.0, 167.4) Target (-274.1, 3277.4, -127.6)
    // (-1053.9, -391.0, 167.4) Target (-274.1, 3277.4, -127.6)
    cutscenesKeyFrames.push_back({
        { Vector3(-1053.9f, -391.0f, 167.4f), Vector3(-274.1f, 3277.4f, -127.6f), Vector3::UnitZ },
        { Vector3(-1053.9f, -391.0f, 167.4f), Vector3(-274.1f, 3277.4f, -127.6f), Vector3::UnitZ },
        2.0f
    });

    // (-1053.9, -391.0, 167.4) Target (-274.1, 3277.4, -127.6)
    // (-1518.9, 696.2, 125.8) Target (1515.2, 2900.6, -169.3)
    cutscenesKeyFrames.push_back({
        { Vector3(-1053.9f, -391.0f, 167.4f), Vector3(-274.1f, 3277.4f, -127.6f), Vector3::UnitZ },
        { Vector3(-1518.9f, 696.2f, 125.8f), Vector3(1515.2f, 2900.6f, -169.3f), Vector3::UnitZ },
        2.0f
    });

    // (-1518.9, 696.2, 125.8) Target (1515.2, 2900.6, -169.3)
    // (-1518.9, 696.2, 125.8) Target (1515.2, 2900.6, -169.3)
    cutscenesKeyFrames.push_back({
        { Vector3(-1518.9f, 696.2f, 125.8f), Vector3(1515.2f, 2900.6f, -169.3f), Vector3::UnitZ },
        { Vector3(-1518.9f, 696.2f, 125.8f), Vector3(1515.2f, 2900.6f, -169.3f), Vector3::UnitZ },
        2.0f
    });

    // (766.5, -138.8, 242.9) Target (963.4, 3617.8, 210.2)
    // eye(3501.7f, 100.0f, 1650.0f);  target(294.6f, 156.0f, -315.5f)
    cutscenesKeyFrames.push_back({
        { Vector3(766.5f, -138.8f, 242.9f),  Vector3(963.4f, 3617.8f, 210.2f), Vector3::UnitZ },
        { Vector3(3501.7f, 100.0f, 1650.0f), Vector3(294.6f, 156.0f, -315.5f), Vector3::UnitZ },
        3.0f
    });



    mCamera->SetCutsceneKeyframes(cutscenesKeyFrames);
}

void Level2::OnUpdate(float deltaTime)
{
    Level::OnUpdate(deltaTime);
}

void Level2::ProcessInput(SDL_Event &event)
{

}

