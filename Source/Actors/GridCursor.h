//
// Created by Heitor S. on 11/22/2025.
//

/*
 * GridCursor
 * Define um objeto 3D em formato de cursor
 */

#pragma once

// #include "Game.h"
#include "Actors/Actor.h"
#include "Components/Drawing/MeshComponent.h"

class GridCursor : public Actor {
    public:
        GridCursor(class Game* game);


        void OnProcessInput(const Uint8* keyState) override;
        void OnUpdate(float deltaTime) override;

        int GetGridX() const { return mGridX; }
        int GetGridY() const { return mGridY; }
        void UpdateGridCoords(int x, int y);


    private:
        int mGridX;
        int mGridY;

        MeshComponent* mMesh;

        float mBaseHeight;
        float mAnimTimer;
};
