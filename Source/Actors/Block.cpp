//
// Created by Lucas N. Ferreira on 06/11/25.
//

#include "Block.h"
#include "../Game.h"
#include "../Actors/Ship.h"

Block::Block(class Game* game)
        :Actor(game)
{
// load block mesh and set to mesh component
    Mesh* blockMesh = game->GetRenderer()->GetMesh("../Assets/Cube.gpmesh");

    // create block mesh
    mMeshComponent = new MeshComponent(this);
    mMeshComponent->SetMesh(blockMesh);
}

Block::~Block()
{
}

void Block::OnUpdate(float deltaTime)
{
    Ship* ship = mGame->GetPlayer();
    if (!ship)  return;

    float shipX  = ship->GetPosition().x;
    float blockX = GetPosition().x;

    // destroy block if it is far behind the ship
    if (blockX + 600.0f < shipX) // talvez aumentar esse valor
        SetState(ActorState::Destroy);
}