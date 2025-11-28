//
// Created by mateus on 25/11/2025.
//

/*
 * Level
 * Instancia todos os atores e mostra uma cena
 * básica de grids.
 */

#pragma once

#include "Camera.h"
#include "Game.h"
#include "Actors/Robot.h"
#include "Actors/GridCursor.h"
#include "Map/GridMap.h"
#include "UI/Screens/HUD.h"

struct TurnAction {
    // Move
    int moveX, moveY;

    // Skill
    bool hasAction;
    PartSlot skillSlot;
    int targetX, targetY;

    TurnAction() : moveX(-1), moveY(-1), hasAction(false), targetX(-1), targetY(-1) {}
};

enum class BattleState {
    Null,           // Estado inválido
    Exploration,    // Navegando livremente
    MoveSelection,  // Robô selecionado, escolhendo destino
    SkillSelection, // Robô moveu, escolhendo habilidade
    TargetSelection // Selecionando a grid alvo da habilidade
};

class Level
{
    public:
        Level(Game* game, HUD *hud);
        virtual ~Level();

        void ProcessInput(SDL_Event &event);
        virtual void OnUpdate(float deltaTime);

        void MoveInGrid(Actor *actor, int x, int y);

        GridMap* GetGrid() const { return mGrid; }
        GridCursor* GetCursor() { return mCursor; }
        BattleState GetBattleState() const { return mBattleState; }
        PartSlot GetSelectedSlot() const { return mSelectedSlot; }


        void SetSelectedSlot(PartSlot slot) { mSelectedSlot = slot; }
        void SetBattleState(BattleState state) { mBattleState = state; }

    protected:
        Game* mGame;
        Camera* mCamera;

        HUD* mHud;
        GridCursor* mCursor;
        GridMap* mGrid;

        // battle
        BattleState mBattleState;
        PartSlot mSelectedSlot;

        Robot *mPlayer;
        Robot *mEnemy;

        void SpawnFloor(int rows, int cols);
        void MoveCursor(int tile_x, int tile_y);

    private:
        static const int ROWS = 4;
        static const int COLS = 4;
        const float SIZE = 500.0f;
        const float OFFSET_Z = -100;

        // Pre Action
        Robot* mGhostPlayer;

        // Action
        TurnAction mPlayerTurn;
        TurnAction mEnemyTurn;    // IA define isso dps

        // Lógica do gameplay
        void HandleAction();
        void HandleCancel();
        void HandleWait();

        // Handle Phase
        void HandleExplorationPhase();
        void HandleMovementPhase();
        void HandleSkillSelectionPhase(PartSlot slot);
        void HandleTargetingPhase();
        void HandleUnitDeath(Robot*& robot);

        void SpawnGhost();
        void RemoveGhost();

        // IA simples para testar o fluxo do jogo
        void CalculateEnemyAction();
        void ResolveTurn();
};
