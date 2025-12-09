//
// Created by mateus on 29/11/2025.
//

#include "MainMenuScene.h"
#include "../UI/UIImage.h"

MainMenuScene::MainMenuScene(Game* game) : Scene(game)
{
    // Load background texture
    // mBackgroundImg = new UIImage(mGame, "../Assets/MenuBackground.png", Vector2(-500.0f, 0.0f), 1.0f, 0.0f, 200);

    CreateScene();

    // Add the UI screen on top
    mMenuUI = new MainMenu(game, "../Assets/Fonts/Arial.ttf");
}

MainMenuScene::~MainMenuScene()
{
    delete mMenuUI;

    for (Actor* a : mSceneActors)
    {
        if (a) a->SetState(ActorState::Destroy);
    }

    mSceneActors.clear();
}

void MainMenuScene::CreateScene()
{
    // TODO: deixar logo como uma textura em um ambiente 3D

    // ----- CÂMERA -----
    // Vector3 eye(1250.0f, -2354.2f, 1063.9f);
    // Vector3 target(1250.0f, -1267.4f, 801.7f);
    Vector3 eye(1250.0f, -1332.1f, 1302.1f);
    Vector3 target(1250.0f, -215.3f, 1252.1f);
    Vector3 up(0.0f, 0.0f, 1.0f);
    mCamera = new Camera(mGame, eye, target, up, 70.0f, 10.0f, 10000.0f);
    mGame->SetCamera(mCamera);

    mGame->GetRenderer()->SetWorldLightIntensity(1.0f);

    // ----- CHÃO -----
    int rows = 5;
    int cols = 6;

    float xPos = 0.0f;
    float yPos = 0.0f;
    float zPos = 0.0f;

    for (int y = 0; y < rows; y++)
    {
        for (int x = 0; x < cols; x++)
        {
            Block* block = new Block(mGame);
            block->SetScale(TILE_SCALE);
            block->SetPosition(Vector3(xPos, yPos, zPos));
            block->SetTexture("../Assets/Textures/floor_default.png");
            mSceneActors.push_back(block);

            xPos += TILE_SCALE.x;
        }
        xPos = 0.0f;
        yPos -= TILE_SCALE.y;
    }

    xPos = 0.0f;
    yPos = 0.0f;
    zPos = 0.0f;

    // ----- PAREDE -----
    for (int y = 0; y < rows; y++)
    {
        for (int x = 0; x < cols; x++)
        {
            Block* block = new Block(mGame);
            block->SetScale(TILE_SCALE);
            block->SetPosition(Vector3(xPos, yPos, zPos));
            block->SetTexture("../Assets/Textures/wall_default.png");
            mSceneActors.push_back(block);

            xPos += TILE_SCALE.x;
        }
        xPos = 0.0f;
        zPos += TILE_SCALE.z;
    }

    // ----- LATA DE LIXO -----
    Actor* trash = new Actor(mGame);
    MeshComponent* mcTrash = new MeshComponent(trash);
    Mesh* meshTrash = mGame->GetRenderer()->GetMesh("../Assets/Robots/Robota/RobotaTorso.gpmesh");
    mcTrash->SetMesh(meshTrash);

    trash->SetScale(Vector3(100.0f, 100.0f, 100.0f));
    // trash->SetRotation(Vector3(0.0f, 0.0f, Math::ToRadians(-90.0f)));
    trash->SetPosition(Vector3(1250.0f, -700.0f, 1000.0f));

    mSceneActors.push_back(trash);


    // // ----- VASSOURA -----
    // Actor* broom = new Actor(mGame);
    // MeshComponent* mcBroom = new MeshComponent(broom);
    // Mesh* broomMesh = mGame->GetRenderer()->GetMesh("../Assets/Robots/Robota/RobotaLeftArm.gpmesh");
    // mcBroom->SetMesh(broomMesh);
    //
    // broom->SetScale(Vector3(100.0f, 100.0f, 100.0f));
    // broom->SetRotation(Vector3(Math::ToRadians(70.0f), 0.0f, 0.0f));
    // broom->SetPosition(Vector3(2000.0f, -300.0f, 750.0f));
    //
    // mSceneActors.push_back(broom);
}


void MainMenuScene::Update(float deltaTime)
{
    // Call base class update to handle UI
    mMenuUI->HandleKeyPress(deltaTime);
}

void MainMenuScene::Draw()
{

}
