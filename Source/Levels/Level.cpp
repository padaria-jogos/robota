//
// Created by mateus on 25/11/2025.
//

#include "Level.h"
#include <algorithm>
#include <fstream>
#include <sstream>
#include <vector>
#include <utility>
#include "Game.h"
#include "Math.h"
#include "Actors/Block.h"
#include "Actors/Destructible.h"
#include "Json.h"
#include "UI/Screens/ActionSelection.h"
#include "UI/Screens/GameOver.h"
#include "UI/Screens/TileSelection.h"
#include "UI/Screens/Win.h"

// Constantes de altura Z para posicionamento de objetos
namespace {
    constexpr float FLOOR_Z = -500.0f;
    constexpr float WALL_Z = 0.0f;
}

// TODO: Dinamite. Formatos de Ataques(Urgente), Fazer o Modelo do ultimo boss
// TODO: Verificar se a câmera está sendo destruída

Level::Level(class Game *game, HUD *hud) :
    mGame(game),
    mCamera(game->GetCamera()),
    mHud(hud),
    mCursor(nullptr),
    mGrid(nullptr),
    mBattleState(BattleState::Null),
    mSelectedSlot(PartSlot::Null),
    mPlayer(nullptr),
    mEnemy(nullptr),
    mGhostPlayer(nullptr),
    mIsResolving(false),
    mStepIndex(0),
    mActionSelection(nullptr),
    mTileSelection(nullptr),
    mSkybox(nullptr),
    mIA(nullptr)
{
    mBattleState = BattleState::Exploration;
    mParticleManager = new ParticleManager(game, 500);

    // ---------- SOUND ----------
    mGame->GetAudio()->StopAllSounds();
    mLevelMusic = mGame->GetAudio()->PlaySound("Backpullver-Shamanez-Overnight.wav", true);

    // ---------- CURSOR ----------
    mCursor = new GridCursor(game);

    // ---------- ROBOTS ----------
    mPlayer = new Robot(game, Team::Player);
    mEnemy = new Robot(game, Team::Enemy);
    
    // Começa com o braço direito escolhido
    SetSelectedSlot(PartSlot::RightArm);

    // HUD tracking
    if (mHud)
        mHud->TrackRobots(mPlayer, mEnemy);
}

void Level::SetWorldLightIntensity(float intensity)
{
    mGame->GetRenderer()->SetWorldLightIntensity(intensity);
}

void Level::MoveCursor(int xOffset, int yOffset)
{
    // não move quando a tela de seleção de habilidade está mostrando
    if (mBattleState == BattleState::SkillSelection)
        return;

    // define a nova posição absoluta de acordo com offset
    if (!mGrid || !mCursor) return;

    int currentX = mCursor->GetGridX();
    int currentY = mCursor->GetGridY();

    int newX = currentX + xOffset;
    int newY = currentY + yOffset;

    // Verificar se saiu do mapa (Clamp)
    if (newX < 0) newX = 0;
    else if (newX >= mGrid->GetCols()) newX = mGrid->GetCols() - 1;

    if (newY < 0) newY = 0;
    else if (newY >= mGrid->GetRows()) newY = mGrid->GetRows() - 1;

    MoveInGrid(mCursor, newX, newY);
}

// helper for ProcessInput positioning
Vector2 SnapDirection(const Vector2& v)
{
    if (fabs(v.x) > fabs(v.y))
        return Vector2(v.x > 0 ? 1 : -1, 0);

    return Vector2(0, v.y > 0 ? 1 : -1);
}

void Level::ProcessInput(const SDL_Event &event)
{
    // free camera mode overlap inputs, so we skip game inputs when active
    // TODO: por alguma razão, ainda consigo selecionar o robô
    if (mCamera->IsFreeCameraMode())
        return;

    // get the direction relative to the camera
    Vector3 f3 = Vector3::Normalize(mCamera->GetTarget() - mCamera->GetPosition());
    Vector2 f2 = SnapDirection(Vector2(f3.x, f3.y));

    Vector2 forward  = f2;
    Vector2 backward = f2 * Vector2(-1.0f, -1.0f);
    Vector2 left     = Vector2(f2.y, -f2.x);
    Vector2 right    = Vector2(-f2.y, f2.x);

    switch (event.key.keysym.sym)
    {
        case SDLK_w:
            MoveCursor(forward.x, forward.y);
            break;

        case SDLK_s:
            MoveCursor(backward.x, backward.y);
            break;

        case SDLK_a:
            MoveCursor(left.x, left.y);
            break;

        case SDLK_d:
            MoveCursor(right.x, right.y);
            break;

        case SDLK_SPACE:
            HandleAction();
            break;

        case SDLK_BACKSPACE:
        case SDLK_ESCAPE:
            HandleCancel();
            break;

        default:
            break;
    }
}

void Level::HandleAction()
{
    if (!mGrid || !mPlayer) return;

    switch (mBattleState) {
        case BattleState::Exploration:
            {
                HandleExplorationPhase();
                break;
            }

        case BattleState::MoveSelection:
            {
                HandleMovementPhase();
                break;
            }

        case BattleState::SkillSelection:
            {
                HandleSkillSelectionPhase(GetSelectedSlot());
                break;
            }

        case BattleState::TargetSelection:
            {
                HandleTargetingPhase();
                break;
            }

        default:
            break;
    }
}

