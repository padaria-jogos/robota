//
// Created by mateus on 06/12/2025.
//

#pragma once
#include <string>
#include <vector>
#include <algorithm>
#include "Math.h"
#include "Actors/Robot.h"

struct PartStats
{
    int maxHP;
    int currentHP;
    int damage;
    int range;
    std::string skill;
};

struct RobotStats
{
    std::string name;
    Vector2 position;   // posição na grid
    PartStats head;
    PartStats torso;
    PartStats rightArm;
    PartStats leftArm;
    PartStats legs;
};

struct EnemyResolution
{
    Vector2 moveTo;
    bool hasAction;
    Vector2 targetTile;
    PartSlot skillSlot;     // qual pedaço vai usar
    std::string skillUsed;  // utilizado para sistema de particulas, por ex
};

// por questões de manutenção do código, os robôs sempre fazem o caminho mínimo entre
// a posição atual e a posição alvo.
// isso é calculado pela função CalculatePath() da GridMap.

class IA
{
public:
    IA(RobotStats playerStats, RobotStats enemyStats);

    EnemyResolution GetEnemyResolution(std::vector<std::vector<int>> mMapData);
    void UpdatePlayerPosition(Vector2 position) {mPlayerStats.position = position;}
    void UpdateEnemyPosition(Vector2 position) {mEnemyStats.position = position;}

private:
    RobotStats mPlayerStats;
    RobotStats mEnemyStats;
    std::vector<std::vector<int>> mMapData;

    // logicas específicas
    void NaiveResolution(EnemyResolution* action);
};