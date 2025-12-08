#include "IA.h"
#include <queue>

// TODO: fazer uma forma de informar ao jogo em quais lugares colocar mel, por exemplo do ataque da abelha

IA::IA(RobotStats playerStats, RobotStats enemyStats)
    : mPlayerStats(playerStats), mEnemyStats(enemyStats)
{
    auto PrintRobotStats = [&] (const RobotStats& r)
    {
        SDL_Log("----- IA LOG: Robot %s -----", r.name.c_str());
        SDL_Log("Pos: (%.0f, %.0f)", r.position.x, r.position.y);

        auto printP = [&](const char* n, const PartStats& p){
            SDL_Log("%s \t-> HP: %d/%d \t DMG: %d \t RNG: %d \t Skill: %s    \t Name: %s",
                n, p.currentHP, p.maxHP, p.damage, p.range, p.skill.c_str(), p.name.c_str());
        };

        printP("Head ",     r.head);
        printP("Torso",     r.torso);
        printP("RightArm",  r.rightArm);
        printP("LeftArm",   r.leftArm);
        printP("Legs  ",    r.legs);
    };

    PrintRobotStats(mPlayerStats);
    PrintRobotStats(mEnemyStats);
}

// calcula a distância de blocos entre origin e target e verifica se está no range
bool IA::IsInRange(Vector2& origin, const Vector2& target, int minRange, int maxRange)
{
    int dx = abs((int)target.x - (int)origin.x);
    int dy = abs((int)target.y - (int)origin.y);
    int dist = dx + dy;
    return (dist >= minRange && dist <= maxRange);
}

// verifica se o inimigo pode atacar o jogador com a arma dada
bool IA::CanHitPlayer(const RobotStats& enemy, const RobotStats& player, PartStats weapon)
{
    Vector2 epos = enemy.position;
    Vector2 ppos = player.position;

    int minRange = 1;
    int maxRange = weapon.range;

    return IsInRange(epos, ppos, minRange, maxRange);
}

// retorna todos os tiles alcançáveis a partir de start dentro do moveRange
std::vector<Vector2> IA::GetReachableTiles(Vector2& start, int moveRange, std::vector<std::vector<int>>& grid)
{
    struct Node { int x, y, dist; };

    int rows = grid.size();
    int cols = grid[0].size();

    std::vector<Vector2> result;

    int sx = (int)start.x;
    int sy = (int)start.y;

    std::queue<Node> q;
    q.push({ sx, sy, 0 });

    bool visited[64][64] = { false };
    visited[sy][sx] = true;

    int dirs[4][2] = { {1,0}, {-1,0}, {0,1}, {0,-1} };

    while (!q.empty())
    {
        auto [x, y, d] = q.front();
        q.pop();

        result.emplace_back(x, y);

        if (d == moveRange)
            continue;

        for (auto& dir : dirs)
        {
            int nx = x + dir[0];
            int ny = y + dir[1];

            if (nx < 0 || ny < 0 || ny >= rows || nx >= cols) continue;
            if (visited[ny][nx]) continue;

            // somente walkable = 0
            if (grid[ny][nx] != 0) continue;

            visited[ny][nx] = true;
            q.push({ nx, ny, d + 1 });
        }
    }

    return result;
}

// resolução ingênua: mover em direção até conseguir ter range, atacar no espaço onde ele estava
// por último (para o ataque funcionar, o robô tem que não se mover)
// caso o caminho dele esteja bloqueado, quebra o elemento destrutível
// TODO: verificar regra de jogo, está atacando mesmo tendo um destrutível na frente.
void IA::NaiveResolution(EnemyResolution* action)
{
    Vector2 enemyPos = mEnemyStats.position;
    Vector2 playerPos = mPlayerStats.position;

    // escolhe a arma esquerda caso a direita esteja destruída
    PartSlot  weaponPart = mEnemyStats.rightArm.currentHP > 0 ? PartSlot::RightArm : PartSlot::LeftArm;
    PartStats weapon = mEnemyStats.rightArm.currentHP > 0 ? mEnemyStats.rightArm : mEnemyStats.leftArm;

    // checa se já é possível atacar na atual posição
    if (CanHitPlayer(mEnemyStats, mPlayerStats, weapon))
    {
        action->hasAction = true;
        action->skillSlot = weaponPart;
        action->targetTile = playerPos;
        action->moveTo = enemyPos;
        return;
    }

    // caso não seja, vamos mover para mais perto. começamos verificando onde podemos andar
    std::vector<Vector2> reachable = GetReachableTiles(enemyPos, mEnemyStats.legs.range, mMapData);

    // escolha o tile que fique mais próximo do jogador
    Vector2 bestPos = enemyPos; // posição atual
    int bestDist = 9999;        // distância ruim

    for (auto& tile : reachable)
    {
        int dist = abs(tile.x - playerPos.x) + abs(tile.y - playerPos.y);

        // evita ficar no mesmo lugar se puder melhorar
        if (dist < bestDist)
        {
            bestDist = dist;
            bestPos = tile;
        }
    }

    // define a melhor posição para mover, utilizada em todos os outros casos
    action->moveTo = bestPos;

    // simular posição para checar ataque possível
    mEnemyStats.position = bestPos;

    if (CanHitPlayer(mEnemyStats, mPlayerStats, weapon))
    {
        action->hasAction = true;
        action->skillSlot = weaponPart;
        action->targetTile = playerPos;
        return;
    }

    SDL_Log("Não é possível atacar o jogador mesmo após mover. Procurando destrutíveis...");
    // caso ainda não alcance, verificar se há algum destrutível no range da arma equipada e destruir o que
    // é mais próximo da posição do player
    int maxRange = weapon.range;
    int minRange = 1;

    Vector2 chosenTarget(-1, -1);
    int bestDestructibleDist = 9999;

    // verifica todos os tiles destrutíveis
    for (int y = 0; y < mMapData.size(); y++)
    {
        for (int x = 0; x < mMapData[0].size(); x++)
        {
            if (mMapData[y][x] != 2) // não é destrutível
                continue;

            // verifica se o destrutível está no range da arma
            int dx = abs(x - bestPos.x);
            int dy = abs(y - bestPos.y);
            int distToEnemy = dx + dy;

            if (distToEnemy < minRange || distToEnemy > maxRange)
                continue;

            // prioriza o destrutível mais perto do player (abrir caminho)
            int distToPlayer = abs(x - playerPos.x) + abs(y - playerPos.y);

            if (distToPlayer < bestDestructibleDist)
            {
                bestDestructibleDist = distToPlayer;
                chosenTarget = Vector2(x, y);
            }
        }
    }

    // se encontrou algum alvo para destruir, destrua.
    if (chosenTarget.x != -1)
    {
        action->hasAction = true;
        action->skillSlot = weaponPart;
        action->targetTile = chosenTarget;
        return;
    }

    SDL_Log("Não foi possível encontrar destrutíveis.");
}

EnemyResolution IA::GetEnemyResolution(std::vector<std::vector<int>> mapData)
{
    mMapData = mapData;

    EnemyResolution action;
    action.moveTo     = Vector2::Zero;
    action.targetTile = Vector2::Zero;
    action.hasAction  = false;
    action.skillUsed  = "";

    NaiveResolution(&action);

    return action;
}
