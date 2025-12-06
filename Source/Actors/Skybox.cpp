//
// Created by mateus on 05/12/2025.
//

#include "Skybox.h"
#include "Game.h"
#include "Components/Drawing/MeshComponent.h"

Skybox::Skybox(Game* game)
    : Actor(game)
{
    // Load cube mesh (must have inverted normals!)
    Mesh* mesh = game->GetRenderer()->GetMesh("../Assets/Arena/SkyboxArena.gpmesh");
    mMeshComponent = new MeshComponent(this);
    mMeshComponent->SetMesh(mesh);
    SetPosition(Vector3(0.0f, 0.0f, -2000.0f));
    SetSize(SIZE);

    // default skybox
    SetTexture("../Assets/Textures/SkyboxDefault.png");
}

Skybox::~Skybox()
{
}

void Skybox::SetTexture(const std::string& path)
{
    if (mMeshComponent) {
        Texture* texture = mGame->GetRenderer()->GetTexture(path);
        mMeshComponent->SetTextureOverride(texture);
    }
}

void Skybox::SetSize(float s)
{
    SetScale(Vector3(s, s, s));
}