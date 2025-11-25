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
#include "Actors/Robot.h"
#include "Actors/GridCursor.h"
#include "Map/GridMap.h"
#include "UI/Screens/HUD.h"

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
        virtual void OnUpdate(float deltaTime) {};

        // lógica do gameplay
        void HandleAction();
        void HandleCancel();

        void MoveInGrid(Actor *actor, int x, int y);

        GridMap* GetGrid() { return mGrid; }
        GridCursor* GetCursor() { return mCursor; }
        BattleState GetBattleState() const { return mBattleState; }
        Robot* GetSelectedUnit() { return mSelectedUnit; }
        PartSlot GetSelectedSlot() const { return mSelectedSlot; }

        void SelectSkill(PartSlot slot);

        void SetSelectedSlot(PartSlot slot) { mSelectedSlot = slot; }
        void SetSelectedUnit(Robot* robot) { mSelectedUnit = robot; }
        void SetBattleState(BattleState state) { mBattleState = state; }

    protected:
        Game* mGame;
        Camera* mCamera;

        HUD* mHud;
        GridCursor* mCursor;
        GridMap* mGrid;

        int mLastTileActorPositionX;
        int mLastTileActorPositionY;
        int mTileCursorPositionX;
        int mTileCursorPositionY;

        // battle
        BattleState mBattleState;
        Robot* mSelectedUnit;
        PartSlot mSelectedSlot;

        Robot *playerUnit;
        Robot *enemyUnit;

        void SpawnFloor(int rows, int cols);
        void MoveCursor(int tile_x, int tile_y);

    private:
        static const int ROWS = 4;
        static const int COLS = 4;
        const float SIZE = 500.0f;
        const Vector3 OFFSET_Z = Vector3(0.0f, 0.0f, 150.0f);
};
