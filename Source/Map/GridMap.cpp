//
// Created by Heitor S. on 11/20/2025.
//

#include "GridMap.h"
#include <queue>
#include <iostream>

GridMap::GridMap(Game* game, int rows, int cols, float cellSize)
    : mGame(game), mRows(rows), mCols(cols), mCellSize(cellSize)
{
    float totalWidth = mCols * mCellSize;
    float totalHeight = mRows * mCellSize;

    // Mesma origem do SpawnWalls
    mStartPosition = Vector3(-totalWidth / 2.0f, -totalHeight / 2.0f, -250.0f);

    // Nenhum robô no começo
    mUnitsGrid.resize(mRows * mCols, nullptr);

    // definir os tiles em cada bloco
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

    // teste
    SetSelectedTile(1, 1);

    // verificar quais tiles devem ser pintados
    for (auto &p : GetWalkableTiles(1, 1, 1))
    {
        std::cout << p.x << "," << p.y << " ";
    }
    std::cout << std::endl;

    // ataque
    for (auto &p : GetAttackableTiles(1, 1, 1, 1))
    {
        std::cout << p.x << "," << p.y << " ";
    }
    std::cout << std::endl;
}

Vector3 GridMap::GetWorldPosition(int gridX, int gridY) const
{
    return Vector3(
        mStartPosition.x + (gridX * mCellSize) + (mCellSize * 0.5f),
        mStartPosition.y + (gridY * mCellSize) + (mCellSize * 0.5f),
        mStartPosition.z
    );
}

Vector3 GridMap::GetTilePosition(int worldX, int worldY) const
{
    float worldXF = static_cast<float>(worldX);
    float worldYF = static_cast<float>(worldY);

    int gridX = static_cast<int>((worldXF - mStartPosition.x - (mCellSize * 0.5f)) / mCellSize);
    int gridY = static_cast<int>((worldYF - mStartPosition.y - (mCellSize * 0.5f)) / mCellSize);

    return Vector3(static_cast<float>(gridX), static_cast<float>(gridY), 0.0f);
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
                    // TODO: reimplementar no futuro
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

std::vector<TileNode> GridMap::GetAttackableTiles(int startX, int startY, int minRange, int maxRange)
{
    std::vector<TileNode> results;

    // Arredores do robo
    int minX = std::max(0, startX - maxRange);
    int maxX = std::min(mCols - 1, startX + maxRange);
    int minY = std::max(0, startY - maxRange);
    int maxY = std::min(mRows - 1, startY + maxRange);

    for (int y = minY; y <= maxY; ++y) {
        for (int x = minX; x <= maxX; ++x) {

            // Distância de Manhattan (|dx| + |dy|)
            int dist = std::abs(x - startX) + std::abs(y - startY);

            // Verifica se está dentro da rosqiunha do alcance
            if (dist >= minRange && dist <= maxRange) {
                results.push_back({x, y, dist});
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


// TODO: testar depois
Tile* GridMap::GetTileAt(int x, int y)
{
    if (x >= 0 && x < mCols && y >= 0 && y < mRows) {
        return mTiles[y * mCols + x];
    }
    return nullptr;
}

Actor* GridMap::GetUnitAt(int x, int y)
{
    if (x >= 0 && x < mCols && y >= 0 && y < mRows)
    {
        int index = y * mCols + x;
        return mUnitsGrid[index];
    }
    return nullptr;
}

void GridMap::SetUnitAt(Actor* actor, int x, int y)
{
    for (auto &unit : mUnitsGrid) {
        if (unit == actor) {
            unit = nullptr;
            break;
        }
    }

    // substituo pelo novo
    if (x >= 0 && x < mCols && y >= 0 && y < mRows)
    {
        int index = y * mCols + x;
        mUnitsGrid[index] = actor;
    }
}

GridMap::~GridMap()
{
    mTiles.clear();
}