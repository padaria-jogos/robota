//
// Created by Usuario on 11/20/2025.
//

/*
 * GridMap
 * Define a estrutura e de gerenciamento de grid
 */

#pragma once

#include <vector>
#include "Math.h"
#include "Actors/Tile.h"

// Vertices BFS
struct TileNode {
    int x, y;
    int distance;
};

class GridMap {
public:
    GridMap(class Game* game, int rows, int cols, float cellSize);
    ~GridMap();

    Vector3 GetWorldPosition(int gridX, int gridY) const;
    Vector3 GetTilePosition(int worldX, int worldY) const;

    int GetRows() const { return mRows; }
    int GetCols() const { return mCols; }
    int GetSelectedX() const { return mSelectedX; }
    int GetSelectedY() const { return mSelectedY; }
    float GetCellSize() const { return mCellSize; }

    //BFS Walk
    std::vector<TileNode> GetWalkableTiles(int startX, int startY, int maxRange);

    //Attack Range
    std::vector<TileNode> GetAttackableTiles(int startX, int startY, int minRange, int maxRange);

    // void SetUnitAt(int x, int y, Robot* robot);
    Actor* GetUnitAt(int x, int y);
    Tile* GetTileAt(int x, int y);

    void SetUnitAt(Actor* actor, int x, int y);
    void SetSelectedTile(int x, int y);
    void ClearTileStates();


private:
    class Game* mGame;
    std::vector<Tile*> mTiles;
    std::vector<Actor*> mUnitsGrid;

    int mRows;
    int mCols;
    float mCellSize;
    float mSelectedX;
    float mSelectedY;
    Vector3 mStartPosition;
};

