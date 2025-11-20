//
// Created by Lucas N. Ferreira on 03/11/25.
//

#pragma once

#include "Block.h"
#include "../Components/Drawing/MeshComponent.h"
#include "../Components/Physics/AABBColliderComponent.h"

class BlockObstacle : public Block
{
public:
    BlockObstacle(class Game* game, const bool isExploding = false);
    ~BlockObstacle();

    // Set/Get Exploding state
    void SetExploding(const bool isExploding) { mIsExploding = isExploding; }
    bool IsExploding() const { return mIsExploding; }

    void Explode();

    // Get collider
    AABBColliderComponent* GetCollider() const { return mColliderComponent; }

private:
    AABBColliderComponent *mColliderComponent;
    bool mIsExploding;
};
