//
// Created by Heitor S. on 11/22/2025.
//



#pragma once
#include "Actors/Actor.h"

enum class TileType {
    Default,    // Chão normal
    Path,       // Azul (Movimento possível)
    Attack,     // Vermelho (Ataque possível)
    Wall        // Obstáculo
};

class Tile : public Actor {
    public:
        Tile(class Game* game);
        void SetTileType(TileType type);
        void SetSelected(bool selected);
        TileType GetType() const{ return mType; };

    private:
        void UpdateTexture();

        TileType mType;
        bool mIsSelected;
        class MeshComponent* mMeshComp;
};
