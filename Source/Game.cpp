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

#include <SDL_mixer.h>

#include "Random.h"
#include "Actors/Actor.h"
#include "Camera.h"

#include "UI/Screens/MainMenu.h"
#include "Scenes/MainMenuScene.h"
#include "Scenes/GarageScene.h"
#include "UI/Screens/HUD.h"
#include "UI/Screens/Credits.h"
#include "Levels/Level0.h"
#include "Levels/Level1.h"
#include "Levels/Level2.h"
#include "Levels/Level3.h"

Game::Game()
        :mWindow(nullptr)
        ,mRenderer(nullptr)
        ,mTicksCount(0)
        ,mIsRunning(true)
        ,mIsDebugging(false)
        ,mUpdatingActors(false)
        ,mCamera(nullptr)
        ,mAudio(nullptr)
        ,mHUD(nullptr)
        ,mLevel(nullptr)
        ,mLastLevelCompleted(-1)
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

    // window configuration
    mWindow = SDL_CreateWindow("ROBOTA", 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_OPENGL);
    if (!mWindow)
    {
        SDL_Log("Failed to create window: %s", SDL_GetError());
        return false;
    }
    mRenderer = new Renderer(mWindow);
    mRenderer->Initialize(WINDOW_WIDTH, WINDOW_HEIGHT);

    // Audio
    mAudio = new AudioSystem();

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
            new MainMenuScene(this);
        }
        break;

        case GameScene::Garage:
        {
            new GarageScene(this);
        }
        break;

        case GameScene::Level0:
        {
            delete mLevel;
            mHUD = new HUD(this, "../Assets/Fonts/Jersey10.ttf");
            mLevel = new Level0(this, mHUD); // se der problema definir destrutor level sem virtual e remover o do level1
        }
            break;

        case GameScene::Level1:
        {
            delete mLevel;
            mHUD = new HUD(this, "../Assets/Fonts/Jersey10.ttf");
            mLevel = new Level1(this, mHUD); // se der problema definir destrutor level sem virtual e remover o do level1
        }
        break;
        case GameScene::Level2:
        {
            delete mLevel;
            mHUD = new HUD(this, "../Assets/Fonts/Jersey10.ttf");
            mLevel = new Level2(this, mHUD); // se der problema definir destrutor level sem virtual e remover o do level1
        }
        break;

        case GameScene::Level3:
        {
            delete mLevel;
            mHUD = new HUD(this, "../Assets/Fonts/Jersey10.ttf");
            mLevel = new Level3(this, mHUD); // se der problema definir destrutor level sem virtual e remover o do level1
        }
        break;

        case GameScene::Credits:
        {
            delete mLevel;
            new Credits(this); // se der problema definir destrutor level sem virtual e remover o do level1
        }
        break;
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
                // Rotação da câmera com Q e E (processa ANTES de tudo)
                // if (event.key.keysym.sym == SDLK_q)
                // {
                //     mCamera->RotateLeft();
                //     break;  // Não processa mais nada
                // }
                // else if (event.key.keysym.sym == SDLK_e)
                // {
                //     mCamera->RotateRight();
                //     break;  // Não processa mais nada
                // }

                // handle key press camera
                if (mCamera)
                    mCamera->HandleKeyPress(event.key.keysym.sym);

                // Processa input nas UIs
                if (!mUIStack.empty()) {
                    mUIStack.back()->HandleKeyPress(event.key.keysym.sym);
                }

                // Processa input no level
                // Mas pula se for ESPAÇO e houver UI modal (evita processamento duplicado)
                bool hasModalUI = !mUIStack.empty() && mUIStack.back()->IsModal();
                bool isSpaceKey = event.key.keysym.sym == SDLK_SPACE;

                if (mLevel && !(hasModalUI && isSpaceKey))
                {
                    mLevel->ProcessInput(event);
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

void Game::UpdateGame(float deltaTime)
{
    // update camera
    if (mCamera)
        mCamera->Update(deltaTime);

    // Update all actors and pending actors
    UpdateActors(deltaTime);

    // Update UI screens
    for (auto ui : mUIStack) {
        if (ui->GetState() == UIScreen::UIState::Active) {
            ui->Update(deltaTime);
        }
    }

    mAudio->Update(deltaTime);

    // Delete any UI that are closed
    auto iter = mUIStack.begin();
    while (iter != mUIStack.end()) {
        if ((*iter)->GetState() == UIScreen::UIState::Closing) {
            delete *iter;
            iter = mUIStack.erase(iter);
        } else {
            ++iter;
        }
    }

    // Update level
    if (mLevel)
    {
        mLevel->OnUpdate(deltaTime);
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

    // Delete renderer
    mRenderer->Shutdown();
    delete mRenderer;
    mRenderer = nullptr;

    delete mAudio;
    mAudio = nullptr;

    SDL_DestroyWindow(mWindow);
    SDL_Quit();
}