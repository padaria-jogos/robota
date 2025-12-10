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

    SDL_Log("\n\n========== LEVEL 0 INICIADO ========== ");
}

void Level0::ConfigureCutscene()
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

void Level0::OnUpdate(float deltaTime)
{
    Level::OnUpdate(deltaTime);
}

void Level0::ProcessInput(SDL_Event &event)
{

}

