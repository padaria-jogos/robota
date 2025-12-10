//
// Created by mateus on 20/11/2025.
//

#include "Level2.h"
// #include "Map/GridMap.h"

Level2::Level2(Game* game, HUD *hud) : Level(game, hud)
{
    // ---------- CAMERA ----------
    // default camera
    Vector3 eye(-3411.8f, -1.0f, 1931.2f);
    Vector3 target(-176.3f, -1.1f, -162.6f);
    Vector3 up(0.0f, 0.0f, 1.0f);
    mCamera = new Camera(game, eye, target, up, 70.0f, 10.0f, 10000.0f);

    // aditional camera poses
    // mCamera->AddCameraPose({ Vector3(3500.0f, 0.0f, 1500.0f),   Vector3(50.0f, 0.0f, 0.0f), Vector3::UnitZ });
    // mCamera->AddCameraPose({ Vector3(124.8f, 3434.1f, 1563.4f),   Vector3(114.5f, 176.7f, -409.1f), Vector3::UnitZ });
    // mCamera->AddCameraPose({ Vector3(-2967.4f, 150.0f, 1848.1f),  Vector3(192.6f, 94.8f, -357.3f), Vector3::UnitZ });
    // mCamera->AddCameraPose({ Vector3(315.8f, -3091.3f, 1864.8f), Vector3(296.9f, 313.5f, -423.9f), Vector3::UnitZ });
    // mCamera->AddCameraPose({ Vector3(1551.6f, 159.1f, 4381.8f), Vector3(546.3f, 176.6f, 756.8f), Vector3::UnitZ });

    // mCamera->AddCameraPose({ Vector3(-3411.8f, -1.0f, 1931.2f), Vector3(-176.3f, -1.1f, -162.6f), Vector3::UnitZ });
    mCamera->AddCameraPose({ Vector3(108.9f, -1841.2f, 5737.7f), Vector3(104.1f, -981.9f, 1726.1f), Vector3::UnitZ });
    mCamera->AddCameraPose({ Vector3(318.1f, -3508.5f, 2022.3f), Vector3(302.3f, -654.0f, -924.3f), Vector3::UnitZ });
    mCamera->AddCameraPose({ Vector3(3461.2f, 26.0f, 2144.8f), Vector3(437.2f, 26.0f, -92.7f), Vector3::UnitZ });
    mCamera->AddCameraPose({ Vector3(225.2f, 3562.7f, 2032.5f), Vector3(216.9f, 932.9f, -715.9f), Vector3::UnitZ });

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
    mGame->SetLastLevelCompleted(2);

    // cutscene
    mCamera->StartCutscene();

    SDL_Log("\n\n========== LEVEL 2 INICIADO ========== ");

    NotifyPlayer("Terminal de controle RBT ativado!\n\nSelecione sua unidade.");
    NotifyEnemy("Acesso remoto ativado! Bea Bee utiliza um protocolo de segurança ultrapassado.");
}

void Level2::ConfigureCutscene()
{
    std::vector<TransictionKeyframe> cutscenesKeyFrames;

    //Position (318.0, -3487.8, 2008.4) Target (299.6, -165.0, -397.8)
    //Position (302.4, -668.3, 1451.7) Target (283.3, 2775.8, -777.3)

    //Position (-2056.9, 2634.7, 929.8) Target (160.2, -103.4, -632.1)
    //Position (-604.9, 487.3, 675.0) Target (2126.7, -1647.0, -1008.9)

    //Position (-736.7, 940.8, 269.8) Target (-671.1, 4696.7, -591.1)
    //(-730.0, 1325.4, 510.2) Target (-668.4, 4848.1, -1051.7)

    //(-730.0, 1325.4, 510.2) Target (-668.4, 4848.1, -1051.7)
    //(-730.0, 1325.4, 510.2) Target (-668.4, 4848.1, -1051.7)

    //(172.8, 2171.5, 322.2) Target (-3540.9, 1516.8, -473.1)
    //(-58.4, 1308.6, 272.2) Target (-2687.1, 4030.9, -457.1)

    //(-58.4, 1308.6, 272.2) Target (-2687.1, 4030.9, -457.1)
    //(-58.4, 1308.6, 272.2) Target (-2687.1, 4030.9, -457.1)

    //Position (-804.2, -637.8, 205.9) Target (-604.3, 3174.2, -324.4)
    //eye(-3411.8f, -1.0f, 1931.2f)target(-176.3f, -1.1f, -162.6f)


    cutscenesKeyFrames.push_back({
        { Vector3(318.0f, -3487.8f, 2008.4f), Vector3(299.6f, -165.0f, -397.8f), Vector3::UnitZ },
        { Vector3(302.4f,  -668.3f, 1451.7f), Vector3(283.3f, 2775.8f, -777.3f), Vector3::UnitZ },
        3.0f
    });

    cutscenesKeyFrames.push_back({
        { Vector3(-2056.9f, 2634.7f, 929.8f), Vector3(160.2f, -103.4f, -632.1f), Vector3::UnitZ },
        { Vector3(-604.9f,   487.3f, 675.0f), Vector3(2126.7f, -1647.0f, -1008.9f), Vector3::UnitZ },
        3.0f
    });

    cutscenesKeyFrames.push_back({
        { Vector3(-736.7f, 940.8f, 269.8f), Vector3(-671.1f, 4696.7f, -591.1f), Vector3::UnitZ },
        { Vector3(-730.0f, 1325.4f, 510.2f), Vector3(-668.4f, 4848.1f, -1051.7f), Vector3::UnitZ },
        2.0f
    });

    cutscenesKeyFrames.push_back({
        { Vector3(-730.0f, 1325.4f, 510.2f), Vector3(-668.4f, 4848.1f, -1051.7f), Vector3::UnitZ },
        { Vector3(-730.0f, 1325.4f, 510.2f), Vector3(-668.4f, 4848.1f, -1051.7f), Vector3::UnitZ },
        2.0f
    });

    cutscenesKeyFrames.push_back({
        { Vector3(172.8f, 2171.5f, 322.2f), Vector3(-3540.9f, 1516.8f, -473.1f), Vector3::UnitZ },
        { Vector3(-58.4f, 1308.6f, 272.2f), Vector3(-2687.1f, 4030.9f, -457.1f), Vector3::UnitZ },
        2.0f
    });

    cutscenesKeyFrames.push_back({
        { Vector3(-58.4f, 1308.6f, 272.2f), Vector3(-2687.1f, 4030.9f, -457.1f), Vector3::UnitZ },
        { Vector3(-58.4f, 1308.6f, 272.2f), Vector3(-2687.1f, 4030.9f, -457.1f), Vector3::UnitZ },
        2.0f
    });

    cutscenesKeyFrames.push_back({
        { Vector3(-804.2f, -637.8f, 205.9f), Vector3(-604.3f, 3174.2f, -324.4f), Vector3::UnitZ },
        { Vector3(-3411.8f, -1.0f, 1931.2f), Vector3(-176.3f,  -1.1f, -162.6f), Vector3::UnitZ },
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

