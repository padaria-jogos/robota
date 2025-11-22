//
// Created by Heitor S. on 11/22/2025.
//


#include "Tile.h"
#include "../Game.h"
#include "Components/Drawing/MeshComponent.h"

Tile::Tile(Game* game)
    : Actor(game)
{
    mMeshComp = new MeshComponent(this);

    Mesh* mesh = game->GetRenderer()->GetMesh("../Assets/Tile.gpmesh");
    mMeshComp->SetMesh(mesh);

}

void Tile::SetSelected(bool selected) {
    if (selected) {
        Texture* selectTex = GetGame()->GetRenderer()->GetTexture("../Assets/TileSelected.png");
        mMeshComp->SetTextureOverride(selectTex);
    }else {
        mMeshComp->SetTextureOverride(nullptr);
    }

}

