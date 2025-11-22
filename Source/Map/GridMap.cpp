//
// Created by Usuario on 11/20/2025.
//

#include "GridMap.h"
#include "../Actors/Tile.h"



GridMap::GridMap(Game* game, int rows, int cols, float cellSize)
    : mGame(game), mRows(rows), mCols(cols), mCellSize(cellSize)
{
    float totalWidth = mCols * mCellSize;
    float totalHeight = mRows * mCellSize;

    // Mesma origem do SpawnWalls
    mStartPosition = Vector3(-totalWidth / 2.0f, -totalHeight / 2.0f, -250.0f);

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
    // Apagar a luz do antigo
    if (mSelectedX >= 0 && mSelectedX < mCols && mSelectedY >= 0 && mSelectedY < mRows) {
        // mTiles é um vetor linear (1D), precisamos converter 2D -> 1D
        int index = mSelectedY * mCols + mSelectedX;
        if (index < mTiles.size()) {
            mTiles[index]->SetSelected(false);
        }
    }

    // Atualizar índices
    mSelectedX = x;
    mSelectedY = y;

    // Acender nova luz
    int index = mSelectedY * mCols + mSelectedX;
    if (index < mTiles.size()) {
        mTiles[index]->SetSelected(true);
    }
}

GridMap::~GridMap() {
    mTiles.clear();
}