void Level::HandleExplorationPhase()
{
    if (mPlayer->GetGridX() == mCursor->GetGridX() &&
                mPlayer->GetGridY() == mCursor->GetGridY())
    {
        // Verifica se está stunned (preso no mel)
        if (mPlayer->HasStatusEffect(StatusEffect::Stunned)) {
            NotifyPlayer("VOCÊ ESTÁ PRESO NO MEL! Não pode se mover, mas pode atacar.");
            SetBattleState(BattleState::SkillSelection);
            SetSelectedSlot(PartSlot::RightArm);
            return;
        }
        
        SetBattleState(BattleState::MoveSelection);
        // Ghost
        SpawnGhost();

        // BFS azul
        int range = mPlayer->GetMovementRange();
        auto tiles = mGrid->GetWalkableTiles(mPlayer->GetGridX(), mPlayer->GetGridY(), range);

        for (const auto& node : tiles) {
            Tile* t = mGrid->GetTileAt(node.x, node.y);
            if (t) {
                t->SetTileType(TileType::Path);
            }
        }

        SDL_Log("Ação do jogador: Selecionado %s. Aguardando movimento...", mPlayer->GetName().c_str());
        NotifyPlayer("Escolha a posição de destino");
    }
    else
    {
        NotifyPlayer("Essa unidade nao e sua.");
    }
}

void Level::HandleMovementPhase()
{
    int cx = mCursor->GetGridX();
    int cy = mCursor->GetGridY();
    Tile* targetVisual = mGrid->GetTileAt(mCursor->GetGridX(), mCursor->GetGridY());
    if (targetVisual && targetVisual->GetType() == TileType::Path)
    {
        // Verifica se não tem ninguém no destino (exceto eu mesmo)
        Actor* obstacle = mGrid->GetUnitAt(mCursor->GetGridX(), mCursor->GetGridY());
        if (obstacle && obstacle != mPlayer) {
            NotifyPlayer("Tile ocupado!");
            return;
        }

        // Move o ghost
        MoveInGrid(mGhostPlayer, cx, cy);
        mPlayerTurn.moveX = cx;
        mPlayerTurn.moveY = cy;

        // Limpa o azul do chão e troca o estado
        mGrid->ClearTileStates();
        SetBattleState(BattleState::SkillSelection);

        // Setup padrão de parte selecionada
        SetSelectedSlot(GetSelectedSlot());

        NotifyPlayer("Movimento concluido.");
    }
    else {
        NotifyPlayer("Movimento invalido.");
    }
}

void Level::HandleSkillSelectionPhase(PartSlot slot)
{
    const RobotPart& part = mPlayer->GetPart(slot);
    if (part.isBroken)
    {
        std::string msg = "ERRO: A parte '" + part.name + "' esta QUEBRADA! Escolha outra.";
        NotifyPlayer(msg);
        return;
    }
    SetSelectedSlot(slot);
    mGrid->ClearTileStates();

    int range = mPlayer->GetPartRange(slot);
    
    // Se stunned, permite atacar próprio tile (minRange = 0)
    int minRange = mPlayer->HasStatusEffect(StatusEffect::Stunned) ? 0 : 1;

    // Range da skill em vermelho
    auto tiles = mGrid->GetAttackableTiles(
        mGhostPlayer->GetGridX(),
        mGhostPlayer->GetGridY(), // Origem Y é o Robô
        minRange,
        range
    );

    for (const auto& node : tiles) {
        Tile* t = mGrid->GetTileAt(node.x, node.y);
        if (t) {
            t->SetTileType(TileType::Attack);
        }
    }

    SetBattleState(BattleState::TargetSelection);
    
    if (mPlayer->HasStatusEffect(StatusEffect::Stunned)) {
        NotifyPlayer("Preso no mel! Ataque o próprio tile para se libertar.");
    }
    else
    {
        SDL_Log("Ação do jogador: selecionou habilidade. Aguardando mira...");
        NotifyPlayer("Selecione o alvo");
    }


}

void Level::HandleTargetingPhase()
{
    Tile* targetTile = mGrid->GetTileAt(mCursor->GetGridX(), mCursor->GetGridY());

    if (targetTile && targetTile->GetType() == TileType::Attack) {
        // Caso especial: Atacando o próprio tile (mel) para se livrar do stun
        if (mPlayer->HasStatusEffect(StatusEffect::Stunned)) {
            int px = mPlayer->GetGridX();
            int py = mPlayer->GetGridY();
            int cx = mCursor->GetGridX();
            int cy = mCursor->GetGridY();
            
            if (px == cx && py == cy) {
                // Atacou o próprio tile com mel
                TerrainType terrainType = mGrid->GetTerrainType(px, py);
                if (terrainType == TerrainType::Honey) {
                    // Remove o mel do tile (muda o tipo base)
                    mGrid->SetTerrainType(px, py, TerrainType::Floor);
                    
                    // Troca a textura do Block para chão normal
                    Actor* floorActor = mGrid->GetFloorBlock(px, py);
                    if (floorActor) {
                        auto* floorBlock = dynamic_cast<Block*>(floorActor);
                        floorBlock->SetTexture(mLevelConfig.floorTexture);
                    }
                    
                    // Remove o stun
                    mPlayer->RemoveStatusEffect(StatusEffect::Stunned);
                    NotifyPlayer("Você se libertou do mel!");
                    
                    mGrid->ClearTileStates();
                    SetBattleState(BattleState::Exploration);
                    
                    // Inimigo age sozinho
                    CalculateEnemyAction();
                    ResolveTurn();
                    return;
                }
            }
        }
        
        mPlayerTurn.hasAction = true;
        mPlayerTurn.skillSlot = GetSelectedSlot();
        mPlayerTurn.targetX = mCursor->GetGridX();
        mPlayerTurn.targetY = mCursor->GetGridY();


        // Limpa o vermelho e troca de estado
        mGrid->ClearTileStates();
        RemoveGhost();
        SetBattleState(BattleState::Exploration);

        NotifyPlayer("=== PLANEJAMENTO CONCLUIDO. INICIANDO RESOLUCAO ===");
        StartResolution();
    }else {
        NotifyPlayer("Alvo invalido! Selecione um quadrado dentro da range.");
    }
}

void Level::HandleUnitDeath(Robot*& robot)
{
    if (robot && robot->IsDead())
    {
        mGrid->SetUnitAt(nullptr, robot->GetGridX(), robot->GetGridY());
        robot->SetState(ActorState::Destroy);
        robot->SetState(ActorState::Destroy);
        if (robot == mPlayer) mPlayer = nullptr;
        if (robot == mEnemy)  mEnemy = nullptr;

        robot = nullptr;
        NotifyBoth("Level removeu o robo do jogo.");
    }
}

