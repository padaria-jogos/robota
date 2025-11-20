//
// Created by Lucas N. Ferreira on 03/11/25.
//

#include "BlockObstacle.h"
#include "../Game.h"

BlockObstacle::BlockObstacle(class Game* game, const bool isExploding)
        :Block(game)
        ,mIsExploding(isExploding)
{
    // create collider component
    mColliderComponent = new AABBColliderComponent(this, 1.0f, 1.0f, 1.0f, Vector3::Zero, ColliderLayer::Block, true);

    // add obstacle to the global obstacle list
    game->AddObstacle(this);
}

BlockObstacle::~BlockObstacle()
{
    mGame->RemoveObstacle(this);
}

void BlockObstacle::Explode()
{
    // only explode if it's an exploding block
    if (GetState() == ActorState::Destroy)  return;

    SetState(ActorState::Destroy);

    // only continues if it's an exploding block
    if (!mIsExploding) return;

    auto& obstacles = mGame->GetObstacles();
    Vector3 position = GetPosition();

    // explode neighbours
    for (auto* neighbor : obstacles)
    {
        if (!neighbor) continue;                                        // null pointer
        if (neighbor->GetState() == ActorState::Destroy) continue;      // already destroyed
        if (neighbor == this) continue;                                 // dont explode itself

        float distance = (neighbor->GetPosition() - position).Length();

        if (distance <= 50.0f)
            neighbor->Explode();
    }
}