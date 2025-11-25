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
        void OnKeyDown(int key);

        int GetGridX() const { return mGridX; }
        int GetGridY() const { return mGridY; }

        void Move(int xOffset, int yOffset);

    private:

        // void SelectSkill(PartSlot slot);
        void HandleAction();
        void HandleCancel();

        int mGridX;
        int mGridY;

        MeshComponent* mMesh;

        float mBaseHeight;
        float mAnimTimer;
};
