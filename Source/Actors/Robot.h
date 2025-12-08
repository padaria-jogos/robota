//
// Created by Heitor S. on 11/23/2025.
//

/*
 * Robot
 * Lida com representações básicas dos gráficos
 * dos robôs em batalha.
 */

#pragma once

#include <string>
#include "Actor.h"
#include "Components/Drawing/MeshComponent.h"
#include "Components/Lighting/PointLightComponent.h"

// TODO: Murilo olha aqui dps os tipos mecanicas etc
enum class SkillType {
    None,
    Punch,
    Missile,
    Dash,
    Shield,
    Repair
};

enum class Team {
    Player,
    Enemy,
    Neutral //Obstáculos etc
};

enum class StatusEffect {
    None,
    Stunned,    // Não pode se mover (mel)
    Burning     // Recebe dano ao final do turno (fogo)
};

enum PartSlot {
    Null,
    Head,
    Torso,
    RightArm,
    LeftArm,
    Legs,
    Count
};

struct RobotPart {
    std::string name;
    std::string meshPath;

    // Status
    int maxHP;
    int currentHP;
    bool isBroken;

    SkillType skill;
    int damage;
    int range;

    RobotPart()
        : name("Empty"), maxHP(0), currentHP(0), isBroken(true), skill(SkillType::None), damage(0), range(0) {}

    RobotPart(std::string name, std::string path, int hp, SkillType sk, int dmg, int rng)
        : name(name), meshPath(path), maxHP(hp), currentHP(hp), isBroken(false), skill(sk), damage(dmg), range(rng) {}

};

class Robot : public Actor{
    public:
        Robot(class Game* game, Team team);

        void OnUpdate(float deltaTime) override;

        // Interpolar entre as grids
        void StartSmoothMovement(const Vector3& targetWorldPos, float duration = 0.3f);

        //Equip
        void EquipPart(PartSlot slot, const RobotPart& part);
        bool EquipPartFromJson(const std::string& partJsonPath);
        
        // Load/Save robot configuration from JSON
        bool LoadFromJson(const std::string& jsonPath);
        bool SaveToJson(const std::string& jsonPath);

        void SetName(std::string name)  { mName = name; }
        std::string GetName() const { return mName; }
        std::string GetSlotName(PartSlot slot);

        //Copy
        void CopyDataFrom(const Robot* other);
        void SyncAnimationState(const Robot* other);

        // Position
        int GetGridX() const { return mGridX; }
        int GetGridY() const { return mGridY; }

        // int GetCurrentTotalHP() const;
        // int GetMaxTotalHP() const;

        int GetMovementRange() const { return mMoveRange; }
        void SetMovementRange(int range) { mMoveRange = range; }

        void SetVisible(bool visible);

        void UpdateGridCoords(int x, int y);

        Team GetTeam() const {return mTeam; }
        const RobotPart& GetPart(PartSlot slot) const { return mParts[(int)slot]; }
        int GetPartRange(PartSlot chosenSlot) const {return mParts[chosenSlot].range; }
        void SetGhostMode(bool enable);


        // Attack
        void TakeDamage(int damage, PartSlot slotHit);
        bool CanUseSkill(PartSlot slot) const;
        void AttackLocation(int targetX, int targetY, PartSlot slotUsed);

        // Death
        bool IsMoving() const { return mIsMoving; }
        bool IsDead() const { return mIsDead; }
        
        // Status Effects
        void ApplyStatusEffect(StatusEffect effect);
        void RemoveStatusEffect(StatusEffect effect);
        bool HasStatusEffect(StatusEffect effect) const;
        StatusEffect GetCurrentStatus() const { return mStatusEffect; }

        void SetLastEffectTile(int x, int y) {
                mLastEffectTileX = x;
                mLastEffectTileY = y;
            }

        int GetLastEffectTileX() const { return mLastEffectTileX; }
        int GetLastEffectTileY() const { return mLastEffectTileY; }

        void ClearLastEffectTile() {
                mLastEffectTileX = -1;
                mLastEffectTileY = -1;
            }

    private:
        std::string mName;
        bool mHasDualLegs;
        int mGridX, mGridY;
        int mMoveRange;

        Vector3 mStartPos;
        Vector3 mTargetPos;
        float mMoveTimer;
        float mMoveDuration;

        bool mIsMoving;
        bool mIsDead;
        
        StatusEffect mStatusEffect;

        Team mTeam;
        RobotPart mParts[(int)PartSlot::Count];

        class MeshComponent* mPartMeshes[(int)PartSlot::Count] = { nullptr };
        MeshComponent* mRightLegAuxMesh = nullptr;
        
        // Point light para iluminação do robô
        PointLightComponent* mLight = nullptr;

        // Animation
        float mAnimOffset;

        void CheckDeath();
        Vector3 GetPartMountPosition(PartSlot slot);

        // Rastreia qual tile tem efeitos ativos
        int mLastEffectTileX = -1;
        int mLastEffectTileY = -1;
};