void Level::HandleCancel()
{
    if (!mPlayer) return;
    
    // Se está stunned, não pode cancelar (deve atacar o mel)
    if (mPlayer->HasStatusEffect(StatusEffect::Stunned) && 
        mBattleState == BattleState::SkillSelection) {
        NotifyPlayer("Você está preso no mel! Precisa atacar para se libertar!");
        return;
    }
    
    // UNDO SELECTION
    switch (mBattleState) {
        case BattleState::MoveSelection:
        {
            mGrid->ClearTileStates();
            RemoveGhost();
            SetBattleState(BattleState::Exploration);
            NotifyPlayer("Selecao cancelada.");
            break;
        }

        case BattleState::SkillSelection:
        {
            int px = mPlayer->GetGridX();
            int py = mPlayer->GetGridY();

            // Move o Ghost de volta
            MoveInGrid(mGhostPlayer, px, py);

            // Sincroniza o cursor
            MoveInGrid(mCursor, px, py);
            mGrid->SetSelectedTile(px, py);

            // Volta estado e repinta o azul
            SetBattleState(BattleState::MoveSelection);

            int range = mPlayer->GetMovementRange();
            auto tiles = mGrid->GetWalkableTiles(px, py, range);
            for (const auto& node : tiles) {
                Tile* t = mGrid->GetTileAt(node.x, node.y);
                if (t) {
                    t->SetTileType(TileType::Path);
                }
            }

            NotifyPlayer("Movimento do fantasma desfeito.");
            break;
        }

        case BattleState::TargetSelection:
        {
            mGrid->ClearTileStates();

            int gx = mGhostPlayer->GetGridX();
            int gy = mGhostPlayer->GetGridY();

            MoveInGrid(mCursor, gx, gy);
            mGrid->SetSelectedTile(gx, gy);

            SetBattleState(BattleState::SkillSelection);

            NotifyPlayer("Mira cancelada. Escolha outra habilidade (1 ou 2).");
            break;
        }

        default:
            break;
    }
}

void Level::HandleWait() {
    if (!mPlayer) return;
    mPlayerTurn.moveX = mGhostPlayer->GetGridX();
    mPlayerTurn.moveY = mGhostPlayer->GetGridY();

    mPlayerTurn.hasAction = false;
    mPlayerTurn.targetX = -1;
    mPlayerTurn.targetY = -1;

    mGrid->ClearTileStates();
    RemoveGhost();

    SetBattleState(BattleState::Exploration);
    NotifyPlayer("Jogador escolheu ESPERAR.");
    NotifyBoth("=== INICIANDO RESOLUCAO ===");
    CalculateEnemyAction();
    ResolveTurn();
    NotifyBoth("=== FIM DO TURNO ===");
}

void Level::MoveInGrid(Actor *actor, int x, int y) const {
    if (!actor || !mGrid) return;

    // Atualiza o Visual (mundo 3D)
    Vector3 worldPos = mGrid->GetWorldPosition(x, y);
    worldPos.z = OFFSET_Z;
    actor->SetPosition(worldPos);

    // Atualiza o Grid Lógico
    auto robot = dynamic_cast<Robot*>(actor);
    if (robot)
    {
        if (actor != mGhostPlayer)
            mGrid->SetUnitAt(robot, x, y);  // atualizar a informacao do robo no vector da grid
        robot->UpdateGridCoords(x, y);
    }
    else if (actor == mCursor)
    {
        mCursor->UpdateGridCoords(x, y);
        mGrid->SetSelectedTile(mCursor->GetGridX(), mCursor->GetGridY());   // visual
    }
}

void Level::SpawnGhost() { // TODO: Sync da rotação
    if (!mGhostPlayer) {
        mGhostPlayer = new Robot(mGame, mPlayer->GetTeam());
        mGhostPlayer->SetName("Ghost");
        mGhostPlayer->SetGhostMode(true);
    }

    mGhostPlayer->SetVisible(true);
    mGhostPlayer->CopyDataFrom(mPlayer);
    mGhostPlayer->SyncAnimationState(mPlayer);
    mGhostPlayer->SetState(ActorState::Active);

    int px = mPlayer->GetGridX();
    int py = mPlayer->GetGridY();
    MoveInGrid(mGhostPlayer, px, py);
}

void Level::RemoveGhost() const {
    if (mGhostPlayer) {
        mGhostPlayer->SetState(ActorState::Paused);
        mGhostPlayer->SetVisible(false);
        mGhostPlayer->SetPosition(Vector3(0.0f, 0.0f, -10000.0f));
    }
}

void Level::InitializeIA()
{
    RobotStats playerStats;
    RobotStats enemyStats;

    UpdateIARobotStats(&playerStats, &enemyStats);
    mIA = new IA(playerStats, enemyStats); // lembrar de dar um deletee em algum momento
}

void Level::UpdateIARobotStats(RobotStats *playerStats, RobotStats *enemyStats)
{
    if (!mPlayer || !mEnemy) {
        SDL_Log("UpdateIARobotStats WARNING: Player ou Enemy nulos.");
        return;
    }

    auto FillStats = [&](Robot* robot, RobotStats* outStats)
    {
        outStats->name = robot->GetName();
        outStats->position = Vector2(robot->GetGridX(), robot->GetGridY());

        auto ConvertSkill = [&](SkillType t) -> std::string {
            switch (t) {
                case SkillType::Punch:  return "Punch";
                case SkillType::Missile:return "Missile";
                case SkillType::Dash:   return "Dash";
                case SkillType::Shield: return "Shield";
                case SkillType::Repair: return "Repair";
                default: return "None";
            }
        };

        auto FillPart = [&](const RobotPart& p, PartStats& ps)
        {
            ps.maxHP     = p.maxHP;
            ps.currentHP = p.currentHP;
            ps.damage    = p.damage;
            ps.range     = p.range;
            ps.skill     = ConvertSkill(p.skill);
            ps.name      = p.name;
        };

        FillPart(robot->GetPart(PartSlot::Head),      outStats->head);
        FillPart(robot->GetPart(PartSlot::Torso),     outStats->torso);
        FillPart(robot->GetPart(PartSlot::RightArm),  outStats->rightArm);
        FillPart(robot->GetPart(PartSlot::LeftArm),   outStats->leftArm);
        FillPart(robot->GetPart(PartSlot::Legs),      outStats->legs);
    };

    // Preenche player e enemy
    FillStats(mPlayer, playerStats);
    FillStats(mEnemy,  enemyStats);
}

