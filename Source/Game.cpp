// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
//
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include <algorithm>
#include <vector>
#include <fstream>
#include "Game.h"
#include "Random.h"
#include "Actors/Actor.h"
#include "Actors/BlockObstacle.h"
#include "Camera.h"
#include "Actors/GridCursor.h"
#include "Actors/Robot.h"
#include "UI/Screens/HUD.h"
#include "UI/Screens/MainMenu.h"

Game::Game()
        :mWindow(nullptr)
        ,mRenderer(nullptr)
        ,mTicksCount(0)
        ,mIsRunning(true)
        ,mIsDebugging(false)
        ,mUpdatingActors(false)
        ,mCursor(nullptr)
        ,mCamera(nullptr)
        ,mAudio(nullptr)
        ,mHUD(nullptr)
{

}

bool Game::Initialize()
{
    Random::Init();

    // video init
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
        return false;
    }

    // init SDL Image
    int imgFlags = IMG_INIT_PNG;
    if (!(IMG_Init(imgFlags) & imgFlags))
    {
        SDL_Log("Unable to initialize SDL_image: %s", IMG_GetError());
        return false;
    }

    // initialize SDL_ttf
    if (TTF_Init() != 0)
    {
        SDL_Log("Failed to initialize SDL_ttf");
        return false;
    }

    // initialize SDL_mixer
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) == -1)
    {
        SDL_Log("Failed to initialize SDL_mixer: %s", Mix_GetError());
        return false;
    }

    // audio debug
    // int n = SDL_GetNumAudioDrivers();
    // SDL_Log("Audio drivers available:");
    // for (int i = 0; i < n; i++) {
    //     SDL_Log("  %s", SDL_GetAudioDriver(i));
    // }
    // SDL_Log("Current audio driver: %s", SDL_GetCurrentAudioDriver());

    // window configuration
    mWindow = SDL_CreateWindow("ROBOTA", 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_OPENGL);
    if (!mWindow)
    {
        SDL_Log("Failed to create window: %s", SDL_GetError());
        return false;
    }
    mRenderer = new Renderer(mWindow);
    mRenderer->Initialize(WINDOW_WIDTH, WINDOW_HEIGHT);


    mGrid = nullptr;
    // start at main menu
    SetScene(GameScene::MainMenu);
    mTicksCount = SDL_GetTicks();

    return true;
}

void Game::UnloadScene()
{
    // Use state so we can call this from withing an a actor update
    for(auto *actor : mActors) {
        actor->SetState(ActorState::Destroy);
    }

    // Delete UI screens
    for (auto ui : mUIStack) {
        delete ui;
    }

    mUIStack.clear();
    mHUD = nullptr;
}

void Game::SetScene(GameScene nextScene)
{
    // unload current scene
    UnloadScene();

    // change scene
    switch (nextScene)
    {
        case GameScene::MainMenu:
        {
            new MainMenu(this, "../Assets/Fonts/Arial.ttf");
        }
        break;

        case GameScene::Level1:
        {
            mBattleState = BattleState::Exploration;
            mSelectedUnit = nullptr;

            // const
            const int ROWS = 4;
            const int COLS = 4;
            const float SIZE = 500.0f;

            // log controls
            SDL_Log("E/Q: forward/backward; W/S: up/down; A/D: right/left; SPACE: shoot");

            // hud
            // mHUD = new HUD(this, "../Assets/Fonts/Arial.ttf");
            // mHUD->SetScore(0);


            // spawn floor
            SpawnWalls(ROWS, COLS);


            // Teste grid
            mGrid = new GridMap(this, ROWS, COLS, SIZE);

            // teste robos
            Robot* playerUnit = new Robot(this, Team::Player);
            playerUnit->SetName("CryingBee");
            playerUnit->EquipPart(PartSlot::RightArm,
                                    RobotPart("Iron Fist", "../Assets/Cube.gpmesh",
                                        50, SkillType::Punch, 30, 1));

            playerUnit->EquipPart(PartSlot::LeftArm,
                                    RobotPart("Thunder Beam", "../Assets/Cube.gpmesh",
                                        50, SkillType::Missile, 40, 3));

            playerUnit->SetGridPosition(1, 1);

            Robot* enemyUnit = new Robot(this, Team::Enemy);
            enemyUnit->SetName("EvilBee");
            enemyUnit->EquipPart(PartSlot::Torso,
                                    RobotPart("Heavy Armor", "../Assets/Cube.gpmesh",
                                        100, SkillType::None, 0, 0));
            enemyUnit->SetGridPosition(3, 3);

            mCursor = new GridCursor(this);

            // create camera
            Vector3 eye(0.0f, -2500.0f, 1000.0f);
            Vector3 target(0.0f, 0.0f, 0.0f);
            Vector3 up(0.0f, 0.0f, 1.0f);
            mCamera = new Camera(this, eye, target, up, 70.0f, 10.0f, 10000.0f);
        }
    }
}

