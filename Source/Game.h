// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once

#include <SDL.h>
#include <vector>
#include "Renderer/Renderer.h"
#include "AudioSystem.h"

#include "Levels/Level.h"

enum class GameScene
{
    MainMenu,
    Garage,
    Level0,
    Level1,
    Level2
};

class Game
{
public:
    Game();

    bool Initialize();
    void RunLoop();
    void Shutdown();
    void Quit() { mIsRunning = false; }

    // Actor functions
    void UpdateActors(float deltaTime);
    void AddActor(class Actor* actor);
    void RemoveActor(class Actor* actor);

    // UI functions
    void PushUI(class UIScreen* screen) { mUIStack.emplace_back(screen); }
    const std::vector<class UIScreen*>& GetUIStack() { return mUIStack; }

    // Audio
    AudioSystem* GetAudio() { return mAudio; }

    // Scene Handling
    void SetScene(GameScene scene);
    void UnloadScene();

    // Getters
    class Renderer* GetRenderer() { return mRenderer; }
    class HUD* GetHUD() { return mHUD; }
    class Camera* GetCamera() { return mCamera; }
    class Level* GetLevel() { return mLevel; }

    // Setters
    void SetCamera(class Camera* camera) { mCamera = camera; }
    void SetLevel(class Level* level) { mLevel = level; }

    // Constants
    static const int WINDOW_WIDTH   = 1024;
    static const int WINDOW_HEIGHT  = 768;
    static const int TUNNEL_DEPTH   = 10;
    static const int WALL_SCALE     = 500;
    static const int OBSTACLE_SCALE = 25;
    static const int OBSTACLE_PATTERN_SIZE     = 20;
    static const int OBSTACLE_PATTERN_GAP      = 1000;
    static const int OBSTACLES_WAVES_IN_SCREEN = 3;
    static const int FPS = 60;

private:
    void ProcessInput();
    void UpdateGame(float deltaTime);
    void GenerateOutput();

    // All the actors in the game
    std::vector<class Actor*> mActors;
    std::vector<class Actor*> mPendingActors;

    // All UI screens in the game
    std::vector<class UIScreen*> mUIStack;

    // Rendeding stuff
    SDL_Window* mWindow;
    class Renderer* mRenderer;

    // Audio system
    AudioSystem* mAudio;

    // Track elapsed time since game start
    Uint32 mTicksCount;

    // Track if we're updating actors right now
    bool mIsRunning;
    bool mIsDebugging;
    bool mUpdatingActors;

    // Game-specific
    class Camera *mCamera;
    class HUD *mHUD;

    Level* mLevel;

    // List of obstacle patterns
    // std::vector<class BlockObstacle*> mObstacles;


};
