//
// Created by Heitor S. on 11/23/2025.
//

#pragma once
#include <string>

#include "Actor.h"

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
        int GetGridX() const { return mGridX; }
        int GetGridY() const { return mGridY; }

        int GetCurrentTotalHP() const;
        int GetMaxTotalHP() const;

        int GetMovementRange() const { return mMoveRange; }
        void SetMovementRange(int range) { mMoveRange = range; }

        void SetGridPosition(int x, int y);

        Team GetTeam() const {return mTeam; }
        int GetPartRange(PartSlot chosenSlot) {return mParts[chosenSlot].range; }

        // Move
        void MoveTo(int newX, int newY);
        void UndoMove();

        // Attack
        void TakeDamage(int damage, PartSlot slotHit);
        bool CanUseSkill(PartSlot slot) const;
        void AttackLocation(int targetX, int targetY, PartSlot slotUsed);

        // Death
        void CheckDeath();
        void Kill();


    private:
        std::string mName;
        int mGridX, mGridY;
        int mPrevGridX, mPrevGridY;

        int mMoveRange;

        Team mTeam;
        RobotPart mParts[(int)PartSlot::Count];
        class MeshComponent* mPartMeshes[(int)PartSlot::Count] = { nullptr };

};

