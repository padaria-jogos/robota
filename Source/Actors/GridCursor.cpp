//
// Created by Heitor S. on 11/22/2025.
//

#include "GridCursor.h"

#include "Game.h"
#include "Robot.h"
#include "Tile.h"
#include "Components/Drawing/MeshComponent.h"


GridCursor::GridCursor(Game* game)
        : Actor(game)
        , mGridX(0)
        , mGridY(0)
        , mAnimTimer(0.0f)
        , mBaseHeight(500.0f)
{
        MeshComponent* mc = new MeshComponent(this);
        Mesh* mesh = game->GetRenderer()->GetMesh("../Assets/Cursor.gpmesh");
        mc->SetMesh(mesh);

        SetScale(Vector3(100.0f, 100.0f, 100.0f));
        Move(0, 0);
        SetRotation(Vector3(Math::ToRadians(180.0f), 0.0f, 0.0f));
}

void GridCursor::Move(int xOffset, int yOffset)
{
        // Calcular nova posição
        int newX = mGridX + xOffset;
        int newY = mGridY + yOffset;

        // Obter o GridMap para checar limites
        GridMap* grid = GetGame()->GetGrid();
        if (!grid) {
                SDL_Log("ERRO: GridCursor tentou mover mas GetGrid() retornou NULL!");
                return;
        }

        if (!grid) return;

        // Verificar se saiu do mapa (Clamp)
        if (newX < 0) newX = 0;
        if (newX >= grid->GetCols()) newX = grid->GetCols() - 1;
        if (newY < 0) newY = 0;
        if (newY >= grid->GetRows()) newY = grid->GetRows() - 1;

        mGridX = newX;
        mGridY = newY;

        // Atualizar Visual
        Vector3 worldPos = grid->GetWorldPosition(mGridX, mGridY);
        worldPos.z = mBaseHeight;
        SetPosition(worldPos);

        // Highlight
        grid->SetSelectedTile(mGridX, mGridY);
}

void GridCursor::OnUpdate(float deltaTime)
{
        // Animação de Flutuar
        mAnimTimer += deltaTime;
        Vector3 pos = GetPosition();
        float bobbing = std::sin(mAnimTimer * 5.0f) * 20.0f;
        pos.z = mBaseHeight + bobbing;
        SetPosition(pos);
}

void GridCursor::OnProcessInput(const uint8_t* state)
{
        // Game.cpp para evitar movimento rápido demais.
}

void GridCursor::OnKeyDown(int key)
{
        BattleState state = GetGame()->GetBattleState();
        switch (key)
        {
                case SDLK_w:
                case SDLK_UP:
                        Move(0, 1);
                        break;

                case SDLK_s:
                case SDLK_DOWN:
                        Move(0, -1);
                        break;

                case SDLK_a:
                case SDLK_LEFT:
                        Move(1, 0);
                        break;

                case SDLK_d:
                case SDLK_RIGHT:
                        Move(-1, 0);
                        break;

                case SDLK_SPACE:
                case SDLK_RETURN:
                        SDL_Log("Selecionou Tile: %d, %d", mGridX, mGridY);
                        HandleAction();
                        break;

                case SDLK_BACKSPACE:
                case SDLK_ESCAPE:
                    HandleCancel();
                    break;

                case SDLK_1:
                case SDLK_KP_1:
                    if (state == BattleState::SkillSelection) {
                        GetGame()->SetSelectedSlot(PartSlot::RightArm);
                        SDL_Log(">> Selecionado: Braço Direito");
                        SDL_Log(">> Enter/Espaço para confirmar");
                    }
                    break;

                case SDLK_2:
                case SDLK_KP_2:
                    if (state == BattleState::SkillSelection) {
                        GetGame()->SetSelectedSlot(PartSlot::LeftArm);
                        SDL_Log(">> Selecionado: Braço Esquerdo");
                        SDL_Log(">> Enter/Espaço para confirmar");
                    }
                    break;
        }
}

void GridCursor::SelectSkill(PartSlot slot)
{
    Game* game = GetGame();
    GridMap* grid = game->GetGrid();
    Robot* unit = game->GetSelectedUnit();

    game->SetSelectedSlot(slot);

    int range = unit->GetPartRange(slot);

    // Range da skill em vermelho
    auto tiles = grid->GetAttackableTiles(unit->GetGridX(), unit->GetGridY(), 1, range); // TODO: o min pode ser 0 para habildades de raparo

    for (const auto& node : tiles) {
        Tile* t = grid->GetTileAt(node.x, node.y);
        if (t) t->SetTileType(TileType::Attack);
    }

    game->SetBattleState(BattleState::TargetSelection);
    SDL_Log("Habilidade escolhida, selecione o alvo.");
}

