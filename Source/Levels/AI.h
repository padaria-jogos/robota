//
// Created by Murilo on 11/29/2025.
//

#ifndef ROBOTA_AI_H
#define ROBOTA_AI_H

#include "Camera.h"
#include "Game.h"
#include "Actors/Robot.h"
#include "Actors/GridCursor.h"
#include "Map/GridMap.h"

class AI {
public:
    AI(GridMap* grid,
        int enemyX,
        int enemyY,
        const std::vector<SkillType>& enemyAttacks,
        const std::vector<SkillType>& playerAttacks);

    void mapvalues();
    void move(int ex, int ey, int px, int py);
    void attack();
private:
    GridMap* mGrid;
    std::vector<std::vector<int>> vmap;
    int mEnemyX;
    int mEnemyY;

    std::vector<SkillType> mEnemySkills;
    std::vector<SkillType> mPlayerSkills;
};


#endif //ROBOTA_AI_H