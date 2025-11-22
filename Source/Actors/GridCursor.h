//
// Created by Heitor S. on 11/22/2025.
//

#pragma once
#include "Actor.h"

class GridCursor : public Actor{
    public:
        GridCursor(class Game* game);


        void OnProcessInput(const Uint8* keyState) override;
        void OnUpdate(float deltaTime) override;
        void OnKeyDown(int key);

        void Move(int xOffset, int yOffset);

        int GetGridX() const { return mGridX; }
        int GetGridY() const { return mGridY; }

    private:
        int mGridX;
        int mGridY;

        float mBaseHeight;
        float mAnimTimer;

};