void GridCursor::HandleAction()
{
    Game* game = GetGame();
    GridMap* grid = game->GetGrid();
    if (!grid) return;

    BattleState state = game->GetBattleState();

    // EXPLORAÇÃO
    if (state == BattleState::Exploration)
    {
        Robot* unit = grid->GetUnitAt(mGridX, mGridY);

        // Se tem um robô E é do meu time
        if (unit && unit->GetTeam() == Team::Player)
        {
            game->SetSelectedUnit(unit);
            game->SetBattleState(BattleState::MoveSelection);

            //BFS azul
            int range = unit->GetMovementRange();
            auto tiles = grid->GetWalkableTiles(mGridX, mGridY, range);

            for (const auto& node : tiles) {
                Tile* t = grid->GetTileAt(node.x, node.y);
                if (t) t->SetTileType(TileType::Path);
            }

            SDL_Log("Robo selecionado! Escolha o destino.");

        }
        else if (unit) {
            SDL_Log("Essa unidade nao e sua.");
        }
    }

    // MOVIMENTO
    else if (state == BattleState::MoveSelection)
    {
        Robot* selected = game->GetSelectedUnit();
        if (!selected) return;

        Tile* targetVisual = grid->GetTileAt(mGridX, mGridY);
        if (targetVisual && targetVisual->GetType() == TileType::Path)
        {
            // Verifica se não tem ninguém no destino (exceto eu mesmo)
            Robot* obstacle = grid->GetUnitAt(mGridX, mGridY);
            if (obstacle && obstacle != selected) {
                SDL_Log("Tile ocupado!");
                return;
            }


            selected->MoveTo(mGridX, mGridY);   // Move
            grid->ClearTileStates(); // Apaga o azul do mapa

            game->SetBattleState(BattleState::SkillSelection);

            //Padrao ultimo utilizado
            game->SetSelectedSlot(GetGame()->GetSelectedSlot());

            SDL_Log("Movimento concluido.");
            SDL_Log("PRESSIONE '1' PARA BRACO DIREITO");
            SDL_Log("PRESSIONE '2' PARA BRACO ESQUERDO");
        }
        else {
            SDL_Log("Movimento invalido, muito distante.");
        }
    }

    // Seleção de habilidade
    else if (state == BattleState::SkillSelection) {
        PartSlot chosenSlot = GetGame()->GetSelectedSlot();
        Robot* unit = game->GetSelectedUnit();
        SelectSkill(chosenSlot);

    }

    else if (state == BattleState::TargetSelection) {
        Tile* targetTile = grid->GetTileAt(mGridX, mGridY);

        if (targetTile && targetTile->GetType() == TileType::Attack) {
            Robot* attacker = game->GetSelectedUnit();
            PartSlot slotToUse = PartSlot::RightArm;

            attacker->AttackLocation(mGridX, mGridY, slotToUse);

            grid->ClearTileStates();
            game->SetBattleState(BattleState::Exploration);
            game->SetSelectedUnit(nullptr);

            SDL_Log("Ataque realizado no tile (%d, %d). Fim do turno.", mGridX, mGridY);
        }else {
            SDL_Log("Alvo invalido! Selecione um quadrado dentro da range.");
        }
    }
}


void GridCursor::HandleCancel()
{
    Game* game = GetGame();
    BattleState state = game->GetBattleState();

    // UNDO SELECTION
    if (state == BattleState::MoveSelection)
    {
        game->GetGrid()->ClearTileStates();
        game->SetSelectedUnit(nullptr);
        game->SetBattleState(BattleState::Exploration);
        SDL_Log("Selecao cancelada.");
    }

    // UNDO MOVEMENT
    else if (state == BattleState::SkillSelection)
    {
        Robot* selected = game->GetSelectedUnit();
        if (selected) {
            selected->UndoMove(); // O robô volta fisicamente para trás
        }

        game->SetSelectedUnit(nullptr);
        game->SetBattleState(BattleState::Exploration);

        SDL_Log("Acao cancelada (Robo voltou).");
    }
    // UNDO SKILL
    else if (state == BattleState::TargetSelection)
    {
        game->GetGrid()->ClearTileStates();
        game->SetBattleState(BattleState::SkillSelection);
        SDL_Log("Mira cancelada. Escolha outra habilidade (1 ou 2).");
    }
}
