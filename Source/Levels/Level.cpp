//
// Created by mateus on 25/11/2025.
//

#include "Level.h"

#include <algorithm>

#include "Game.h"

#include "Actors/Block.h"

//TODO: Movimento, uma grid por tempo

Level::Level(class Game *game, HUD *hud) :
    mGame(game),
    mCamera(mGame->GetCamera()),
    mHud(hud),
    mCursor(nullptr),
    mGrid(nullptr),
    mBattleState(BattleState::Null),
    mSelectedSlot(PartSlot::Null),
    mPlayer(nullptr),
    mEnemy(nullptr),
    mGhostPlayer(nullptr),
    mIsResolving(false),
    mStepIndex(0)
{
    mBattleState = BattleState::Exploration;
    // ost
    mLevelMusic = mGame->GetAudio()->PlaySound("Backpullver-Shamanez-Overnight.wav", true);
    // hud
    // mHUD = new HUD(this, "../Assets/Fonts/Arial.ttf");
    // mHUD->SetScore(0);

    // create camera
    Vector3 eye(0.0f, -2500.0f, 1000.0f);
    Vector3 target(0.0f, 0.0f, 0.0f);
    Vector3 up(0.0f, 0.0f, 1.0f);
    mCamera = new Camera(game, eye, target, up, 70.0f, 10.0f, 10000.0f);
    mGame->SetCamera(mCamera);

    // spawn floor
    SpawnFloor(ROWS, COLS);

    // grid
    mGrid = new GridMap(game, ROWS, COLS, SIZE);

    // cursor
    mCursor = new GridCursor(game);
    // mCursor->Move(0, 0);

    // PLAYER
    mPlayer = new Robot(game, Team::Player);
    mPlayer->SetName("BeaBee");

    mPlayer->EquipPart(PartSlot::Torso,
                         RobotPart("Honey Chest", "../Assets/Robots/Robota/RobotaTorso.gpmesh",
                                   100, SkillType::None, 0, 0));
    mPlayer->EquipPart(PartSlot::RightArm,
                          RobotPart("Robota Dustpan", "../Assets/Robots/Robota/RobotaRightArm.gpmesh",
                                    1000, SkillType::Missile, 1000, 3));

    mPlayer->EquipPart(PartSlot::LeftArm,
                          RobotPart("Robota Broom", "../Assets/Robots/Robota/RobotaLeftArm.gpmesh",
                                    10, SkillType::Punch, 50, 1));

    mPlayer->EquipPart(PartSlot::Legs,
                          RobotPart("Robota Legs", "../Assets/Robots/Robota/RobotaLeg.gpmesh",
                                    30, SkillType::None, 0, 2));

    mPlayer->EquipPart(PartSlot::Head,
                          RobotPart("Robota Head", "../Assets/Robots/Robota/RobotaHead.gpmesh",
                                    30, SkillType::Repair, 0, 0));

    // Comeca com o brado direito escolhido
    SetSelectedSlot(PartSlot::RightArm);
    mPlayer->UpdateGridCoords(1, 1);
    mGrid->SetUnitAt(mPlayer, 1, 1);

    MoveInGrid(mPlayer, 1, 1);

    // cursor no seu personagem
    MoveInGrid(mCursor, 1, 1);

    // ENEMY
    mEnemy = new Robot(game, Team::Enemy);
    mEnemy->SetName("EvilBee");

    mEnemy->EquipPart(PartSlot::Torso,
                     RobotPart("Honey Chest", "../Assets/Robots/BeaBee/BeaBeeTorso.gpmesh",
                               100, SkillType::None, 0, 0));
    mEnemy->EquipPart(PartSlot::RightArm,
                          RobotPart("Honey Blast", "../Assets/Robots/BeaBee/BeaBeeRightArm.gpmesh",
                                    10, SkillType::Missile, 20, 3));

    mEnemy->EquipPart(PartSlot::LeftArm,
                          RobotPart("Queen's Drill", "../Assets/Robots/BeaBee/BeaBeeLeftArm.gpmesh",
                                    10, SkillType::Punch, 50, 1));

    mEnemy->EquipPart(PartSlot::Legs,
                          RobotPart("Honey Boots", "../Assets/Robots/BeaBee/BeaBeeLeftLeg.gpmesh",
                                    30, SkillType::None, 0, 2));

    mEnemy->EquipPart(PartSlot::Head,
                          RobotPart("Queen's Crown", "../Assets/Robots/BeaBee/BeaBeeHead.gpmesh",
                                    30, SkillType::Repair, 0, 0));

    mEnemy->UpdateGridCoords(2, 2);
    mGrid->SetUnitAt(mEnemy, 2, 2);
    MoveInGrid(mEnemy, 2, 2);
}


