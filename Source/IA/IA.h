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
    std::string name;
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
    void UpdatePlayerStats(RobotStats &playerStats) {mPlayerStats = playerStats;}
    void UpdateEnemyStats(RobotStats &enemyStats) {mEnemyStats = enemyStats;}
    std::vector<std::vector<int>> MapEfficiency(std::vector<std::vector<int>> mapData);

private:
    RobotStats mPlayerStats;
    RobotStats mEnemyStats;

        /*
     * Define a matriz do mapa para a ia tomar decisões:
     * 0 = chão livre, caminhável
     * 1 = parede, bloqueio
     * 2 = destrutível, bloqueio, porém pode ser destruído
     * 3 = mel, terreno especial (efeito)
     * 4 = fogo, terreno especial (efeito)
     */
    std::vector<std::vector<int>> mMapData;
    std::vector<std::vector<int>> mMapEfficiency;
    int mBloodThirst = 4;
    std::vector<std::vector<int>> transpose(const std::vector<std::vector<int>>& g);
    Vector2 ChooseMove(bool isEnemy = true);
    Vector2 ChooseTarget(Vector2 pos);
    PartSlot ChooseSlot(Vector2 target, Vector2 pos);

    // helpers
    bool IsInRange(Vector2& origin, const Vector2& target, int minRange, int maxRange);
    bool CanHitPlayer(const RobotStats& enemy, const RobotStats& player, PartStats weapon);
    // std::vector<Vector2> FindTilesInRange(Vector2& origin, int minRange, int maxRange, std::vector<std::vector<int>>& grid);
    std::vector<Vector2> GetReachableTiles(Vector2& start, int moveRange, std::vector<std::vector<int>>& grid);

    // logicas específicas
    void NaiveResolution(EnemyResolution* action);
};