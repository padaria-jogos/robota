//
// Created by mateus on 08/12/2025.
//

#include "Level0.h"

Level0::Level0(Game* game, HUD *hud) : Level(game, hud)
{
    // ---------- CAMERA ----------
    // default camera
    Vector3 eye(-1.1f, 3548.0f, 1540.8f);
    Vector3 target(-11.7f, 191.7f, -258.5f);
    Vector3 up(0.0f, 0.0f, 1.0f);
    mCamera = new Camera(game, eye, target, up, 70.0f, 10.0f, 10000.0f);

    // aditional camera poses
    mCamera->AddCameraPose({ Vector3(-2977.4f, -59.1f, 1507.4f),   Vector3(362.9f, -117.4f, -414.0f), Vector3::UnitZ });
    mCamera->AddCameraPose({ Vector3(24.6f, -3371.6f, 1672.3f),   Vector3(70.6f, -150.0f, -356.7f), Vector3::UnitZ });
    mCamera->AddCameraPose({ Vector3(2855.2f, 50.0f, 1833.6f), Vector3(-207.1f, 103.5f, -350.9f), Vector3::UnitZ });
    mCamera->AddCameraPose({ Vector3(-29.9f, 660.5f, 5754.1f), Vector3(-31.4f, 183.5f, 1975.8f), Vector3::UnitZ });

    mGame->SetCamera(mCamera);

    // ---------- OPENING CUTSCENE ----------
    ConfigureCutscene();

    // ---------- LIGHT ----------
    SetWorldLightIntensity(1.0f);

    // aditional light
    Actor* lightActor = new Actor(mGame);
    lightActor->SetPosition(Vector3(0.0f, 3500.0, 1500.0f));   // posição do ponto de luz
    PointLightComponent* pLight = new PointLightComponent(lightActor);
    pLight->SetColor(Vector3(0.55f, 0.50f, 0.70f));   // cor quente
    pLight->SetIntensity(0.3f);                   // força da luz
    pLight->SetRadius(20000.0f);                   // alcance da luz


    // ---------- ROBOTS ----------
    // Teste Setup do Player
    mPlayer->LoadFromJson("../Assets/Robots/Robota/Robota.json");
    mPlayer->SaveToJson("../Saves/PlayerBuild.json"); // Salvar build atual do player
    mPlayer->LoadFromJson("../Saves/PlayerBuild.json"); // Recarregar build
    mPlayer->LoadFromJson("../Assets/Robots/BeaBee/BeaBee.json");
    mPlayer->SetRotation(Vector3(0.0f, 0.0f, Math::ToRadians(-180.0f)));
    
    // Setup do Enemy - carrega do JSON
    mEnemy->LoadFromJson("../Assets/Robots/EvilRobota/EvilRobota.json");
    mEnemy->SetRotation(Vector3(0.0f, 0.0f, Math::ToRadians(90.0f)));

    // ---------- ARENA ----------
    mSkybox = new Skybox(mGame);
    mSkybox->SetTexture("../Assets/Textures/SkyboxLevel0.png");
    mSkybox->SetSize(15000.0f);

    // Carrega a Arena 0
    LevelConfig config;
    if (LoadLevelConfig("../Assets/Arena/Arena0/Arena0.json", config)) {
        LoadLevel(config);
    } else {
        SDL_Log("ERRO: Falha ao carregar Arena0!");
    }

    // ---------- CURSOR ----------
    // Posiciona cursor inicial
    if (mGrid) {
        MoveInGrid(mCursor, mPlayer->GetGridX(), mPlayer->GetGridY());
    }

    // ---------- IA ----------
    InitializeIA();

    // define level para garagem
    mGame->SetLastLevelCompleted(0);

    // start cutscene
    mCamera->StartCutscene();

    SDL_Log("\n\n========== LEVEL 0 INICIADO ========== ");
}

void Level0::ConfigureCutscene()
{
    std::vector<TransictionKeyframe> cutscenesKeyFrames;

    //Position (-761.0, 1450.7, 544.3) Target (2856.0, 1525.3, -644.9)
    //(-355.4, 759.6, 323.1) Target (2481.5, 3241.5, -219.9)
    cutscenesKeyFrames.push_back({
        { Vector3(-761.0f, 1450.7f, 544.3f),   Vector3(2856.0f, 1525.3f, -644.9f), Vector3::UnitZ },
        { Vector3(-355.4f, 759.6f, 323.1f),    Vector3(2481.5f, 3241.5f, -219.9f), Vector3::UnitZ },
        3.0f
    });


    //(-355.4, 759.6, 323.1) Target (2481.5, 3241.5, -219.9)
    //(-355.4, 759.6, 323.1) Target (2481.5, 3241.5, -219.9)
    cutscenesKeyFrames.push_back({
        { Vector3(-355.4f, 759.6f, 323.1f),    Vector3(2481.5f, 3241.5f, -219.9f), Vector3::UnitZ },
        { Vector3(-355.4f, 759.6f, 323.1f),    Vector3(2481.5f, 3241.5f, -219.9f), Vector3::UnitZ },
        2.0f
    });


    // (-1842.6, 827.9, 393.8) Target (-604.1, -2732.2, -149.1)
    // (-762.9, 85.2, 272.8) Target (-224.1, -3662.2, -138.2)
    cutscenesKeyFrames.push_back({
        { Vector3(-1842.6f, 827.9f, 393.8f),   Vector3(-604.1f, -2732.2f, -149.1f), Vector3::UnitZ },
        { Vector3(-762.9f, 85.2f, 272.8f),     Vector3(-224.1f, -3662.2f, -138.2f), Vector3::UnitZ },
        3.0f
    });


    // stand still
    //(-525.8, -990.7, 283.4) Target (-380.9, -4795.4, 203.9)
    //(-525.8, -990.7, 283.4) Target (-380.9, -4795.4, 203.9)
    cutscenesKeyFrames.push_back({
        { Vector3(-525.8f, -990.7f, 283.4f),   Vector3(-380.9f, -4795.4f, 203.9f), Vector3::UnitZ },
        { Vector3(-525.8f, -990.7f, 283.4f),   Vector3(-380.9f, -4795.4f, 203.9f), Vector3::UnitZ },
        3.0f
    });


    // (355.3, 1506.8, 300.6) Target (-3335.9, 1430.7, -633.4)
    // Position (-1.1f, 3548.0f, 1540.8f) Target (-11.7f, 191.7f, -258.5f)
    cutscenesKeyFrames.push_back({
        { Vector3(355.3f, 1506.8f, 300.6f),     Vector3(-3335.9f, 1430.7f, -633.4f), Vector3::UnitZ },
        { Vector3(-1.1f, 3548.0f, 1540.8f),     Vector3(-11.7f, 191.7f, -258.5f),    Vector3::UnitZ },
        3.0f
    });


    mCamera->SetCutsceneKeyframes(cutscenesKeyFrames);
}

void Level0::OnUpdate(float deltaTime)
{
    Level::OnUpdate(deltaTime);
}

void Level0::ProcessInput(SDL_Event &event)
{

}

