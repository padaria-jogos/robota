//
// Created by Heitor S. on 11/22/2025.
//



#pragma once
#include "Actors/Actor.h"

enum class TileType {
    Default,    // Padrão
    Path,       // Azul (Movimento possível)
    Attack      // Vermelho (Ataque possível)
};

class Tile : public Actor {
    public:
        Tile(class Game* game);
        void SetTileType(TileType type);
        void SetSelected(bool selected);
        TileType GetType() { return mType; };

    private:
        void UpdateTexture();


        TileType mType;
        bool mIsSelected;
        class MeshComponent* mMeshComp;
};
