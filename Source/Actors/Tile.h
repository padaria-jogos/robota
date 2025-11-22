//
// Created by Heitor S. on 11/22/2025.
//



#pragma once
#include "Actors/Actor.h"

class Tile : public Actor {
    public:
        Tile(class Game* game);
        void SetSelected(bool selected);

    private:
        class MeshComponent* mMeshComp;

};
