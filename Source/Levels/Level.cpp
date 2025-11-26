//
// Created by mateus on 25/11/2025.
//

#include "Level.h"
#include "Game.h"

#include "Actors/Block.h"
//TODO Lidar com a morte dos robos e entender como ficou os movimentos
Level::Level(class Game *game, HUD *hud) :
    mGame(game),
    mCamera(mGame->GetCamera()),
    mHud(hud),
    mCursor(nullptr),
    mGrid(nullptr),
    mBattleState(BattleState::Null),
    mSelectedSlot(PartSlot::Null),
    mPlayer(nullptr),
    enemyUnit(nullptr)
{
    mBattleState = BattleState::Exploration;

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
    mPlayer->SetName("CryingBee");

    mPlayer->EquipPart(PartSlot::RightArm,
                          RobotPart("Iron Fist", "../Assets/Cube.gpmesh",
                                    50, SkillType::Punch, 30, 1));

    mPlayer->EquipPart(PartSlot::LeftArm,
                          RobotPart("Thunder Beam", "../Assets/Cube.gpmesh",
                                    50, SkillType::Missile, 40, 3));

    mPlayer->UpdateGridCoords(1, 1);
    mGrid->SetUnitAt(mPlayer, 1, 1);

    MoveInGrid(mPlayer, 1, 1);

    // cursor no seu personagem
    MoveInGrid(mCursor, 1, 1);

    // ENEMY
    enemyUnit = new Robot(game, Team::Enemy);
    enemyUnit->SetName("EvilBee");

    enemyUnit->EquipPart(PartSlot::Torso,
                         RobotPart("Heavy Armor", "../Assets/Cube.gpmesh",
                                   100, SkillType::None, 0, 0));

    enemyUnit->UpdateGridCoords(2, 2);
    mGrid->SetUnitAt(enemyUnit, 2, 2);
    MoveInGrid(enemyUnit, 2, 2);
}

Level::~Level()
{

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

void Level::ProcessInput(SDL_Event &event)
{
    int key = event.key.keysym.sym;

    switch (key)
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
        // case SDLK_RETURN:
            SDL_Log("Selecionou Tile: %d, %d", mCursor->GetGridX(), mCursor->GetGridY());
            HandleAction();
            break;

        case SDLK_BACKSPACE:
        case SDLK_ESCAPE:
            HandleCancel();
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
    }
}