/*
 * Define a matriz do mapa para a ia tomar decisões:
 * 0 = chão livre, caminhável
 * 1 = parede, bloqueio
 * 2 = destrutível, bloqueio, porém pode ser destruído
 * 3 = mel, terreno especial (efeito)
 * 4 = fogo, terreno especial (efeito)
 */
void Level::UpdateIAGridData()
{
    if (!mGrid)
        return;

    int rows = mGrid->GetRows();
    int cols = mGrid->GetCols();

    mIAGridData.clear();
    mIAGridData.resize(rows, std::vector<int>(cols, 0));

    for (int y = 0; y < rows; y++)
    {
        for (int x = 0; x < cols; x++)
        {
            TerrainType terrain = mGrid->GetTerrainType(x, y);
            Actor* unit = mGrid->GetUnitAt(x, y);

            // ---------- BLOQUEIOS ----------
            if (unit && dynamic_cast<Destructible*>(unit))  // tem que vir primeiro destructible é wall
            {
                mIAGridData[y][x] = 2;     // destrutível
                continue;
            }

            if (terrain == TerrainType::Wall)
            {
                mIAGridData[y][x] = 1;     // parede
                continue;
            }

            // ---------- TERRENOS ESPECIAIS (EFEITOS) ----------
            if (terrain == TerrainType::Honey)
            {
                mIAGridData[y][x] = 3;     // mel
                continue;
            }

            if (terrain == TerrainType::Fire)
            {
                mIAGridData[y][x] = 4;     // fogo
                continue;
            }

            // ---------- TERRENO LIVRE ----------
            mIAGridData[y][x] = 0;
        }
    }
}


void Level::CalculateEnemyAction()
{
    if (!mEnemy)
        return;

    if (!mIA)
        InitializeIA();

    SDL_Log("Calculando ação do inimigo...");

    // Reset
    mEnemyTurn = TurnAction();

    // ---------- CALCULAR AÇÃO DO INIMIGO ----------
    UpdateIAGridData();

    SDL_Log("---------- IA GRID DATA ----------");
    for (int y = 0; y < mIAGridData.size(); y++)
    {
        std::stringstream ss;
        for (int x = 0; x < mIAGridData[y].size(); x++)
        {
            ss << mIAGridData[y][x] << " ";
        }
        SDL_Log("%s", ss.str().c_str());
    }
    SDL_Log("----------------------------------");

    RobotStats playerStats;
    RobotStats enemyStats;
    UpdateIARobotStats(&playerStats, &enemyStats);
    mIA->UpdatePlayerStats(playerStats);
    mIA->UpdateEnemyStats(enemyStats);

    EnemyResolution resolution = mIA->GetEnemyResolution(mIAGridData);

    // ---------- PARSE ACTION ----------
    mEnemyTurn.moveX        = resolution.moveTo.x;
    mEnemyTurn.moveY        = resolution.moveTo.y;
    mEnemyTurn.hasAction    = resolution.hasAction;
    mEnemyTurn.skillSlot    = resolution.skillSlot;
    mEnemyTurn.targetX      = resolution.targetTile.x;
    mEnemyTurn.targetY      = resolution.targetTile.y;

    // logs e notificação na UI
    SDL_Log("Inimigo planejou ir para (%d, %d) e atacar (%d, %d)", mEnemyTurn.moveX, mEnemyTurn.moveY, mEnemyTurn.targetX, mEnemyTurn.targetY);
    std::ostringstream enemyPlan;
    enemyPlan << "Usou: " << resolution.skillUsed;
    NotifyEnemy(enemyPlan.str());
}

void Level::ResolveTurn() {
    CalculateEnemyAction();
    if (mPlayerTurn.moveX != -1) {
        mPlayerTurn.path = mGrid->CalculatePath(
            mPlayer->GetGridX(), mPlayer->GetGridY(),
            mPlayerTurn.moveX, mPlayerTurn.moveY
        );
    }

    if (mEnemy && mEnemyTurn.moveX != -1) {
        mEnemyTurn.path = mGrid->CalculatePath(
            mEnemy->GetGridX(), mEnemy->GetGridY(),
            mEnemyTurn.moveX, mEnemyTurn.moveY
        );
    }

    mStepIndex = 0;
    mIsResolving = true;
    ExecuteNextStep();
}

