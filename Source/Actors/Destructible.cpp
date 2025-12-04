//
// Created by Heitor S. on 12/4/2025.
//

#include "Destructible.h"
#include "Game.h"
#include "Components/Drawing/MeshComponent.h"
#include "Renderer/Mesh.h"
#include "Renderer/Renderer.h"
#include "Components/Physics/AABBColliderComponent.h"

Destructible::Destructible(Game* game)
    : Actor(game)
    , mMeshComponent(nullptr)
{
    mMeshComponent = new MeshComponent(this);
}

void Destructible::SetMesh(Mesh* mesh)
{
    if (mMeshComponent) {
        mMeshComponent->SetMesh(mesh);
    }
}

void Destructible::SetTexture(const std::string& texturePath)
{
    if (mMeshComponent) {
        Texture* texture = mGame->GetRenderer()->GetTexture(texturePath);
        mMeshComponent->SetTextureOverride(texture);
    }
}

void Destructible::OnDestroy()
{
    // TODO: Adicionar efeitos de destruição
}
