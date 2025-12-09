//
// Created by mateus on 09/12/2025.
//

#include "Garage.h"
#include "Actors/Robot.h"
#include "Actors/Block.h"

Garage::Garage(Game* game) : Scene(game)
{
    // create 3d scene
    CreateScene();

    // Add the UI screen on top
}

Garage::~Garage()
{
    // delete mMenuUI;

    for (Actor* a : mSceneActors)
    {
        if (a) a->SetState(ActorState::Destroy);
    }

    mSceneActors.clear();
}

void Garage::CreateScene()
{
    // ----- CÂMERA -----
    Vector3 eye(1738.4f, -1828.0f, 581.0f);
    Vector3 target(1248.5f, -823.6f, 550.5f);
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

    xPos = 0.0f;
    yPos = 0.0f;
    zPos = 0.0f;

    for (int y = 0; y < rows; y++)
    {
        for (int x = 1; x < cols; x++)
        {
            Block* block = new Block(mGame);
            block->SetScale(TILE_SCALE);
            block->SetPosition(Vector3(xPos, yPos, zPos));
            block->SetTexture("../Assets/Textures/wall_default.png");
            block->SetRotation(Vector3(Math::ToRadians(90.0f), 0.0f, 0.0f));
            mSceneActors.push_back(block);

            yPos -= TILE_SCALE.x;
        }
        yPos = 0.0f;
        zPos += TILE_SCALE.z;
    }


    // ----- ROBÔ -----
    Robot* robot = new Robot(mGame, Team::Player);

    // Carregar partes prontas
    robot->LoadFromJson("../Assets/Robots/Robota/Robota.json");

    // Definir posição fixa
    robot->SetPosition(Vector3(800.0f, -800.0f, 400.0f));
    robot->SetRotation(Vector3(0.0f, 0.0f, Math::ToRadians(-45.0f)));

    // Ajustar escala se quiser
    robot->SetScale(Vector3(60.0f, 60.0f, 60.0f));

    // Guardar pra destruir depois
    mSceneActors.push_back(robot);

    // ----- LUZ ADICIONAL -----
    Actor* lightActor = new Actor(mGame);
    lightActor->SetPosition(Vector3(1738.4f, -1828.0f, 581.0f));   // posição do ponto de luz
    PointLightComponent* pLight = new PointLightComponent(lightActor);
    pLight->SetColor(Vector3(0.55f, 0.50f, 0.70f));   // cor quente
    pLight->SetIntensity(0.3f);                   // força da luz
    pLight->SetRadius(2500.0f);                   // alcance da luz

    mSceneActors.push_back(lightActor);
}


void Garage::Update(float deltaTime)
{
    // Call base class update to handle UI
    // mMenuUI->HandleKeyPress(deltaTime);
}

void Garage::Draw()
{

}