//
// Created by Murilo on 11/29/2025.
//

#include "AI.h"

AI::AI(GridMap* grid,
       int enemyX,
       int enemyY,
       const std::vector<SkillType>& enemyAttacks,
       const std::vector<SkillType>& playerAttacks)
    : mGrid(grid),
      mEnemyX(enemyX),
      mEnemyY(enemyY),
      mEnemySkills(enemyAttacks),
      mPlayerSkills(playerAttacks)
{
    mapvalues();
}


void AI::move(int ex, int ey, int px, int py) {

    // Tenta mover 1 casa na direção do player
    int dx = (px > ex) ? 1 : (px < ex ? -1 : 0);
    int dy = (py > ey) ? 1 : (py < ey ? -1 : 0);

    int nextX = ex;
    int nextY = ey;

    if (std::abs(px - ex) > std::abs(py - ey)) {
        nextX += dx;
    } else {
        nextY += dy;
    }

    nextX = std::clamp(nextX, 0, mGrid->GetCols() - 1);
    nextY = std::clamp(nextY, 0, mGrid->GetRows() - 1);

    Actor* obstacle = mGrid->GetUnitAt(nextX, nextY);
    if (obstacle != nullptr && obstacle != mEnemy) {
        // Se estiver bloqueado, cancela o movimento e fica onde está
        nextX = ex;
        nextY = ey;
    }

    mEnemyTurn.moveX = nextX;
    mEnemyTurn.moveY = nextY;

    mEnemyTurn.hasAction = true;
    mEnemyTurn.skillSlot = PartSlot::RightArm;

    // TODO: Tem que levar em conta a skill
    mEnemyTurn.targetX = px;
    mEnemyTurn.targetY = py;

    SDL_Log("Inimigo planejou ir para (%d, %d) e atacar (%d, %d)",
        mEnemyTurn.moveX, mEnemyTurn.moveY, mEnemyTurn.targetX, mEnemyTurn.targetY);
}

void AI::mapvalues() {
    int cols = mGrid->GetCols();
    int rows = mGrid->GetRows();
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {

        }
    }
}