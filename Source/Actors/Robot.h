//
// Created by Heitor S. on 11/23/2025.
//

/*
 * Robot
 * Lida com representações básicas dos gráficos
 * dos robôs em batalha.
 */

#pragma once

#include <memory>
#include <string>
#include "Actor.h"
#include "Components/Drawing/MeshComponent.h"
class RobotVFXManager;

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

        void EquipPart(PartSlot slot, const RobotPart& part);

        void SetName(std::string name)  { mName = name; }
        std::string GetName() const { return mName; }

        // Position
        int GetGridX() const { return mGridX; }
        int GetGridY() const { return mGridY; }

        void SaveGridPosition() {
                mSavedX = GetGridX();
                mSavedY = GetGridY();
            }

        int GetSavedX() const { return mSavedX; }
        int GetSavedY() const { return mSavedY; }

        // int GetCurrentTotalHP() const;
        // int GetMaxTotalHP() const;

        int GetMovementRange() const { return mMoveRange; }
        void SetMovementRange(int range) { mMoveRange = range; }

        void UpdateGridCoords(int x, int y);

        Team GetTeam() const {return mTeam; }
        int GetPartRange(PartSlot chosenSlot) const {return mParts[chosenSlot].range; }


        // Attack
        void TakeDamage(int damage, PartSlot slotHit);
        bool CanUseSkill(PartSlot slot) const;
        void AttackLocation(int targetX, int targetY, PartSlot slotUsed);

        // Death
        bool IsDead() const { return mIsDead; }

        RobotVFXManager* GetVFXManager() { return mVFXManager.get(); }


    private:
        std::unique_ptr<RobotVFXManager> mVFXManager;
        std::string mName;
        int mGridX, mGridY;
        int mSavedX, mSavedY;
        int mMoveRange;

        bool mIsDead;
        void CheckDeath();

        Team mTeam;
        RobotPart mParts[(int)PartSlot::Count];
        class MeshComponent* mPartMeshes[(int)PartSlot::Count] = { nullptr };

};