void Level::OnUpdate(float deltaTime)
{
    // verifica condições de fim de jogo
    if (mBattleState != BattleState::GameOver)
    {
        if (!mPlayer) {
            // mGame->Quit();
            new GameOver(mGame, "../Assets/Fonts/Arial.ttf");
            NotifyPlayer("Jogador derrotado! Fim de jogo.");
            mBattleState = BattleState::GameOver;
        }

        if (!mEnemy) {
            NotifyEnemy("Inimigo derrotado! Jogador vence o nivel!");
            new Win(mGame);
            mBattleState = BattleState::GameOver;
        }
    }

    if (mIsResolving)
    {
        bool playerStopped = (!mPlayer) || !mPlayer->IsMoving();
        bool enemyStopped = (!mEnemy) || (!mEnemy->IsMoving());

        if (playerStopped && enemyStopped)
        {
            ExecuteNextStep();
        }
    }

    if (mBattleState == BattleState::Exploration || mBattleState == BattleState::MoveSelection || mBattleState == BattleState::TargetSelection)
    {
        if (mTileSelection == nullptr)
            mTileSelection = new TileSelection(mGame);
    }
    else
    {
        if (mTileSelection != nullptr)
        {
            mTileSelection->Close();
            mTileSelection = nullptr;
        }
    }

    // se no estado de seleção de ataque
    if (mBattleState == BattleState::SkillSelection)
    {
        if (mActionSelection == nullptr)
            mActionSelection = new ActionSelection(mGame);

    }
    else
    {
        if (mActionSelection != nullptr)
        {
            mActionSelection->Close();
            mActionSelection = nullptr;
        }
    }

    if (mParticleManager)
    {
        mParticleManager->Update(deltaTime);
    }
}

void Level::ExecuteNextStep() {
    bool pHasSteps = mStepIndex < mPlayerTurn.path.size();
    bool eHasSteps = mStepIndex < mEnemyTurn.path.size();

    if (!pHasSteps && !eHasSteps) {
        FinishResolution();
        return;
    }

    Vector2 pNext = Vector2(-100.0f, -100.0f);
    Vector2 pCurr = Vector2(-100.0f, -100.0f);
    Vector2 eNext = Vector2(-200.0f, -200.0f);
    Vector2 eCurr = Vector2(-200.0f, -200.0f);

    if (mPlayer)
    {
        // Onde ele está agora
        pCurr = Vector2((float)mPlayer->GetGridX(), (float)mPlayer->GetGridY());

        // Para onde ele vai (ou fica parado se acabou os passos)
        if (pHasSteps) {
            pNext = mPlayerTurn.path[mStepIndex];
        } else {
            pNext = pCurr;
        }
    }

    if (mEnemy)
    {
        // Onde ele está agora
        eCurr = Vector2((float)mEnemy->GetGridX(), (float)mEnemy->GetGridY());

        // Para onde ele vai
        if (eHasSteps) {
            eNext = mEnemyTurn.path[mStepIndex];
        } else {
            eNext = eCurr;
        }
    }

    bool collision = false;

    if (mPlayer && mEnemy)
    {
        if (pNext.x == eNext.x && pNext.y == eNext.y) {
            collision = true;
        }
        // Colisao com outro parado
        if (pNext.x == eCurr.x && pNext.y == eCurr.y &&
            eNext.x == pCurr.x && eNext.y == pCurr.y)
        {
            collision = true;
        }
    }

    if (collision) {
        std::ostringstream bonkMsg;
        bonkMsg << "BONK no passo " << mStepIndex << "!";
        NotifyBoth(bonkMsg.str());
        mPlayerTurn.path.clear();
        mEnemyTurn.path.clear();
        FinishResolution();
        return;
    }


    float stepTime = 0.3f;

    if (pHasSteps && mPlayer) {
        Vector3 targetPos = mGrid->GetWorldPosition((int)pNext.x, (int)pNext.y);
        mPlayer->StartSmoothMovement(targetPos, stepTime);

        mGrid->SetUnitAt(nullptr, mPlayer->GetGridX(), mPlayer->GetGridY()); // Sai do atual
        mGrid->SetUnitAt(mPlayer, (int)pNext.x, (int)pNext.y);

        mPlayer->UpdateGridCoords((int)pNext.x, (int)pNext.y);

    }

    if (eHasSteps && mEnemy) {
        Vector3 targetPos = mGrid->GetWorldPosition((int)eNext.x, (int)eNext.y);
        mEnemy->StartSmoothMovement(targetPos, stepTime);

        mGrid->SetUnitAt(nullptr, mEnemy->GetGridX(), mEnemy->GetGridY());
        mGrid->SetUnitAt(mEnemy, (int)eNext.x, (int)eNext.y);

        mEnemy->UpdateGridCoords((int)eNext.x, (int)eNext.y);
    }

    mStepIndex++;
}

void Level::StartResolution()
{
    CalculateEnemyAction();

    if (mPlayerTurn.moveX != -1) {
        mPlayerTurn.path = mGrid->CalculatePath(
            mPlayer->GetGridX(), mPlayer->GetGridY(),
            mPlayerTurn.moveX, mPlayerTurn.moveY
        );
    }

    if (mEnemy && mEnemyTurn.moveX != -1) {
        mEnemyTurn.path = mGrid->CalculatePath(
            mEnemy->GetGridX(), mEnemy->GetGridY(),
            mEnemyTurn.moveX, mEnemyTurn.moveY
        );
    }

    mStepIndex = 0;
    mIsResolving = true;

    ExecuteNextStep();
}

