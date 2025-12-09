//
// Created by Heitor S. on 11/22/2025.
//



#pragma once
#include "Actors/Actor.h"

enum class TileType {
    Default,    // Tile padrão
    Path,       // Overlay azul (indica movimento possível)
    Attack      // Overlay vermelho (indica ataque possível)
};

class Tile : public Actor {
    public:
        Tile(class Game* game);
        void SetTileType(TileType type);
        void SetSelected(bool selected);
        void SetVisible(bool visible);
        TileType GetType() const{ return mType; };

    private:
        void UpdateTexture();

        TileType mType;
        bool mIsSelected;
        class MeshComponent* mMeshComp;
};
