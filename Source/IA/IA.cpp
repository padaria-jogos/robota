#include "IA.h"
#include <queue>
#include <sstream>
#include <vector>
#include <utility>
#include <random>

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

std::vector<std::vector<int>> IA::transpose(const std::vector<std::vector<int>>& g) {
    int rows = g.size();
    int cols = g[0].size();

    std::vector<std::vector<int>> t(cols, std::vector<int>(rows));

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            t[j][i] = g[i][j];
        }
    }

    return t;
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

std::vector<std::vector<int>> IA::MapEfficiency(std::vector<std::vector<int>> mapData) {
    int cols = mapData[0].size();
    int rows = mapData.size();

    int ex = mEnemyStats.position.x;
    int ey = mEnemyStats.position.y;

    int px = mPlayerStats.position.x;
    int py = mPlayerStats.position.y;

    // pré-calcular limites do "entre"
    int minRow = std::min(ex, px);
    int maxRow = std::max(ex, px);
    int minCol = std::min(ey, py);  // cuidado: dependendo da sua convenção, x = col, y = row
    int maxCol = std::max(ey, py);

        /*
    * Define a matriz do mapa para a ia tomar decisões:
    * 0 = chão livre, caminhável
    * 1 = parede, bloqueio
    * 2 = destrutível, bloqueio, porém pode ser destruído
    * 3 = mel, terreno especial (efeito)
    * 4 = fogo, terreno especial (efeito)
    */
    std::vector<std::vector<int>> mapEfficiency(rows, std::vector<int>(cols, 0));
    int movement = mEnemyStats.legs.range;
    for (int i = 0; i < mapData.size(); i++) {
        for (int j = 0; j < mapData[i].size(); j++) {
            // type and efficiency of that position.
            // make basic efficiency map.
            int type = mapData[i][j];
            int eff = 0;
            if (type == 1) eff = -1000;
            if (type == 2) eff = -1000;
            mapEfficiency[i][j] = eff;
        }
    }

    //loop para definir a quantidade de vizinhos
    for (int i = 0; i < mapEfficiency.size(); i++) {
        for (int j = 0; j < mapEfficiency[i].size(); j++) {
            // calculate advanced efficiency.
            // for each pos, calculate available neighbors
            int type = mapData[i][j];
            int eff = 0;
            if (mapEfficiency[i][j] == -1000) continue;

            //calculate reverse crampedness
            if (i   > 0                       && mapEfficiency[i-1][j] != -1000) eff++;
            if (i+1 < mapEfficiency.size()    && mapEfficiency[i+1][j] != -1000) eff++;
            if (j   > 0                       && mapEfficiency[i][j-1] != -1000) eff++;
            if (j+1 < mapEfficiency[i].size() && mapEfficiency[i][j+1] != -1000) eff++;



            mapEfficiency[i][j] = mapEfficiency[i][j] + eff;
        }
    }

    //loop para definir a quantidade de vizinhos dos seus vizinhos
    std::vector<std::vector<int>> original = mapEfficiency;
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {

            if (original[i][j] == -1000) continue;

            int eff = 0;
            if (i > 0               && original[i-1][j] != -1000) eff += original[i-1][j];
            if (i+1 < rows          && original[i+1][j] != -1000) eff += original[i+1][j];
            if (j > 0               && original[i][j-1] != -1000) eff += original[i][j-1];
            if (j+1 < cols          && original[i][j+1] != -1000) eff += original[i][j+1];

            //Se entre os robôs, prioriza o espaço.
            if (i >  minRow && i <  maxRow &&
                j >  minCol && j <  maxCol) eff = eff + 2*mBloodThirst; //preference for moving to other robot
            if (i >= minRow && i <= maxRow &&
                j >= minCol && j <= maxCol) eff = eff +   mBloodThirst; //preference for moving to other robot
            if (std::abs(i-px) + std::abs(j-py) <=2)
                if (mEnemyStats.name == "Rosevif")
                    eff = eff + 10*mBloodThirst;




            mapEfficiency[i][j] += eff;

        }
    }



    SDL_Log("---------- IA EFFICIENCY DATA ----------");
    for (int y = 0; y < mapEfficiency.size(); y++)
    {
        std::stringstream ss;
        for (int x = 0; x < mapEfficiency[y].size(); x++)
        {
            ss << mapEfficiency[y][x] << " ";
        }
        SDL_Log("%s", ss.str().c_str());
    }
    SDL_Log("----------------------------------");
    return mapEfficiency;
}

