//
// Created by Heitor S. on 11/20/2025.
//

#include "GridMap.h"

#include <algorithm>
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
    
    // Inicializa tipos de terreno como Floor
    mTerrainTypes.resize(mRows * mCols, TerrainType::Floor);
    
    // Inicializa referências de blocks como nullptr
    mFloorBlocks.resize(mRows * mCols, nullptr);

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

    // Seleciona tile inicial apenas se o grid não estiver vazio
    if (mRows > 0 && mCols > 0) {
        SetSelectedTile(0, 0);
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
                    // É caminhável? (Não tem obstáculo/inimigo)
                    if (GetUnitAt(nx, ny) == nullptr && IsWalkable(nx, ny))
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

std::vector<Vector2> GridMap::CalculatePath(int startX, int startY, int endX, int endY) {
    std::unordered_map<int, int> cameFrom; // Guardar o caminho
    std::queue<int> frontier;

    int startIdx = startY * mCols + startX;
    int endIdx = endY * mCols + endX;

    frontier.push(startIdx);
    cameFrom[startIdx] = startIdx;

    bool found = false;

    while (!frontier.empty()) {
        int currentIdx = frontier.front();
        frontier.pop();

        if (currentIdx == endIdx) {
            found = true;
            break;
        }

        int curX = currentIdx % mCols;
        int curY = currentIdx / mCols;

        const int dirs[4][2] = { {0, 1}, {0, -1}, {-1, 0}, {1, 0} };

        for (auto& d : dirs) {
            int nx = curX + d[0];
            int ny = curY + d[1];

            if (nx >= 0 && nx < mCols && ny >= 0 && ny < mRows) {
                int idx = ny * mCols + nx;
                if (cameFrom.find(idx) == cameFrom.end()) {
                    // Tile deve ser caminhável OU ser o destino (se o destino também for caminhável)
                    bool isWalkable = IsWalkable(nx, ny);
                    bool hasNoUnit = (GetUnitAt(nx, ny) == nullptr);
                    bool isDestination = (idx == endIdx);

                    // Permite passar por tiles com unidades apenas se for o destino
                    // Mas não permite passar por paredes :p
                    if (isWalkable && (hasNoUnit || isDestination))
                    {
                        frontier.push(idx);
                        cameFrom[idx] = currentIdx; // Cheguei no idx vindo do currentIdx
                    }
                }
            }
        }

    }

    std::vector<Vector2> path;
    if (!found) {
        return path;
    }

    int curr = endIdx;
    while (curr != startIdx)
    {
        float px = static_cast<float>(curr % mCols);
        float py = static_cast<float>(curr / mCols);
        path.push_back(Vector2(px, py));
        curr = cameFrom[curr];
    }
    std::reverse(path.begin(), path.end());
    return path;
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
    // Apenas limpa os overlays visuais temporários (Path e Attack)
    for (auto* tile : mTiles) {
        TileType currentType = tile->GetType();
        if (currentType == TileType::Path || currentType == TileType::Attack) {
            tile->SetTileType(TileType::Default);
        }
    }
}


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
    if (actor != nullptr) {
        for (auto &unit : mUnitsGrid) {
            if (unit == actor) {
                unit = nullptr;
                break;
            }
        }
    }

    // substituo pelo novo
    if (x >= 0 && x < mCols && y >= 0 && y < mRows)
    {
        int index = y * mCols + x;
        mUnitsGrid[index] = actor;
    }
}

bool GridMap::IsWalkable(int x, int y) const
{
    if (x < 0 || x >= mCols || y < 0 || y >= mRows) {
        return false;
    }
    
    // Verifica o tipo de terreno
    int idx = y * mCols + x;
    TerrainType terrain = mTerrainTypes[idx];
    
    // Apenas paredes são intransponíveis
    // Mel, fogo e outros hazards são caminháveis, mas aplicam efeitos depois
    return terrain != TerrainType::Wall;
}

void GridMap::SetTerrainType(int x, int y, TerrainType type)
{
    if (x >= 0 && x < mCols && y >= 0 && y < mRows) {
        int idx = y * mCols + x;
        mTerrainTypes[idx] = type;
    }
}

TerrainType GridMap::GetTerrainType(int x, int y) const
{
    if (x >= 0 && x < mCols && y >= 0 && y < mRows) {
        int idx = y * mCols + x;
        return mTerrainTypes[idx];
    }
    return TerrainType::Floor;
}

void GridMap::SetFloorBlock(int x, int y, Actor* block)
{
    if (x >= 0 && x < mCols && y >= 0 && y < mRows) {
        int idx = y * mCols + x;
        mFloorBlocks[idx] = block;
    }
}

Actor* GridMap::GetFloorBlock(int x, int y) const
{
    if (x >= 0 && x < mCols && y >= 0 && y < mRows) {
        int idx = y * mCols + x;
        return mFloorBlocks[idx];
    }
    return nullptr;
}

GridMap::~GridMap()
{
    // Deletar todos os Tiles antes de limpar o vetor
    for (auto* tile : mTiles) {
        if (tile) {
            delete tile;
        }
    }
    mTiles.clear();
}