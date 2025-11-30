//
// Created by Heitor S. on 11/22/2025.
//


#include "Tile.h"
#include "../Game.h"
#include "Components/Drawing/MeshComponent.h"

Tile::Tile(Game* game)
    : Actor(game)
    , mIsSelected(false)
    , mType(TileType::Default)
{
    mMeshComp = new MeshComponent(this);
    Mesh* mesh = game->GetRenderer()->GetMesh("../Assets/Tile.gpmesh");
    mMeshComp->SetMesh(mesh);

    UpdateTexture();
}

void Tile::SetTileType(TileType type)
{
    if (mType != type) {
        mType = type;
        UpdateTexture();
    }
}

void Tile::SetSelected(bool selected)
{
    if (mIsSelected != selected) {
        mIsSelected = selected;
        UpdateTexture();
    }
}

void Tile::UpdateTexture()
{
    std::string textureName = "";
    if (mIsSelected)
    {
        switch (mType) {
            case TileType::Path:
                textureName = "../Assets/TileSelectBlue.png";
                break;
            case TileType::Attack:
                textureName = "../Assets/TileSelectRed.png";
                break;
            default: // Default
                textureName = "../Assets/TileSelectStd.png";
                break;
        }
    }
    else
    {
        switch (mType) {
            case TileType::Path:
                textureName = "../Assets/TileBlue.png";
                break;
            case TileType::Attack:
                textureName = "../Assets/TileRed.png";
                break;
            default: // Default
                textureName = "../Assets/TileGrid.png";
                break;
        }
    }

    // Aplica a textura
    auto* renderer = GetGame()->GetRenderer();
    if (!textureName.empty()) {
        mMeshComp->SetTextureOverride(renderer->GetTexture(textureName));
    } else {
        mMeshComp->SetTextureOverride(nullptr);
    }
}
