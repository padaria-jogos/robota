//
// Created by mateus on 25/11/2025.
//

#include "Level.h"
#include "Game.h"

#include "Actors/Block.h"

Level::Level(class Game *game, HUD *hud) :
    mGame(game),
    mCamera(mGame->GetCamera()),
    mHud(hud),
    mCursor(nullptr),
    mGrid(nullptr),
    mBattleState(BattleState::Null),
    mSelectedUnit(nullptr),
    mSelectedSlot(PartSlot::Null),
    playerUnit(nullptr),
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
    playerUnit = new Robot(game, Team::Player);
    playerUnit->SetName("CryingBee");

    playerUnit->EquipPart(PartSlot::RightArm,
                          RobotPart("Iron Fist", "../Assets/Cube.gpmesh",
                                    50, SkillType::Punch, 30, 1));

    playerUnit->EquipPart(PartSlot::LeftArm,
                          RobotPart("Thunder Beam", "../Assets/Cube.gpmesh",
                                    50, SkillType::Missile, 40, 3));

    playerUnit->SetGridPosition(1, 1);
    mGrid->SetUnitAt(playerUnit, 1, 1);

    mLastTileActorPositionX = mGrid->GetTilePosition(playerUnit->GetPosition().x, playerUnit->GetPosition().y).x;
    mLastTileActorPositionY = mGrid->GetTilePosition(playerUnit->GetPosition().x, playerUnit->GetPosition().y).y;
    MoveInGrid(playerUnit, 1, 1);

    // cursor no seu personagem
    MoveInGrid(mCursor, 1, 1);
    mTileCursorPositionX = 1;
    mTileCursorPositionY = 1;

    // ENEMY
    enemyUnit = new Robot(game, Team::Enemy);
    enemyUnit->SetName("EvilBee");

    enemyUnit->EquipPart(PartSlot::Torso,
                         RobotPart("Heavy Armor", "../Assets/Cube.gpmesh",
                                   100, SkillType::None, 0, 0));

    enemyUnit->SetGridPosition(2, 2);
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
    if (!mGrid) return;
    Vector3 gridPos = mGrid->GetTilePosition(mCursor->GetPosition().x, mCursor->GetPosition().y);

    int newX = gridPos.x + xOffset;
    int newY = gridPos.y + yOffset;

    // Verificar se saiu do mapa (Clamp)
    if (newX < 0) newX = 0;
    if (newX >= mGrid->GetCols()) newX = mGrid->GetCols() - 1;
    if (newY < 0) newY = 0;
    if (newY >= mGrid->GetRows()) newY = mGrid->GetRows() - 1;

    mTileCursorPositionX = newX;
    mTileCursorPositionY = newY;

    mCursor->SetPosition(mGrid->GetWorldPosition(newX, newY));
    mGrid->SetSelectedTile(newX, newY);
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
            SDL_Log("Selecionou Tile: %d, %d", mTileCursorPositionX, mTileCursorPositionY);
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
    if (!mGrid) return;
    if (!playerUnit) return;

    // EXPLORAÇÃO
    if (mBattleState == BattleState::Exploration)
    {
        if (playerUnit->GetGridX() == mTileCursorPositionX &&
            playerUnit->GetGridY() == mTileCursorPositionY)
        {
            SetSelectedUnit(playerUnit);
            SetBattleState(BattleState::MoveSelection);

            // BFS azul
            int range = playerUnit->GetMovementRange();
            auto tiles = mGrid->GetWalkableTiles(mTileCursorPositionX, mTileCursorPositionY, range);

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

    // MOVIMENTO
    else if (mBattleState == BattleState::MoveSelection)
    {
        if (!mSelectedUnit) return;

        Tile* targetVisual = mGrid->GetTileAt(mTileCursorPositionX, mTileCursorPositionY);
        if (targetVisual && targetVisual->GetType() == TileType::Path)
        {
            // Verifica se não tem ninguém no destino (exceto eu mesmo)
            Actor* obstacle = mGrid->GetUnitAt(mTileCursorPositionX, mTileCursorPositionY);
            if (obstacle && obstacle != mSelectedUnit) {
                SDL_Log("Tile ocupado!");
                return;
            }

            // Move
            // TODO: algo melhor que isso
            mLastTileActorPositionX = mGrid->GetTilePosition(playerUnit->GetPosition().x, playerUnit->GetPosition().y).x;
            mLastTileActorPositionY = mGrid->GetTilePosition(playerUnit->GetPosition().x, playerUnit->GetPosition().y).y;
            MoveInGrid(mSelectedUnit, mTileCursorPositionX, mTileCursorPositionY);
            mGrid->ClearTileStates(); // Apaga o azul do mapa
            SetBattleState(BattleState::SkillSelection);

            // Padrao ultimo utilizado
            SetSelectedSlot(GetSelectedSlot());

            SDL_Log("Movimento concluido.");
            SDL_Log("PRESSIONE '1' PARA BRACO DIREITO");
            SDL_Log("PRESSIONE '2' PARA BRACO ESQUERDO");
        }
        else {
            SDL_Log("Movimento invalido, muito distante.");
        }
    }

    // Seleção de habilidade
    else if (mBattleState == BattleState::SkillSelection) {
        SelectSkill(GetSelectedSlot());
    }

    else if (mBattleState == BattleState::TargetSelection) {
        Tile* targetTile = mGrid->GetTileAt(mTileCursorPositionX, mTileCursorPositionY);

        if (targetTile && targetTile->GetType() == TileType::Attack) {
            Robot* attacker = GetSelectedUnit();
            PartSlot slotToUse = PartSlot::RightArm;

            attacker->AttackLocation(mTileCursorPositionX, mTileCursorPositionY, slotToUse);

            mGrid->ClearTileStates();
            SetBattleState(BattleState::Exploration);
            SetSelectedUnit(nullptr);

            SDL_Log("Ataque realizado no tile (%d, %d). Fim do turno.", mTileCursorPositionX, mTileCursorPositionY);
        }else {
            SDL_Log("Alvo invalido! Selecione um quadrado dentro da range.");
        }
    }
}

void Level::SelectSkill(PartSlot slot)
{
    SetSelectedSlot(slot);

    int range = playerUnit->GetPartRange(slot);

    // Range da skill em vermelho
    auto tiles = mGrid->GetAttackableTiles(mTileCursorPositionX, mTileCursorPositionY, 1, range); // TODO: o min pode ser 0 para habildades de raparo

    SDL_Log("%d", mTileCursorPositionX);
    SDL_Log("%d", mTileCursorPositionY);
    for (const auto& node : tiles) {
        Tile* t = mGrid->GetTileAt(node.x, node.y);
        if (t) t->SetTileType(TileType::Attack);
    }

    SetBattleState(BattleState::TargetSelection);
    SDL_Log("Habilidade escolhida, selecione o alvo.");
}

void Level::HandleCancel()
{
    // UNDO SELECTION
    if (mBattleState == BattleState::MoveSelection)
    {
        mGrid->ClearTileStates();
        SetSelectedUnit(nullptr);
        SetBattleState(BattleState::Exploration);
        SDL_Log("Selecao cancelada.");
    }

    // UNDO MOVEMENT
    else if (mBattleState == BattleState::SkillSelection)
    {
        Robot* selected = GetSelectedUnit();
        if (selected) {
            int lastX = mLastTileActorPositionX;
            int lastY = mLastTileActorPositionY;

            mLastTileActorPositionX = mGrid->GetTilePosition(playerUnit->GetPosition().x, playerUnit->GetPosition().y).x;
            mLastTileActorPositionY = mGrid->GetTilePosition(playerUnit->GetPosition().x, playerUnit->GetPosition().y).y;

            MoveInGrid(mSelectedUnit, lastX, lastY);
            MoveInGrid(mCursor, lastX, lastY);
            mGrid->SetSelectedTile(lastX, lastY);

            mTileCursorPositionX = lastX;
            mTileCursorPositionY = lastY;
        }

        mSelectedUnit = selected;
        // SetSelectedUnit(nullptr);
        SetBattleState(BattleState::Exploration);

        SDL_Log("Acao cancelada (Robo voltou).");
    }
    // UNDO SKILL
    else if (mBattleState == BattleState::TargetSelection)
    {
        mGrid->ClearTileStates();

        // mover o cursos para a posiçao atual do jogador
        Vector3 pos = mGrid->GetTilePosition(playerUnit->GetPosition().x, playerUnit->GetPosition().y);
        MoveInGrid(mCursor, pos.x, pos.y);
        mGrid->SetSelectedTile(pos.x, pos.y);

        SetBattleState(BattleState::SkillSelection);
        SDL_Log("Mira cancelada. Escolha outra habilidade (1 ou 2).");
    }
}

// TODO: BUG: quando dou esc duas vezez provavalmente a antiga posição se perdeu

// TODO: faltando lidar com obstaculos (chegar se nao tem um actor ou algum obstaculo)
void Level::MoveInGrid(Actor *actor, int x, int y)
{
    actor->SetPosition(mGrid->GetWorldPosition(x, y) + OFFSET_Z);
    mGrid->SetUnitAt(actor, x, y);
}

