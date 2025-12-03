//
// Created by Lucas N. Ferreira on 06/11/25.
//

#pragma once

#include "Actor.h"
#include "../Components/Drawing/MeshComponent.h"
#include "../Components/Physics/AABBColliderComponent.h"

class Block : public Actor
{
public:
    Block(class Game* game);
    ~Block();

    void SetTexture(const size_t textureId) const { mMeshComponent->SetTextureIndex(textureId); }
    void SetTexture(const std::string& texturePath);

    void OnUpdate(float deltaTime) override;

private:
    MeshComponent *mMeshComponent;
    AABBColliderComponent *mColliderComponent;
};