void Level::ProcessTileEffects()
{
    SDL_Log("Processando efeitos dos tiles...");
    
    // Processa efeitos para o Player
    if (mPlayer && !mPlayer->IsDead()) {
        int px = mPlayer->GetGridX();
        int py = mPlayer->GetGridY();

        TerrainType terrainType = mGrid->GetTerrainType(px, py);
        
        if (true) {
            switch (terrainType) {
                case TerrainType::Honey: {
                    // Efeito visual de mel pingando
                    HoneyConfig honeyConfig;
                    honeyConfig.color = Vector3(1.0f, 0.75f, 0.1f);         // Amarelo-dourado vibrante
                    honeyConfig.particlesPerSecond = 60;                                // Mais partículas = fluxo contínuo
                    honeyConfig.minSpeed = 30.0f;                                       // Bem devagar
                    honeyConfig.maxSpeed = 80.0f;
                    honeyConfig.particleLifetime = 3.0f;                                // Dura o suficiente para acumular
                    honeyConfig.particleScale = 40.0f;                                  // Menor (gotas, não blocos)
                    honeyConfig.spreadRadius = 120.0f;                                  // Mais espalhado
                    honeyConfig.gravity = -80.0f;                                       // Cai devagar
                    honeyConfig.viscosity = 0.9f;                                      // Bem viscoso

                    mParticleManager->StartHoneyDripAtGrid(px, py, mGrid, honeyConfig);

                    // Aplica stun (não pode se mover no próximo turno)
                    mPlayer->ApplyStatusEffect(StatusEffect::Stunned);
                    NotifyPlayer("Pisou no mel! Não pode se mover no próximo turno!");
                    break;
                }

                case TerrainType::Fire: {
                    // Efeito visual de fogo
                    FireConfig fireConfig;
                    fireConfig.baseColor = Vector3(1.0f, 0.3f, 0.0f);      // Laranja
                    fireConfig.tipColor = Vector3(1.0f, 1.0f, 0.0f);       // Amarelo
                    fireConfig.particlesPerSecond = 70;
                    fireConfig.minSpeed = 100.0f;
                    fireConfig.maxSpeed = 300.0f;
                    fireConfig.particleLifetime = 1.0f;
                    fireConfig.particleScale = 80.0f;
                    fireConfig.spreadRadius = 100.0f;

                    mParticleManager->StartFireAtGrid(px, py, mGrid, fireConfig);

                    // Causa dano
                    {
                        int fireDamage = 10;
                        mPlayer->TakeDamage(fireDamage, PartSlot::Legs);
                        NotifyPlayer("Queimou nas chamas! Recebeu " + std::to_string(fireDamage) + " de dano!");
                        HandleUnitDeath(mPlayer);
                    }
                    break;
                }

                default:
                    //TODO: Remover efeitos
                    mParticleManager->StopHoneyDripAtGrid(px, py);
                    mParticleManager->StopAllFires();
                    break;
            }
        }
    }
    
    // Processa efeitos para o Enemy TODO: Lembrar de considerar os status na IA
    if (mEnemy && !mEnemy->IsDead()) {
        int ex = mEnemy->GetGridX();
        int ey = mEnemy->GetGridY();

        TerrainType terrainType = mGrid->GetTerrainType(ex, ey);
        
        if (true) {
            switch (terrainType) {
                case TerrainType::Honey: {
                    HoneyConfig honeyConfig;
                    honeyConfig.color = Vector3(1.0f, 0.8f, 0.2f);
                    honeyConfig.particlesPerSecond = 20;
                    honeyConfig.viscosity = 0.8f;

                    mParticleManager->StartHoneyDripAtGrid(ex, ey, mGrid, honeyConfig);

                    mEnemy->ApplyStatusEffect(StatusEffect::Stunned);
                    NotifyEnemy("Inimigo pisou no mel! Não pode se mover!");
                    break;
                }

                case TerrainType::Fire:
                {
                    // Efeito visual de fogo
                    FireConfig fireConfig;
                    fireConfig.baseColor = Vector3(1.0f, 0.3f, 0.0f);
                    fireConfig.tipColor = Vector3(1.0f, 0.9f, 0.2f);
                    fireConfig.particlesPerSecond = 35;

                    mParticleManager->StartFireAtGrid(ex, ey, mGrid, fireConfig);

                    int fireDamage = 10;
                    mEnemy->TakeDamage(fireDamage, PartSlot::Legs);
                    NotifyEnemy("Inimigo queimou nas chamas! Recebeu " + std::to_string(fireDamage) + " de dano!");
                    HandleUnitDeath(mEnemy);
                    }
                    break;
                    
                default:
                    mParticleManager->StopHoneyDripAtGrid(ex, ey);
                    mParticleManager->StopFireAtGrid(ex, ey);
                    break;
            }
        }
    }
}

void Level::FinishResolution() {
    NotifyBoth("Movimento finalizado. Resolvendo Habilidades...");
    mIsResolving = false;

    Robot* first = nullptr;
    Robot* second = nullptr;
    TurnAction* act1 = nullptr;
    TurnAction* act2 = nullptr;

    if (mEnemy) {
        int pSpeed = 10; // mPlayer->GetSpeed(); TODO: Definir forma de decidir quem bate primeiro
        int eSpeed = 8;  // mEnemy->GetSpeed();

        if (pSpeed >= eSpeed) {
            first = mPlayer; second = mEnemy;
            act1 = &mPlayerTurn; act2 = &mEnemyTurn;
        } else {
            first = mEnemy; second = mPlayer;
            act1 = &mEnemyTurn; act2 = &mPlayerTurn;
        }
    } else {
        first = mPlayer;
        act1 = &mPlayerTurn;
    }

    // Primeiro Ataca
    if (first && act1->hasAction && !first->IsDead()) {
        first->AttackLocation(act1->targetX, act1->targetY, act1->skillSlot);

        // Checa mortes
        if (mPlayer) HandleUnitDeath(mPlayer);
        if (mEnemy)  HandleUnitDeath(mEnemy);
    }

    // Segundo Ataca (Se ainda existir e estiver vivo)
    bool secondAlive = false;
    if (second == mPlayer) secondAlive = (mPlayer != nullptr);
    else if (second == mEnemy) secondAlive = (mEnemy != nullptr);

    if (secondAlive && second && act2 && act2->hasAction && !second->IsDead()) {
        second->AttackLocation(act2->targetX, act2->targetY, act2->skillSlot);

        if (mPlayer) HandleUnitDeath(mPlayer);
        if (mEnemy)  HandleUnitDeath(mEnemy);
    }

    // Limpa todos os efeitos antes de processar novos
    mParticleManager->StopAllFires();
    mParticleManager->StopAllHoneyDrips();

    // Processa efeitos dos tiles (mel, fogo, etc)
    ProcessTileEffects();

    // Limpeza
    mPlayerTurn = TurnAction();
    mEnemyTurn = TurnAction();

    NotifyBoth("TURNO ENCERRADO.\n\n");
}