void Level::SpawnFloor(int rows, int cols)
{
    //SDL_Log("SPAWNWALLS -> Rows: %d, Cols: %d", rows, cols);
    const float spacing = 500.0f;

    float totalWidth = cols * spacing;
    float totalHeight = rows * spacing;

    float startX = -totalWidth / 2.0f;
    float startY = -totalHeight / 2.0f;

    float zPos = -500.0f; // Centro do cubo

    for (int y = 0; y < rows; y++)
    {
        for (int x = 0; x < cols; x++)
        {
            Block* wall = new Block(mGame);
            wall->SetScale(Vector3(spacing, spacing, spacing)); // cubo tamanho 1x1 -> vira 500x500

            Vector3 pos;
            // Move o ponto da "borda" para o "centro" do bloco
            pos.x = startX + (x * spacing) + (spacing * 0.5f);
            pos.y = startY + (y * spacing) + (spacing * 0.5f);
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
    }
}

void Level::HandleExplorationPhase()
{
    if (mPlayer->GetGridX() == mCursor->GetGridX() &&
                mPlayer->GetGridY() == mCursor->GetGridY())
    {
        SetBattleState(BattleState::MoveSelection);

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
    Tile* targetVisual = mGrid->GetTileAt(mCursor->GetGridX(), mCursor->GetGridY());
    if (targetVisual && targetVisual->GetType() == TileType::Path)
    {
        // Verifica se não tem ninguém no destino (exceto eu mesmo)
        Actor* obstacle = mGrid->GetUnitAt(mCursor->GetGridX(), mCursor->GetGridY());
        if (obstacle && obstacle != mPlayer) {
            SDL_Log("Tile ocupado!");
            return;
        }

        // Move
        // Salva a posição para UNDO
        mPlayer->SaveGridPosition();

        // Move o player
        MoveInGrid(mPlayer, mCursor->GetGridX(), mCursor->GetGridY());

        // Limpa o azul do chão e troca o estado
        mGrid->ClearTileStates();
        SetBattleState(BattleState::SkillSelection);

        // Setup padrão de parte selecionada
        SetSelectedSlot(GetSelectedSlot());

        SDL_Log("Movimento concluido.");
        SDL_Log("PRESSIONE '1' PARA BRACO DIREITO");
        SDL_Log("PRESSIONE '2' PARA BRACO ESQUERDO");
    }
    else {
        SDL_Log("Movimento invalido, muito distante.");
    }
}


void Level::HandleSkillSelectionPhase(PartSlot slot)
{
    SetSelectedSlot(slot);
    mGrid->ClearTileStates();

    int range = mPlayer->GetPartRange(slot);

    // Range da skill em vermelho
    auto tiles = mGrid->GetAttackableTiles(
        mPlayer->GetGridX(), // Origem X é o Robô
        mPlayer->GetGridY(), // Origem Y é o Robô
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
        mPlayer->AttackLocation(mCursor->GetGridX(), mCursor->GetGridY(), GetSelectedSlot());

        // Checagem de morte (TODO: mover para a fase de resolucao quando existir)
        HandleUnitDeath(enemyUnit);
        HandleUnitDeath(mPlayer);

        // Limpa o vermelho e troca de estado
        mGrid->ClearTileStates();
        SetBattleState(BattleState::Exploration);

        SDL_Log("Ataque realizado no tile (%d, %d). Fim do turno.", mCursor->GetGridX(), mCursor->GetGridY());
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
        robot = nullptr;
        SDL_Log("Level removeu o robo do jogo.");
    }
}

void Level::HandleCancel()
{
    // UNDO SELECTION
    switch (mBattleState) {
        case BattleState::MoveSelection:
        {
            mGrid->ClearTileStates();
            MoveInGrid(mCursor, mPlayer->GetGridX(), mPlayer->GetGridY());
            SetBattleState(BattleState::Exploration);
            SDL_Log("Selecao cancelada.");
            break;
        }

        case BattleState::SkillSelection:
        {
            // Recupera o Player
            int restoreX = mPlayer->GetSavedX();
            int restoreY = mPlayer->GetSavedY();

            // Move o Player de volta
            MoveInGrid(mPlayer, restoreX, restoreY);

            // Sincroniza o cursor
            MoveInGrid(mCursor, restoreX, restoreY);
            mGrid->SetSelectedTile(restoreX, restoreY);

            SetBattleState(BattleState::Exploration);

            SDL_Log("Movimento desfeito (Robo voltou para %d, %d).", mPlayer->GetSavedX(), mPlayer->GetSavedY());
            break;
        }

        case BattleState::TargetSelection:
        {
            mGrid->ClearTileStates();
            mGrid->ClearTileStates();

            int px = mPlayer->GetGridX();
            int py = mPlayer->GetGridY();

            MoveInGrid(mCursor, px, py);
            mGrid->SetSelectedTile(px, py);

            SetBattleState(BattleState::SkillSelection);
            SDL_Log("Mira cancelada. Escolha outra habilidade (1 ou 2).");
            break;
        }
    }
}


void Level::MoveInGrid(Actor *actor, int x, int y)
{
    if (!actor) return;

    // Atualiza o Visual (mundo 3D)
    Vector3 worldPos = mGrid->GetWorldPosition(x, y);

    // Ajuste do offset de altura
    worldPos.z = OFFSET_Z;
    actor->SetPosition(worldPos);

    // Atualiza o Grid Lógico
    Robot* robot = dynamic_cast<Robot*>(actor);
    if (robot)
    {
        mGrid->SetUnitAt(robot, x, y);  // atualizar a informacao do robo no vector da grid
        robot->UpdateGridCoords(x, y);
    }
    else if (actor == mCursor)
    {
        mCursor->UpdateGridCoords(x, y);
        mGrid->SetSelectedTile(mCursor->GetGridX(), mCursor->GetGridY());   // visual
    }
}