void Game::RunLoop()
{
    while (mIsRunning)
    {
        // Calculate delta time in seconds
        float deltaTime = (SDL_GetTicks() - mTicksCount) / 1000.0f;
        if (deltaTime > 0.05f)
        {
            deltaTime = 0.05f;
        }

        mTicksCount = SDL_GetTicks();

        ProcessInput();
        UpdateGame(deltaTime);
        GenerateOutput();

        // Sleep to maintain frame rate
        int sleepTime = (1000 / FPS) - (SDL_GetTicks() - mTicksCount);
        if (sleepTime > 0)
        {
            SDL_Delay(sleepTime);
        }
    }
}

void Game::LoadObstaclePatterns(const std::string& dirName, const int nBlockPatterns)
{

}

void Game::ProcessInput()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
            case SDL_QUIT:
                Quit();
                break;
            case SDL_KEYDOWN:
                // Handle key press for UI screens
                if (!mUIStack.empty()) {
                    mUIStack.back()->HandleKeyPress(event.key.keysym.sym);
                }

                if (mCursor) {
                    mCursor->OnKeyDown(event.key.keysym.sym);
                }

                break;
        }
    }

    const Uint8* state = SDL_GetKeyboardState(nullptr);

    for (auto actor : mActors)
    {
        actor->ProcessInput(state);
    }
}

void Game::SpawnObstacles()
{

}

void Game::SpawnWalls(int rows, int cols)
{
    //SDL_Log("SPAWNWALLS -> Rows: %d, Cols: %d", rows, cols);
    const float spacing = 500.0f;

    float totalWidth = cols * spacing;
    float totalHeight = rows * spacing;

    float startX = -totalWidth / 2.0f;
    float startY = -totalHeight / 2.0f;

    float zPos = -500.0f; // Centro do cubo

    for (int y = 0; y < rows; y++)
    {
        for (int x = 0; x < cols; x++)
        {
            Block* wall = new Block(this);
            wall->SetScale(Vector3(spacing, spacing, spacing)); // cubo tamanho 1x1 -> vira 500x500

            Vector3 pos;
            // Move o ponto da "borda" para o "centro" do bloco
            pos.x = startX + (x * spacing) + (spacing * 0.5f);
            pos.y = startY + (y * spacing) + (spacing * 0.5f);
            pos.z = zPos;

            wall->SetPosition(pos);
            wall->SetTexture(0);
        }
    }
}

void Game::UpdateGame(float deltaTime)
{
    // Update all actors and pending actors
    UpdateActors(deltaTime);


    // Update UI screens
    for (auto ui : mUIStack) {
        if (ui->GetState() == UIScreen::UIState::Active) {
            ui->Update(deltaTime);
        }
    }

    // Delete any UI that are closed
    auto iter = mUIStack.begin();
    while (iter != mUIStack.end()) {
        if ((*iter)->GetState() == UIScreen::UIState::Closing) {
            iter = mUIStack.erase(iter);
        } else {
            ++iter;
        }
    }
}

void Game::UpdateActors(float deltaTime)
{
    mUpdatingActors = true;
    for (auto actor : mActors)
    {
        actor->Update(deltaTime);
    }
    mUpdatingActors = false;

    for (auto pending : mPendingActors)
    {
        mActors.emplace_back(pending);
    }
    mPendingActors.clear();

    std::vector<Actor*> deadActors;
    for (auto actor : mActors)
    {
        if (actor->GetState() == ActorState::Destroy)
        {
            deadActors.emplace_back(actor);
        }
    }

    for (auto actor : deadActors)
    {
        delete actor;
    }
}

void Game::AddActor(Actor* actor)
{
    if (mUpdatingActors)
    {
        mPendingActors.emplace_back(actor);
    }
    else
    {
        mActors.emplace_back(actor);
    }
}

void Game::RemoveActor(Actor* actor)
{
    auto iter = std::find(mPendingActors.begin(), mPendingActors.end(), actor);
    if (iter != mPendingActors.end())
    {
        // Swap to end of vector and pop off (avoid erase copies)
        std::iter_swap(iter, mPendingActors.end() - 1);
        mPendingActors.pop_back();
    }

    iter = std::find(mActors.begin(), mActors.end(), actor);
    if (iter != mActors.end())
    {
        // Swap to end of vector and pop off (avoid erase copies)
        std::iter_swap(iter, mActors.end() - 1);
        mActors.pop_back();
    }
}

void Game::AddObstacle(BlockObstacle* obstacle)
{

}

void Game::RemoveObstacle(BlockObstacle* obstacle)
{

}

void Game::GenerateOutput()
{
    // Clear back buffer
    mRenderer->Clear();

    // Draw all drawables
    mRenderer->Draw();

    // Swap front buffer and back buffer
    mRenderer->Present();
}

void Game::Shutdown()
{
    // Because ~Actor calls RemoveActor, have to use a different style loop
    while (!mActors.empty()) {
        delete mActors.back();
    }

    // Delete UI screens
    for (auto ui : mUIStack) {
        delete ui;
    }
    mUIStack.clear();

    // Delete obstacle patterns
    for (auto pattern : mObstaclePatterns) {
        for (auto obstacle : pattern) {
            delete obstacle;
        }
    }
    mObstaclePatterns.clear();

    // Delete renderer
    mRenderer->Shutdown();
    delete mRenderer;
    mRenderer = nullptr;

    delete mAudio;
    mAudio = nullptr;

    SDL_DestroyWindow(mWindow);
    SDL_Quit();
}
