//
// Created by mateus on 06/12/2025.
//

#include "IA.h"

IA::IA(RobotStats playerStats, RobotStats enemyStats) :
    mPlayerStats(playerStats),
    mEnemyStats(enemyStats)
{
    auto PrintRobotStats = [&] (const RobotStats& r)
    {
        SDL_Log("=== IA LOG: ROBOT %s ===", r.name.c_str());
        SDL_Log("Pos: (%.0f, %.0f)", r.position.x, r.position.y);

        auto printP = [&](const char* n, const PartStats& p){
            SDL_Log("%s \t-> HP: %d/%d \t DMG: %d \t RNG: %d \t Skill: %s", n, p.currentHP, p.maxHP, p.damage, p.range, p.skill.c_str());
        };

        printP("Head ",     r.head);
        printP("Torso",    r.torso);
        printP("RightArm", r.rightArm);
        printP("LeftArm",  r.leftArm);
        printP("Legs  ",     r.legs);
    };

    PrintRobotStats(mPlayerStats);
    PrintRobotStats(mEnemyStats);
}

void IA::NaiveResolution(EnemyResolution* action)
{
    // obter posições do inimigo e do player, na grid
    int ex = mEnemyStats.position.x;
    int ey = mEnemyStats.position.y;

    int px = mPlayerStats.position.x;
    int py = mPlayerStats.position.y;

    // calcular movimento naive: aproximar 1 tile
    int nextX = ex;
    int nextY = ey;

    if (px > ex) nextX++;
    else if (px < ex) nextX--;

    if (py > ey) nextY++;
    else if (py < ey) nextY--;

    // clamp aos limites do mapa
    int maxCols = mMapData.at(0).size();
    int maxRows = mMapData.size();

    nextX = std::clamp(nextX, 0, maxCols - 1);
    nextY = std::clamp(nextY, 0, maxRows - 1);

    // verificar obstaculo
    int tile = mMapData.at(nextY).at(nextX);

    bool isBlocked = (tile != 0); // considerando: 0 = chão livre, qualquer outro valor = obstáculo

    if (isBlocked)
    {
        nextX = ex;
        nextY = ey;
    }

    // preencher enemy resolution
    action->moveTo = Vector2((float)nextX, (float)nextY);
    action->hasAction = true;

    // mira no player
    action->targetTile = Vector2((float)px, (float)py);

    // skill sempre RightArm
    action->skillSlot = PartSlot::RightArm;
    action->skillUsed = mEnemyStats.rightArm.skill;
}

EnemyResolution IA::GetEnemyResolution(std::vector<std::vector<int>> mapData)
{
    mMapData = mapData;

    EnemyResolution action;
    action.moveTo = Vector2::Zero;
    action.hasAction = false;
    action.targetTile = Vector2::Zero;
    action.skillUsed = "";

    // chamar as funções que resolvem a IA aqui
    NaiveResolution(&action);

    return action;
}