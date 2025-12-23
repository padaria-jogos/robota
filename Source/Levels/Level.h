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
#include "Actors/Skybox.h"
#include "Map/GridMap.h"
#include "UI/Screens/HUD.h"
#include "UI/Screens/ActionSelection.h"
#include "IA/IA.h"
#include "ParticleManager.h"
#include "Actors/SkillSystem.h"

// IDs dos tiles no CSV
enum TileID {
    TILE_EMPTY = -1,
    TILE_VOID = 0,
    TILE_FLOOR = 1,
    TILE_WALL = 2,
    TILE_PLAYER_SPAWN = 5,
    TILE_HONEY = 6,
    TILE_FIRE = 7,
    TILE_DESTRUCTIBLE = 8,
    TILE_ENEMY_SPAWN = 11
};

struct LevelConfig {
    std::string csvPath;
    std::string floorTexture;
    std::string wallTexture;
    std::string honeyTexture;
    std::string fireTexture;
    std::vector<std::string> destructibleMeshes;  // Lista de meshes destrutíveis
    std::string musicPath;
    
    LevelConfig() : 
        csvPath(""),
        floorTexture(""),
        wallTexture(""),
        honeyTexture(""),
        musicPath("") {}
};

struct TurnAction {
    // Move
    int moveX, moveY;
    std::vector<Vector2> path;

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
    TargetSelection, // Selecionando a grid alvo da habilidade
    GameOver,        // Fim de jogo
    GaveUp           // Jogador escolhendo desistir
};

class Level
{
    public:
        Level(Game* game, HUD *hud);
        virtual ~Level();

        void ProcessInput(const SDL_Event &event);
        virtual void OnUpdate(float deltaTime);
        
        // Carrega configuração da arena a partir de JSON
        static bool LoadLevelConfig(const std::string& jsonPath, LevelConfig& config);
        // Carrega o level usando a config
        void LoadLevel(const LevelConfig& config);

        void LoadLevel(const std::string path);
        void MoveInGrid(Actor *actor, int x, int y) const;

        GridMap* GetGrid() const { return mGrid; }
        GridCursor* GetCursor() { return mCursor; }
        BattleState GetBattleState() const { return mBattleState; }
        PartSlot GetSelectedSlot() const { return mSelectedSlot; }

        void SetSelectedSlot(PartSlot slot) { mSelectedSlot = slot; }
        void SetBattleState(BattleState state) { mBattleState = state; }
        
        // Controle de iluminação do mundo (0.0 = escuro, 1.0 = normal, >1.0 = super iluminado)
        void SetWorldLightIntensity(float intensity);

        // Chamados pelos UIs
        void HandleAction();
        void HandleCancel();
        void HandleWait();

        SoundHandle mLevelMusic;

        ParticleManager* GetParticleManager() { return mParticleManager; }
        std::string GetEnemyName() {return mEnemy->GetName();}

        void HandleGaveUp(int action);

    protected:
        Game* mGame;
        Camera* mCamera;

        // skybox
        Skybox* mSkybox;

        HUD* mHud;
        GridCursor* mCursor;
        GridMap* mGrid;
        UIScreen* mActionSelection;
        UIScreen* mTileSelection;
        UIScreen* mMovementSelection;
        UIScreen* mGaveUpSelection;

        // battle
        BattleState mBkpBattleState;
        BattleState mBattleState;
        PartSlot mSelectedSlot;

        Robot *mPlayer;
        Robot *mEnemy;

        void MoveCursor(int tile_x, int tile_y);

        void ConfigureCutscene();

        void InitializeIA();

        void NotifyPlayer(const std::string& message) const;
        void NotifyEnemy(const std::string& message) const;
        void NotifyBoth(const std::string& message) const;

    private:
        const Vector3 TILE_SCALE = Vector3(500.0f, 500.0f, 500.0f);
        const float SIZE = 500.0f;
        const float OFFSET_Z = -100;
        
        // Level config (texturas, música, etc)
        LevelConfig mLevelConfig;

        // Pre Action
        Robot* mGhostPlayer;

        // Action
        TurnAction mPlayerTurn;
        TurnAction mEnemyTurn;    // IA define isso dps
        bool mIsResolving;
        int mStepIndex;

        // IA
        IA *mIA;
        std::vector<std::vector<int>> mIAGridData;
        void UpdateIARobotStats(RobotStats *playerStats, RobotStats *enemyStats);
        void UpdateIAGridData();

        // Handle Phase
        void HandleExplorationPhase();
        void HandleMovementPhase();
        void HandleSkillSelectionPhase(PartSlot slot);
        void HandleTargetingPhase();
        void HandleUnitDeath(Robot*& robot);

        void SpawnGhost();
        void RemoveGhost() const;

        void StartResolution();
        void ExecuteNextStep();
        void FinishResolution();
        
        // Processa efeitos dos tiles onde os robôs estão
        void ProcessTileEffects();

        // IA simples para testar o fluxo do jogo
        void CalculateEnemyAction();
        void ResolveTurn();

        // Novo sistema de habilidades
        SkillData GetSkillDataForPart(PartSlot slot, Robot* robot = nullptr);
        void ResolveSkillAttack(Robot* attacker, const SkillData& skill, 
                               int targetX, int targetY, PartSlot slot);
        void UpdateAoEPreview();

        // Preview de tiles afetados
        void UpdateSkillPreview(int cursorX, int cursorY, const SkillData& skill);
        void ClearSkillPreview();
        
        std::vector<AffectedTile> mCurrentPreviewTiles;

        ParticleManager* mParticleManager;
};
