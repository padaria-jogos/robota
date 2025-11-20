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
#include "Actors/Ship.h"
#include "Actors/BlockObstacle.h"
#include "Camera.h"
#include "UI/Screens/HUD.h"
#include "UI/Screens/MainMenu.h"

Game::Game()
        :mWindow(nullptr)
        ,mRenderer(nullptr)
        ,mTicksCount(0)
        ,mIsRunning(true)
        ,mIsDebugging(false)
        ,mUpdatingActors(false)
        ,mShip(nullptr)
        ,mCamera(nullptr)
        ,mAudio(nullptr)
        ,mHUD(nullptr)
        ,mNextBlock(0)
        ,mNextObstacle(0)
        , mScore(0)
        , mNextScoreObstacle(0)
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
            // log controls
            SDL_Log("E/Q: forward/backward; W/S: up/down; A/D: right/left; SPACE: shoot");

            // hud
            mHUD = new HUD(this, "../Assets/Fonts/Arial.ttf");
            mHUD->SetScore(0);

            // create player
            mShip = new Ship(this);

            // create camera
            Vector3 eye(-300.0f, 0.0f, 0.0f);
            Vector3 target(20.0f, 0.0f, 0.0f);
            Vector3 up(0.0f, 0.0f, 1.0f);
            mCamera = new Camera(this, eye, target, up, 70.0f, 10.0f, 10000.0f);

            // spawn floor
            SpawnWalls();
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

void Game::SpawnWalls()
{
    // spawn the floor using multiple cubes
    const float spacing = 500.0f;
    const Vector3 center(mNextBlock * spacing, 0.0f, -500.0f);
    int n = 2; // size scale

    // construct a grid of n x n blocks centered at 'center'
    float offset = (n - 1) * 0.5f;

    for (int y = 0; y < n; y++)
    {
        for (int x = 0; x < n; x++)
        {
            Block* wall = new Block(this);
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

void Game::UpdateGame(float deltaTime)
{
    // Update all actors and pending actors
    UpdateActors(deltaTime);

    // update camera
    if (mCamera && mShip)
        mCamera->Update(deltaTime, mShip);


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