Vector2 IA::ChooseMove(bool isEnemy) { //defaults true
    Vector2 pos, ppos;
    int legRange;
    if (isEnemy) {
        pos = mEnemyStats.position;
        ppos = mPlayerStats.position;
        legRange = mEnemyStats.legs.range;
    }
    else {
        pos = mPlayerStats.position;
        ppos = mEnemyStats.position;
        legRange = mPlayerStats.legs.range;
    }

    int ex = pos.x, ey = pos.y;


    int rows = mMapEfficiency.size();
    int cols = mMapData[0].size();

    std::vector<Vector2> positions;
    //std::vector<std::vector<int>> grid = transpose(mMapData);
    //Vector2 reversepos = Vector2(pos.y, pos.x);
    std::vector<Vector2> reachables = GetReachableTiles(pos, legRange, mMapData);

    // auto inBounds = [&](int x, int y) {
    //     return x >= 0 && x < rows && y >= 0 && y < cols;
    // };

    auto inBounds = [&](int x, int y) {
        return x >= 0 && x < cols && y >= 0 && y < rows;
    };

    for (auto tile : reachables) {
        int tx = tile.x;
        int ty = tile.y;
        if (!inBounds(tx, ty)) continue;

        if (tx == ppos.x && ty == ppos.y) continue;
        positions.emplace_back(tx, ty);
    }

    // sorteio ponderado
    std::vector<int> weights;
    weights.reserve(positions.size());

    for (auto& p : positions) {
        int w = mMapEfficiency[p.y][p.x]; //changed! but why??
        if (w < 0) w = 0; //
        weights.emplace_back(w);
    }

    int total = 0;
    for (int w : weights) total += w;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist(0, total - 1);

    int r = dist(gen);
    //soma da variavel aleatoria
    for (int i = 0; i < weights.size(); i++) {
        if (r < weights[i]) {
            return positions[i];
        }
        r -= weights[i];
    }
    // fallback
    return positions.back();
}

Vector2 IA::ChooseTarget(Vector2 pos) {
    Vector2 target = Vector2(-1, -1);

    int ex = pos.x, ey = pos.y;
    int rows = mMapData.size();
    int cols = mMapData[0].size();
    Vector2 possibleTarget = ChooseMove(false);
    Vector2 ppos = possibleTarget;
    int atksRange = std::max(mEnemyStats.leftArm.range, mEnemyStats.rightArm.range);
    //std::vector<std::vector<int>> grid = transpose(mMapData);
    //Vector2 reversepos = Vector2(pos.y, pos.x);
    std::vector<Vector2> reachables = GetReachableTiles(pos, atksRange-1, mMapData);

    auto inBounds = [&](int x, int y) {
        return x >= 0 && x < rows && y >= 0 && y < cols;
    };

    int proximity = 50;

    //checks every place in range of attack from the new moved pos
    for (auto tile : reachables) {
        for (int dx = -1; dx <= 1; dx++) {
            for (int dy = -1; dy <= 1; dy++) {
                int nx = tile.x + dx;
                int ny = tile.y + dy;
                //checks if inside map
                if (!inBounds(nx, ny)) continue;

                // manhattan distance limit <= 1
                if (std::abs(dx) + std::abs(dy) <= 1) {
                    // if the predicted enemy movement is in range, shoot it
                    if (nx == ppos.x && ny == ppos.y) {
                        target = possibleTarget;
                        return target;
                    }
                    // next best thing: nearby destructible barrels.
                    if (ny >= 0 && ny < rows && nx >= 0 && nx < cols)   // correção para a segfault
                    {
                        if (mMapData[ny][nx] == 2)
                        {
                            int newproximity = std::abs(ex - nx) + std::abs(ey - ny);
                            if (newproximity < proximity) {
                                proximity = newproximity;
                                target = Vector2(nx, ny);
                            }
                        }
                    }
                }
            }
        }
    }
    return target;
}

PartSlot IA::ChooseSlot(Vector2 target, Vector2 pos) {
    int dist = std::abs(pos.x - target.x) + std::abs(pos.y - target.y);
    int lrange = mEnemyStats.leftArm.range, ldmg = mEnemyStats.leftArm.damage;
    int rrange = mEnemyStats.rightArm.range, rdmg = mEnemyStats.rightArm.damage;
    if (lrange >= dist && rrange >= dist) {
        if (ldmg > rdmg) return PartSlot(LeftArm);
        else return PartSlot(RightArm);
    }
    if (lrange > rrange) return PartSlot(LeftArm);
    else return PartSlot(RightArm);
}

EnemyResolution IA::GetEnemyResolution(std::vector<std::vector<int>> mapData)
{
    mMapData = mapData;
    mMapEfficiency = MapEfficiency(mapData);
    if (mEnemyStats.name == "Rosevif") mBloodThirst = 12;
    Vector2 movement = ChooseMove();

    Vector2 target = ChooseTarget(movement);
    PartSlot slot = ChooseSlot(target, movement);
    bool willAtk = true;
    if (target.x == -1) willAtk = false;
    EnemyResolution action;

    SDL_Log("%s", mEnemyStats.name.c_str());

    if (mEnemyStats.name == "EvilRobota") {
        NaiveResolution(&action);
        return action;
    }

    action.moveTo     = movement;
    action.targetTile = target;
    action.hasAction  = willAtk;
    action.skillSlot  = slot;
    action.skillUsed  = "";



    return action;
}
