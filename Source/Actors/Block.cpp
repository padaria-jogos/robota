//
// Created by Lucas N. Ferreira on 06/11/25.
//

#include "Block.h"
#include "../Game.h"

Block::Block(Game* game, float metallic)
        : Actor(game)
{
// load block mesh and set to mesh component
    Mesh* blockMesh = game->GetRenderer()->GetMesh("../Assets/Cube.gpmesh");

    // create block mesh
    mMeshComponent = new MeshComponent(this);
    mMeshComponent->SetMesh(blockMesh);
    
    // Define o valor metálico
    mMeshComponent->SetMetallic(metallic);
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
