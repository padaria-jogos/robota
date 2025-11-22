//
// Created by Usuario on 11/20/2025.
//

#pragma once
#include "math.h"
#include <vector>

class Tile;

class GridMap {
public:
    GridMap(class Game* game, int rows, int cols, float cellSize);
    ~GridMap();

    Vector3 GetWorldPosition(int gridX, int gridY) const;

    int GetRows() const { return mRows; }
    int GetCols() const { return mCols; }
    int GetSelectedX() const { return mSelectedX; }
    int GetSelectedY() const { return mSelectedY; }

    float GetCellSize() const { return mCellSize; }


    void SetSelectedTile(int x, int y);


private:
    class Game* mGame;
    std::vector<Tile*> mTiles;

    int mRows;
    int mCols;
    float mCellSize;
    float mSelectedX;
    float mSelectedY;
    Vector3 mStartPosition;
};