void Level::NotifyPlayer(const std::string& message) const
{
    // SDL_Log("%s", message.c_str());
    if (mHud)
    {
        mHud->AddPlayerMessage(message);
    }
}

void Level::NotifyEnemy(const std::string& message) const
{
    SDL_Log("%s", message.c_str());
    if (mHud)
    {
        mHud->AddEnemyMessage(message);
    }
}

void Level::NotifyBoth(const std::string& message) const
{
    SDL_Log("%s", message.c_str());
    if (mHud)
    {
        mHud->AddPlayerMessage(message);
        mHud->AddEnemyMessage(message);
    }
}

bool Level::LoadLevelConfig(const std::string& jsonPath, LevelConfig& config)
{
    std::ifstream file(jsonPath);
    if (!file.is_open()) {
        SDL_Log("ERRO: Nao foi possivel abrir config: %s", jsonPath.c_str());
        return false;
    }

    nlohmann::json j;
    try {
        file >> j;
    } catch (const std::exception& e) {
        SDL_Log("ERRO ao parsear JSON %s: %s", jsonPath.c_str(), e.what());
        return false;
    }

    // Lê os campos do JSON
    if (j.contains("csv")) {
        config.csvPath = j["csv"].get<std::string>();
    }
    
    if (j.contains("theme")) {
        auto theme = j["theme"];
        if (theme.contains("floor")) {
            config.floorTexture = theme["floor"].get<std::string>();
        }
        if (theme.contains("wall")) {
            config.wallTexture = theme["wall"].get<std::string>();
        }
        if (theme.contains("honey")) {
            config.honeyTexture = theme["honey"].get<std::string>();
        }
        if (theme.contains("fire")) {
            config.fireTexture = theme["fire"].get<std::string>();
        }
    }
    
    if (j.contains("destructibles")) {
        for (auto& mesh : j["destructibles"]) {
            config.destructibleMeshes.push_back(mesh.get<std::string>());
        }
    }
    
    if (j.contains("music")) {
        config.musicPath = j["music"].get<std::string>();
    }

    SDL_Log("Config carregado: CSV=%s, Floor=%s, Wall=%s, Music=%s", 
            config.csvPath.c_str(), 
            config.floorTexture.c_str(),
            config.wallTexture.c_str(),
            config.musicPath.c_str());

    return true;
}

