//
// Created by Lucas N. Ferreira on 06/11/25.
//

#include "Block.h"
#include "../Game.h"

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

}

void Block::SetTexture(const std::string &texturePath) {
    Texture* texture = mGame->GetRenderer()->GetTexture(texturePath);
    mMeshComponent->SetTextureOverride(texture);
}
