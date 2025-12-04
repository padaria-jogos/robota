//
// Created by Heitor S. on 11/22/2025.
//

#include "GridCursor.h"

#include "Game.h"
// #include "Robot.h"

GridCursor::GridCursor(Game* game) : Actor(game)
        , mGridX(0)
        , mGridY(0)
        , mAnimTimer(0.0f)
        , mBaseHeight(500.0f)
        , mMesh(nullptr)
{
    mMesh = new MeshComponent(this);
    Mesh* mesh = mGame->GetRenderer()->GetMesh("../Assets/Cursor.gpmesh");
    mMesh->SetMesh(mesh);
    
    // Cursor é unlit (sem efeitos de iluminação)
    mMesh->SetUnlit(true);

    SetScale(Vector3(100.0f, 100.0f, 100.0f));
    SetPosition(Vector3(0, 0, 150));    // sample position
    SetRotation(Vector3(Math::ToRadians(180.0f), 0.0f, 0.0f));
}


void GridCursor::OnUpdate(float deltaTime)
{
    // Animação de Flutuar
    mAnimTimer += deltaTime;
    Vector3 pos = GetPosition();
    float bobbing = std::sin(mAnimTimer * 5.0f) * 20.0f;
    pos.z = mBaseHeight + bobbing;
    SetPosition(pos);
}

void GridCursor::OnProcessInput(const uint8_t* state)
{

}

void GridCursor::UpdateGridCoords(int x, int y) {
    mGridX = x;
    mGridY = y;
}