// TODO: ajustar a posição inicial do cursor para a do robo
void Level::MoveCursor(int xOffset, int yOffset)
{
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

void Level::ProcessInput(const SDL_Event &event)
{

    switch (event.key.keysym.sym)
    {
        case SDLK_w:
        case SDLK_UP:
            MoveCursor(0, 1);
            break;

        case SDLK_s:
        case SDLK_DOWN:
            MoveCursor(0, -1);
            break;

        case SDLK_a:
        case SDLK_LEFT:
            MoveCursor(1, 0);
            break;

        case SDLK_d:
        case SDLK_RIGHT:
            MoveCursor(-1, 0);
            break;

        case SDLK_SPACE:
            HandleAction();
            break;

        case SDLK_BACKSPACE:
        case SDLK_ESCAPE:
            HandleCancel();
            break;

        case SDLK_q:
            if (mBattleState == BattleState::SkillSelection) {
                HandleWait();
            }
            break;

        case SDLK_1:
        case SDLK_KP_1:
            if (mBattleState == BattleState::SkillSelection) {
                SetSelectedSlot(PartSlot::RightArm);
                SDL_Log(">> Selecionado: Braço Direito");
                SDL_Log(">> Enter/Espaço para confirmar");
            }
            break;

        case SDLK_2:
        case SDLK_KP_2:
            if (mBattleState == BattleState::SkillSelection) {
                SetSelectedSlot(PartSlot::LeftArm);
                SDL_Log(">> Selecionado: Braço Esquerdo");
                SDL_Log(">> Enter/Espaço para confirmar");
            }
            break;

        default:
            break;
    }
}

void Level::SpawnFloor(int rows, int cols) const {
    //SDL_Log("SPAWNWALLS -> Rows: %d, Cols: %d", rows, cols);
    constexpr float spacing = 500.0f;

    float totalWidth = (float)cols * spacing;
    float totalHeight = (float)rows * spacing;

    float startX = -totalWidth / 2.0f;
    float startY = -totalHeight / 2.0f;

    for (int y = 0; y < rows; y++)
    {
        for (int x = 0; x < cols; x++)
        {
            float zPos = -500.0f;
            auto wall = new Block(mGame);
            wall->SetScale(Vector3(spacing, spacing, spacing)); // cubo tamanho 1x1 -> vira 500x500

            Vector3 pos;
            // Move o ponto da "borda" para o "centro" do bloco
            pos.x = startX + ((float)x * spacing) + (spacing * 0.5f);
            pos.y = startY + ((float)y * spacing) + (spacing * 0.5f);
            pos.z = zPos;

            wall->SetPosition(pos);
            wall->SetTexture(0);
        }
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
        SetBattleState(BattleState::MoveSelection);
        // Ghost
        SpawnGhost();

        // BFS azul
        int range = mPlayer->GetMovementRange();
        auto tiles = mGrid->GetWalkableTiles(mPlayer->GetGridX(), mPlayer->GetGridY(), range);

        for (const auto& node : tiles) {
            Tile* t = mGrid->GetTileAt(node.x, node.y);
            if (t) t->SetTileType(TileType::Path);
        }

        SDL_Log("Robo selecionado! Escolha o destino.");
    }
    else
    {
        SDL_Log("Essa unidade nao e sua.");
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
            SDL_Log("Tile ocupado!");
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

        SDL_Log("Movimento concluido.");
        SDL_Log("PRESSIONE '1' PARA BRACO DIREITO");
        SDL_Log("PRESSIONE '2' PARA BRACO ESQUERDO");
        SDL_Log("Ou PRESSIONE 'Q' para ESPERAR.");
    }
    else {
        SDL_Log("Movimento invalido, muito distante.");
    }
}


void Level::HandleSkillSelectionPhase(PartSlot slot)
{
    const RobotPart& part = mPlayer->GetPart(slot);
    if (part.isBroken)
    {
        SDL_Log("ERRO: A parte '%s' esta QUEBRADA! Escolha outra.", part.name.c_str());
        return;
    }
    SetSelectedSlot(slot);
    mGrid->ClearTileStates();

    int range = mPlayer->GetPartRange(slot);

    // Range da skill em vermelho
    auto tiles = mGrid->GetAttackableTiles(
        mGhostPlayer->GetGridX(),
        mGhostPlayer->GetGridY(), // Origem Y é o Robô
        1,
        range
    );

    for (const auto& node : tiles) {
        Tile* t = mGrid->GetTileAt(node.x, node.y);
        if (t) t->SetTileType(TileType::Attack);
    }

    SetBattleState(BattleState::TargetSelection);
    SDL_Log("Habilidade escolhida, selecione o alvo.");
}

void Level::HandleTargetingPhase()
{
    Tile* targetTile = mGrid->GetTileAt(mCursor->GetGridX(), mCursor->GetGridY());

    if (targetTile && targetTile->GetType() == TileType::Attack) {
        mPlayerTurn.hasAction = true;
        mPlayerTurn.skillSlot = GetSelectedSlot();
        mPlayerTurn.targetX = mCursor->GetGridX();
        mPlayerTurn.targetY = mCursor->GetGridY();


        // Limpa o vermelho e troca de estado
        mGrid->ClearTileStates();
        RemoveGhost();
        SetBattleState(BattleState::Exploration);

        SDL_Log("=== PLANEJAMENTO CONCLUIDO. INICIANDO RESOLUCAO ===");
        StartResolution();
    }else {
        SDL_Log("Alvo invalido! Selecione um quadrado dentro da range.");
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
        SDL_Log("Level removeu o robo do jogo.");
    }
}

void Level::HandleCancel()
{
    if (!mPlayer) return;
    // UNDO SELECTION
    switch (mBattleState) {
        case BattleState::MoveSelection:
        {
            mGrid->ClearTileStates();
            RemoveGhost();
            SetBattleState(BattleState::Exploration);
            SDL_Log("Selecao cancelada.");
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
                if (t) t->SetTileType(TileType::Path);
            }

            SDL_Log("Movimento do fantasma desfeito.");
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
            SDL_Log("Mira cancelada. Escolha outra habilidade (1 ou 2).");
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
    SDL_Log("Jogador escolheu ESPERAR.");
    SDL_Log("=== INICIANDO RESOLUCAO ===");
    CalculateEnemyAction();
    ResolveTurn();
    SDL_Log("=== FIM DO TURNO ===");
}

void Level::MoveInGrid(Actor *actor, int x, int y) const {
    if (!actor) return;

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

void Level::SpawnGhost() {
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

void Level::CalculateEnemyAction() {
    if  (!mEnemy) return;

    // Reset
    mEnemyTurn = TurnAction();

    int ex = mEnemy->GetGridX();
    int ey = mEnemy->GetGridY();
    int px = mPlayer->GetGridX();
    int py = mPlayer->GetGridY();

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
    if (!mPlayer) {
        mGame->Quit();
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
        SDL_Log("BONK no passo %d!", mStepIndex);
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

void Level::StartResolution() {
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


void Level::FinishResolution() {
    SDL_Log("Movimento finalizado. Resolvendo Habilidades...");
    mIsResolving = false;

    Robot* first = nullptr;
    Robot* second = nullptr;
    TurnAction* act1 = nullptr;
    TurnAction* act2 = nullptr;

    if (mEnemy) {
        int pSpeed = 10; // mPlayer->GetSpeed();
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

    // Limpeza
    mPlayerTurn = TurnAction();
    mEnemyTurn = TurnAction();

    SDL_Log("TURNO ENCERRADO.");
}