void Level::LoadLevel(const LevelConfig& config)
{
    // Salva a config
    mLevelConfig = config;
    
    std::ifstream file(config.csvPath);
    if (!file.is_open()) {
        SDL_Log("ERRO FATAL: Nao foi possivel abrir o level: %s", config.csvPath.c_str());
        return;
    }

    // Ler os dados do CSV
    std::vector<std::vector<int>> mapData;
    std::string line;

    while (std::getline(file, line))
    {
        std::vector<int> row;
        std::stringstream ss(line);
        std::string cell;

        while (std::getline(ss, cell, ','))
        {
            if (!cell.empty()) {
                row.push_back(std::stoi(cell));
            }
        }
        if (!row.empty()) {
            mapData.push_back(row);
        }
    }

    int rows = mapData.size();
    int cols = (rows > 0) ? mapData[0].size() : 0;

    SDL_Log("LoadLevel: Lido %d linhas, %d colunas do CSV", rows, cols);

    if (rows == 0 || cols == 0) {
        SDL_Log("ERRO: CSV vazio ou invalido!");
        return;
    }

    // Calcular bounding box da área útil (células != -1 e != 0)
    int minX = cols, maxX = -1;
    int minY = rows, maxY = -1;
    
    for (int y = 0; y < rows; y++) {
        for (int x = 0; x < cols; x++) {
            if (mapData[y][x] != -1 && mapData[y][x] != 0) {
                minX = std::min(minX, x);
                maxX = std::max(maxX, x);
                minY = std::min(minY, y);
                maxY = std::max(maxY, y);
            }
        }
    }
    
    // Se não encontrou nenhuma célula válida, usa o mapa completo
    if (maxX < minX) {
        minX = 0;
        maxX = cols - 1;
        minY = 0;
        maxY = rows - 1;
    }
    
    int gridRows = maxY - minY + 1;
    int gridCols = maxX - minX + 1;

    // Criar todos os Blocks (renderizados primeiro)
    SDL_Log("Criando Blocks de chao e paredes...");
    
    // Armazena Blocks de chão e destrutíveis para guardar referências depois
    std::vector<std::pair<Vector2, Actor*>> floorBlockRefs;
    
    for (int y = minY; y <= maxY; y++)
    {
        for (int x = minX; x <= maxX; x++)
        {
            int tileID = mapData[y][x];
            int gridX = x - minX;
            int gridY = y - minY;
            
            Vector3 basePos = Vector3(
                (gridX - gridCols / 2.0f) * 500.0f + 250.0f,
                (gridY - gridRows / 2.0f) * 500.0f + 250.0f,
                FLOOR_Z
            );

            switch (tileID)
            {
                case TILE_VOID:
                case TILE_EMPTY:
                    break;
                    
                case TILE_FLOOR:
                case TILE_PLAYER_SPAWN:
                case TILE_ENEMY_SPAWN:
                {
                    auto* floor = new Block(mGame);
                    floor->SetPosition(basePos);
                    floor->SetScale(TILE_SCALE);
                    floor->SetTexture(mLevelConfig.floorTexture);
                    floorBlockRefs.emplace_back(Vector2(gridX, gridY), floor);
                }
                break;
                
                case TILE_HONEY:
                {
                    auto* honeyFloor = new Block(mGame);
                    honeyFloor->SetPosition(basePos);
                    honeyFloor->SetScale(TILE_SCALE);
                    honeyFloor->SetTexture(mLevelConfig.honeyTexture);
                    floorBlockRefs.emplace_back(Vector2(gridX, gridY), honeyFloor);
                }
                break;
                
                case TILE_FIRE:
                {
                    auto* fireFloor = new Block(mGame);
                    fireFloor->SetPosition(basePos);
                    fireFloor->SetScale(TILE_SCALE);
                    fireFloor->SetTexture(mLevelConfig.fireTexture);
                    floorBlockRefs.emplace_back(Vector2(gridX, gridY), fireFloor);
                }
                break;
                
                case TILE_DESTRUCTIBLE:
                {
                    // Chão base
                    auto* floor = new Block(mGame);
                    floor->SetPosition(basePos);
                    floor->SetScale(TILE_SCALE);
                    floor->SetTexture(mLevelConfig.floorTexture);
                    floorBlockRefs.emplace_back(Vector2(gridX, gridY), floor);
                    
                    // Objeto destrutível em cima
                    if (!mLevelConfig.destructibleMeshes.empty()) {
                        auto* destructible = new Destructible(mGame);
                        
                        // Usa mesma posição E escala do tile
                        Vector3 objPos = basePos;
                        objPos.z = -250; // acima e menor
                        
                        destructible->SetPosition(objPos);
                        destructible->SetScale(TILE_SCALE);
                        
                        // Escolhe um mesh aleatório da lista
                        int randomIndex = rand() % mLevelConfig.destructibleMeshes.size();
                        std::string meshPath = mLevelConfig.destructibleMeshes[randomIndex];
                        auto* mesh = mGame->GetRenderer()->GetMesh(meshPath);
                        destructible->SetMesh(mesh);
                        
                        // Guarda referência para registrar no GridMap depois
                        floorBlockRefs.emplace_back(Vector2(gridX, gridY), destructible);
                    }
                }
                break;
                
                case TILE_WALL:
                {
                    // Chão base (não metálico)
                    auto* floor = new Block(mGame, 0.0f);
                    floor->SetPosition(basePos);
                    floor->SetScale(TILE_SCALE);
                    floor->SetTexture(mLevelConfig.floorTexture);
                    
                    // Parede em cima (metálica)
                    auto* wall = new Block(mGame, 0.6f);
                    Vector3 wallPos = basePos;
                    wallPos.z = WALL_Z;
                    wall->SetPosition(wallPos);
                    wall->SetScale(TILE_SCALE);
                    wall->SetTexture(mLevelConfig.wallTexture);
                }
                break;

                default:
                    break;
            }
        }
    }

    // Criar GridMap e configurar
    if (mGrid) {
        SDL_Log("Deletando GridMap anterior...");
        delete mGrid;
        mGrid = nullptr;
    }

    SDL_Log("Criando GridMap %dx%d e configurando tiles...", gridRows, gridCols);
    mGrid = new GridMap(mGame, gridRows, gridCols, 500.0f);
    
    // Guardar referências dos Blocks no GridMap
    for (const auto& [pos, block] : floorBlockRefs) {
        mGrid->SetFloorBlock(pos.x, pos.y, block);
    }

    // Configurar TerrainTypes, Tiles visuais e spawns
    for (int y = minY; y <= maxY; y++)
    {
        for (int x = minX; x <= maxX; x++)
        {
            int tileID = mapData[y][x];
            int gridX = x - minX;
            int gridY = y - minY;
            
            Tile* tile = mGrid->GetTileAt(gridX, gridY);

            switch (tileID)
            {
                case TILE_VOID:
                case TILE_EMPTY:
                    if (tile) {
                        tile->SetState(ActorState::Paused);
                    }
                    break;

                case TILE_FLOOR:
                    if (tile) {
                        tile->SetState(ActorState::Active);
                    }
                    mGrid->SetTerrainType(gridX, gridY, TerrainType::Floor);
                    break;
                
                case TILE_WALL:
                    if (tile) {
                        tile->SetState(ActorState::Active);
                    }
                    mGrid->SetTerrainType(gridX, gridY, TerrainType::Wall);
                    break;
                
                case TILE_FIRE:
                    if (tile) {
                        tile->SetState(ActorState::Active);
                    }
                    mGrid->SetTerrainType(gridX, gridY, TerrainType::Fire);
                    break;
                
                case TILE_DESTRUCTIBLE:
                {
                    if (tile) {
                        tile->SetState(ActorState::Active);
                    }
                    mGrid->SetTerrainType(gridX, gridY, TerrainType::Wall);  // Bloqueia movimento como parede
                    
                    // Encontra o destrutível criado anteriormente para este gridX, gridY
                    for (const auto& [pos, actor] : floorBlockRefs) {
                        if (pos.x == gridX && pos.y == gridY) {
                            if (dynamic_cast<Destructible*>(actor)) {
                                mGrid->SetUnitAt(actor, gridX, gridY);
                                break;
                            }
                        }
                    }
                }
                break;
                
                case TILE_PLAYER_SPAWN:
                    if (tile) {
                        tile->SetState(ActorState::Active);
                    }
                    mGrid->SetTerrainType(gridX, gridY, TerrainType::Floor);
                    mPlayer->UpdateGridCoords(gridX, gridY);
                    mGrid->SetUnitAt(mPlayer, gridX, gridY);
                    MoveInGrid(mPlayer, gridX, gridY);
                    break;

                case TILE_ENEMY_SPAWN:
                    if (tile) {
                        tile->SetState(ActorState::Active);
                    }
                    mGrid->SetTerrainType(gridX, gridY, TerrainType::Floor);
                    mEnemy->UpdateGridCoords(gridX, gridY);
                    mGrid->SetUnitAt(mEnemy, gridX, gridY);
                    MoveInGrid(mEnemy, gridX, gridY);
                    break;
                
                case TILE_HONEY:
                    if (tile) {
                        tile->SetState(ActorState::Active);
                    }
                    mGrid->SetTerrainType(gridX, gridY, TerrainType::Honey);
                    break;

                default:
                    break;
            }
        }
    }

    SDL_Log("Level carregado: %d x %d", rows, cols);
}

Level::~Level() {
    // Limpa ParticleManager
    delete mParticleManager;
    mParticleManager = nullptr;
}
