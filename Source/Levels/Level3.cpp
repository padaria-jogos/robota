//
// Created by mateus on 10/12/2025.
//

#include "Level3.h"
// #include "Map/GridMap.h"

Level3::Level3(Game* game, HUD *hud) : Level(game, hud)
{
    // ---------- CAMERA ----------
    // default camera
    Vector3 eye(-26.4f, -3005.3f, 1979.1f);
    Vector3 target(-26.4f, 57.4f, -205.4f);
    Vector3 up(0.0f, 0.0f, 1.0f);
    mCamera = new Camera(game, eye, target, up, 70.0f, 10.0f, 10000.0f);

    // aditional camera poses
    // mCamera->AddCameraPose({ Vector3(3500.0f, 0.0f, 1500.0f),   Vector3(50.0f, 0.0f, 0.0f), Vector3::UnitZ });
    mCamera->AddCameraPose({ Vector3(3301.0f, -105.7f, 2158.8f),   Vector3(356.8f, -105.7f, -182.9f), Vector3::UnitZ });
    mCamera->AddCameraPose({ Vector3(-31.1f, 3201.3f, 1489.0f),  Vector3(-87.7f, -39.6f, -420.2f), Vector3::UnitZ });
    mCamera->AddCameraPose({ Vector3(-3446.2f, -56.4f, 2842.6f), Vector3(-762.9f, -56.5f, 206.0f), Vector3::UnitZ });
    mCamera->AddCameraPose({ Vector3(-26.4f, -3005.3f, 4954.1f), Vector3(-26.4f, -1152.7f, 1679.9f), Vector3::UnitZ });


    mGame->SetCamera(mCamera);

    // ---------- OPENING CUTSCENE ----------
    ConfigureCutscene();

    // ---------- LIGHT ----------
    SetWorldLightIntensity(1.0f);

    // ---------- ROBOTS ----------
    // Recarregar build
    mPlayer->LoadFromJson("../Saves/PlayerBuild.json");
    
    // Setup do Enemy - carrega do JSON
    mEnemy->LoadFromJson("../Assets/Robots/Zeke/Zeke.json");

    // ---------- ARENA ----------
    mSkybox = new Skybox(mGame);
    mSkybox->SetTexture("../Assets/Textures/SkyboxLevel3.png");

    // Carrega a Arena 2
    LevelConfig config;
    if (LoadLevelConfig("../Assets/Arena/Arena3/Arena3.json", config)) {
        LoadLevel(config);
    } else {
        SDL_Log("ERRO: Falha ao carregar Arena 3!");
    }

    // ---------- CURSOR ----------
    // Posiciona cursor inicial
    if (mGrid) {
        MoveInGrid(mCursor, mPlayer->GetGridX(), mPlayer->GetGridY());
    }

    // ---------- IA ----------
    InitializeIA();

    // define level para garagem
    mGame->SetLastLevelCompleted(3);

    // cutscene
    mCamera->StartCutscene();

    SDL_Log("\n\n========== LEVEL 3 INICIADO ========== ");

    NotifyPlayer("Terminal de controle RBT ativado!\n\nSelecione sua unidade.");
    NotifyEnemy("Acesso remoto ativado! Bea Bee utiliza um protocolo de segurança ultrapassado.");
}

void Level3::ConfigureCutscene()
{
    std::vector<TransictionKeyframe> cutscenesKeyFrames;
    
    // (-1418.2, -2872.4, 537.1) Target (442.1, 349.8, -18.8)
    // (-88.7, -617.5, 359.2) Target (1463.5, 2711.2, -454.9)
    cutscenesKeyFrames.push_back({
        { Vector3(-1418.2f, -2872.4f, 537.1f), Vector3(442.1f, 349.8f, -18.8f), Vector3::UnitZ },
        { Vector3(-88.7f, -617.5f, 359.2f), Vector3(1463.5f, 2711.2f, -454.9f), Vector3::UnitZ },
        3.0f
    });

    // (856.4, 831.9, -41.2) Target (-155.9, 4362.4, -855.3)
    // (856.4, 831.9, 333.8) Target (-90.3, 4365.3, -544.3)
    cutscenesKeyFrames.push_back({
        { Vector3(856.4f, 831.9f, -41.2f), Vector3(-155.9f, 4362.4f, -855.3f), Vector3::UnitZ },
        { Vector3(856.4f, 831.9f, 333.8f), Vector3(-90.3f, 4365.3f, -544.3f), Vector3::UnitZ },
        2.0f
    });

    // (856.4, 831.9, 333.8) Target (-90.3, 4365.3, -544.3)
    // (856.4, 831.9, 333.8) Target (-90.3, 4365.3, -544.3)
    cutscenesKeyFrames.push_back({
        { Vector3(856.4f, 831.9f, 333.8f), Vector3(-90.3f, 4365.3f, -544.3f), Vector3::UnitZ },
        { Vector3(856.4f, 831.9f, 333.8f), Vector3(-90.3f, 4365.3f, -544.3f), Vector3::UnitZ },
        2.0f
    });

    // (1614.3, 17.5, 68.7) Target (-2019.4, -956.1, 101.6)
    // (406.9, -306.0, 79.6) Target (-3146.7, -1529.6, 243.8)
    cutscenesKeyFrames.push_back({
        { Vector3(1614.3f, 17.5f, 68.7f), Vector3(-2019.4f, -956.1f, 101.6f), Vector3::UnitZ },
        { Vector3(406.9f, -306.0f, 79.6f), Vector3(-3146.7f, -1529.6f, 243.8f), Vector3::UnitZ },
        2.0f
    });

    // (406.9, -306.0, 79.6) Target (-3146.7, -1529.6, 243.8)
    // (406.9, -306.0, 79.6) Target (-3146.7, -1529.6, 243.8)
    cutscenesKeyFrames.push_back({
        { Vector3(406.9f, -306.0f, 79.6f), Vector3(-3146.7f, -1529.6f, 243.8f), Vector3::UnitZ },
        { Vector3(406.9f, -306.0f, 79.6f), Vector3(-3146.7f, -1529.6f, 243.8f), Vector3::UnitZ },
        2.0f
    });

    // (-428.1, -707.0, 267.8) Target (3321.1, -969.2, 432.0)
    // eye(-26.4f, -3005.3f, 1979.1f) target(-26.4f, 57.4f, -205.4f)
    cutscenesKeyFrames.push_back({
        { Vector3(-428.1f, -707.0f, 267.8f), Vector3(3321.1f, -969.2f, 432.0f), Vector3::UnitZ },
        { Vector3(-26.4f, -3005.3f, 1979.1f), Vector3(-26.4f, 57.4f, -205.4f), Vector3::UnitZ },
        3.0f
    });



    mCamera->SetCutsceneKeyframes(cutscenesKeyFrames);
}

void Level3::OnUpdate(float deltaTime)
{
    Level::OnUpdate(deltaTime);
}

void Level3::ProcessInput(SDL_Event &event)
{

}