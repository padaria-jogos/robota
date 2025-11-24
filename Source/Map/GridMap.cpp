//
// Created by Heitor S. on 11/20/2025.
//

#include "GridMap.h"

#include <queue>

#include "../Actors/Tile.h"
#include "../Actors/Robot.h"


GridMap::GridMap(Game* game, int rows, int cols, float cellSize)
    : mGame(game), mRows(rows), mCols(cols), mCellSize(cellSize)
{
    float totalWidth = mCols * mCellSize;
    float totalHeight = mRows * mCellSize;

    // Mesma origem do SpawnWalls
    mStartPosition = Vector3(-totalWidth / 2.0f, -totalHeight / 2.0f, -250.0f);

    // Nenhum robô no começo
    mUnitsGrid.resize(mRows * mCols, nullptr);

    for (int i = 0; i < mRows; i++) {
        for (int j = 0; j < mCols; j++) {
            Tile* tile = new Tile(mGame);

            Vector3 pos = GetWorldPosition(j, i);
            pos.z += 5.0f;
            tile->SetPosition(pos);

            // O Plano do Blender é 2x2, precisamos dividir por 2
            float blenderFix = 0.5f;

            tile->SetScale(Vector3(mCellSize * blenderFix, mCellSize * blenderFix, 1.0f));

            mTiles.push_back(tile);
        }
    }
}


Vector3 GridMap::GetWorldPosition(int gridX, int gridY) const
{
    return Vector3(
        mStartPosition.x + (gridX * mCellSize) + (mCellSize * 0.5f),
        mStartPosition.y + (gridY * mCellSize) + (mCellSize * 0.5f),
        mStartPosition.z
    );
}

void GridMap::SetSelectedTile(int x, int y)
{
    // Apagar o highlight do antigo
    if (mSelectedX >= 0 && mSelectedX < mCols && mSelectedY >= 0 && mSelectedY < mRows) {
        int index = mSelectedY * mCols + mSelectedX;
        if (index < mTiles.size()) {
            mTiles[index]->SetSelected(false);
        }
    }

    mSelectedX = x;
    mSelectedY = y;

    // Acender novo highlight
    int index = mSelectedY * mCols + mSelectedX;
    if (index < mTiles.size()) {
        mTiles[index]->SetSelected(true);
    }
}

void GridMap::SetUnitAt(int x, int y, Robot* robot)
{
    if (x >= 0 && x < mCols && y >= 0 && y < mRows)
    {
        int index = y * mCols + x;
        mUnitsGrid[index] = robot;
    }
}

Robot* GridMap::GetUnitAt(int x, int y) const
{
    if (x >= 0 && x < mCols && y >= 0 && y < mRows)
    {
        int index = y * mCols + x;
        return mUnitsGrid[index];
    }
    return nullptr;
}

std::vector<TileNode> GridMap::GetWalkableTiles(int startX, int startY, int maxRange)
{
    std::vector<TileNode> results;

    std::vector<bool> visited(mRows * mCols, false);
    std::queue<TileNode> frontier;

    // Inicio onde o robô está (Distância 0)
    frontier.push({startX, startY, 0});
    visited[startY * mCols + startX] = true;

    // Cima, Baixo, Esq, Dir
    const int dirs[4][2] = { {0,1}, {0,-1}, {-1,0}, {1,0} };

    while (!frontier.empty())
    {
        TileNode current = frontier.front();
        frontier.pop();

        results.push_back(current);

        // Se já chegou na borda, não expande mais
        if (current.distance >= maxRange) continue;

        // Checar os 4 vizinhos
        for (auto& d : dirs)
        {
            int nx = current.x + d[0];
            int ny = current.y + d[1];

            // Está dentro do mapa?
            if (nx >= 0 && nx < mCols && ny >= 0 && ny < mRows)
            {
                int idx = ny * mCols + nx;

                if (!visited[idx])
                {
                    // É caminhável? (Não tem obstáculo/inimigo)
                    if (GetUnitAt(nx, ny) == nullptr)
                    {
                        visited[idx] = true;
                        frontier.push({nx, ny, current.distance + 1});
                    }
                }
            }
        }
    }
    return results;
}

void GridMap::ClearTileStates()
{
    for (auto* tile : mTiles) {
        tile->SetTileType(TileType::Default);
    }
}


Tile* GridMap::GetTileAt(int x, int y)
{
    if (x >= 0 && x < mCols && y >= 0 && y < mRows) {
        return mTiles[y * mCols + x];
    }
    return nullptr;
}

GridMap::~GridMap() {
    mTiles.clear();
    mUnitsGrid.clear();
}
