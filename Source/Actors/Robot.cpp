//
// Created by Heitor S. on 11/23/2025.
//

#include "Robot.h"
// #include "Game.h"

#include "Game.h"
#include "Random.h"

Robot::Robot(class Game *game, Team team) : Actor(game)
                                            , mName("Robo")
                                            , mTeam(team)
                                            , mMoveRange(2)
                                            , mIsDead(false)
                                            , mIsMoving(false)
                                            , mHasDualLegs(false)
{

    for (int i = 0; i < (int)PartSlot::Count; i++)
    {
        mPartMeshes[i] = new MeshComponent(this);

        // Define onde essa peça vai ficar quando for equipada
        Vector3 offset = GetPartMountPosition((PartSlot)i);
        mPartMeshes[i]->SetPositionOffset(offset);
    }
    mRightLegAuxMesh = new MeshComponent(this);
    Vector3 rightLegOffset = Vector3(0.0f, 0.6f, 2.5f);

    mRightLegAuxMesh->SetPositionOffset(rightLegOffset);

    SetScale(Vector3(60.0f, 60.0f, 60.0f));
    SetRotation(Vector3(0.0f, 0.0f, Math::ToRadians(270.0f)));

    //Anim rand
    mAnimOffset = Random::GetFloatRange(0.0f, 100.0f);
}

void Robot::UpdateGridCoords(int x, int y) {
    mGridX = x;
    mGridY = y;
}


void Robot::TakeDamage(int damage, PartSlot slotHit) {
    int index = (int)slotHit;

    if (mParts[index].isBroken) {
        // Dano no torso se a parte já tiver quebrada
        index = (int)PartSlot::Torso;
    }

    mParts[index].currentHP -= damage;

    if (mParts[index].currentHP <= 0) {
        mParts[index].currentHP = 0;
        mParts[index].isBroken = true;

        SDL_Log("PARTE DESTRUIDA: %s", mParts[index].name.c_str());
        // Apaga a mesh ou mudo para textura queimada
        // // mPartMeshes[index]->SetVisible(false);
    }

    CheckDeath();
}

bool Robot::CanUseSkill(PartSlot slot) const
{
    int index = (int)slot;
    if (mParts[index].isBroken) {
        SDL_Log("Falha: A parte %s está quebrada!", mParts[index].name.c_str());
        return false;
    }

    if (mParts[index].skill == SkillType::None) return false;

    return true;
}

void Robot::AttackLocation(int targetX, int targetY, PartSlot slotUsed)
{
    if (!CanUseSkill(slotUsed)) return;
    int damageDealt = mParts[(int)slotUsed].damage;
    std::string partName = mParts[(int)slotUsed].name;

    SDL_Log("ATACANDO TILE (%d, %d) com %s!", targetX, targetY, partName.c_str());

    // Verificar o que tem na grid
    GridMap* grid = mGame->GetLevel()->GetGrid();
    Robot* victim = dynamic_cast<Robot*>(grid->GetUnitAt(targetX, targetY));

    if (victim)
    {
        std::vector<PartSlot> validParts;
        for (int i = 0; i < (int)PartSlot::Count; i++)
        {
            // Acessamos partes que ainda nao quebraram
            if (!victim->mParts[i].isBroken) {
                validParts.push_back((PartSlot)i);
            }
        }

        if (!validParts.empty())
        {
            // Sorteia uma parte válida não quebrada
            int randomIndex = Random::GetIntRange(0, validParts.size() - 1);
            PartSlot targetSlot = validParts[randomIndex];
            SDL_Log("%s ACERTOU %s no %s!", GetName().c_str(), victim->GetName().c_str(), GetSlotName(targetSlot).c_str());
            victim->TakeDamage(damageDealt, targetSlot);
        }else
        {
            // Se chegou aqui, o robô já deveria estar morto
            SDL_Log("Já deveria estare morto");
            victim->TakeDamage(damageDealt, PartSlot::Torso);
        }
    }else
    {
        SDL_Log("Miss");
    }
}

void Robot::EquipPart(PartSlot slot, const RobotPart& part)
{
    int index = (int)slot;
    mParts[index] = part;

    // Mesh
    if (!part.meshPath.empty())
    {
        auto* renderer = mGame->GetRenderer();
        auto* mesh = renderer->GetMesh(part.meshPath);

        if (mPartMeshes[index]) {
            mPartMeshes[index]->SetMesh(mesh);
            // if (mTeam == Team::Enemy) {
            //     Texture* enemyText = mGame->GetRenderer()->GetTexture("../Assets/Robots/BeaBee/BeaBeeTextureEvil.png");
            //     mPartMeshes[index]->SetTextureOverride(enemyText);
            // }
        }

        if (slot == PartSlot::Legs)
        {
            std::string meshPath = part.meshPath;
            std::string search = "LeftLeg";
            size_t found = meshPath.find(search);

            if (found != std::string::npos)
            {
                // Robo bipede
                mHasDualLegs = true;

                // Esquerda
                Vector3 defaultOffset = GetPartMountPosition(PartSlot::Legs);
                mPartMeshes[index]->SetPositionOffset(defaultOffset);

                // Direita
                if (mRightLegAuxMesh) {
                    std::string rightPath = meshPath;
                    rightPath.replace(found, search.length(), "RightLeg");

                    auto* meshRight = mGame->GetRenderer()->GetMesh(rightPath);
                    mRightLegAuxMesh->SetMesh(meshRight);
                    mRightLegAuxMesh->SetVisible(true);

                    // Inverte o Y para a direita
                    Vector3 rightOffset = defaultOffset;
                    rightOffset.y *= -1.0f;
                    mRightLegAuxMesh->SetPositionOffset(rightOffset);
                }
            }
            else
            {
                // Robo tanque/roda
                mHasDualLegs = false;

                Vector3 centerOffset = GetPartMountPosition(PartSlot::Legs);
                centerOffset.y = 0.0f;
                mPartMeshes[index]->SetPositionOffset(centerOffset);

                if (mRightLegAuxMesh) {
                    mRightLegAuxMesh->SetVisible(false);
                }
            }
        }


    }
}

void Robot::CheckDeath()
{
    bool isAlive = false;

    for (int i = 0; i < (int)PartSlot::Count; i++)
    {
        if (!mParts[i].isBroken) {
            isAlive = true;
            break;
        }
    }

    if (!isAlive)
    {
        SDL_Log("%s MORREU! (Todas as partes quebradas)", mName.c_str());
        mIsDead = true;
    }
}

void Robot::CopyDataFrom(const Robot *other) {
    for (int i = 0; i < (int)PartSlot::Count; i++)
    {
        PartSlot slot = (PartSlot)i;

        // Pega a peça do original
        const RobotPart& originalPart = other->GetPart(slot);

        // Equipa neste robô
        this->EquipPart(slot, originalPart);
    }
}

void Robot::SetGhostMode(bool enable)
{
    // Percorre todos os MeshComponents e troca a textura
    Texture* ghostTex = nullptr;

    if (enable) {
        ghostTex = GetGame()->GetRenderer()->GetTexture("../Assets/Hologram.png");
    }

    for (int i = 0; i < (int)PartSlot::Count; i++) {
        if (mPartMeshes[i]) {
            mPartMeshes[i]->SetTextureOverride(ghostTex);
        }
    }

    if (mRightLegAuxMesh) {
        mRightLegAuxMesh->SetTextureOverride(ghostTex);
    }
}

void Robot::SetVisible(bool visible)
{
    // Esconde todas as partes equipadas
    for (int i = 0; i < (int)PartSlot::Count; i++)
    {
        if (mPartMeshes[i]) {
            mPartMeshes[i]->SetVisible(visible);
        }
    }

    if (mRightLegAuxMesh) {
        mRightLegAuxMesh->SetVisible(visible);
    }
}

std::string Robot::GetSlotName(PartSlot slot) {
    switch(slot) {
        case PartSlot::Head: return "Head";
        case PartSlot::Torso: return "Torso";
        case PartSlot::RightArm: return "RightArm";
        case PartSlot::LeftArm: return "LeftArm";
        case PartSlot::Legs: return "Legs";
        default: return "Desconhecido";
    }
}

Vector3 Robot::GetPartMountPosition(PartSlot slot)
{

    switch (slot) {
        case PartSlot::Torso:    return Vector3(0.0f, 0.0f, 3.5f);
        case PartSlot::Head:     return Vector3(0.0f, 0.0f, 4.5f);
        case PartSlot::Legs:     return Vector3(-0.0f, -0.6f, 2.5f);
        case PartSlot::RightArm: return Vector3(0.0f, 1.2f, 4.5f);
        case PartSlot::LeftArm:  return Vector3(0.0f, -1.2f, 4.5f);
        default:                 return Vector3::Zero;
    }
}

void Robot::OnUpdate(float deltaTime) {

    if (mIsMoving) {
        mMoveTimer += deltaTime;
        float t = Math::Clamp(mMoveTimer / mMoveDuration, 0.0f, 1.0f);
        Vector3 currentPos = Vector3::Lerp(mStartPos, mTargetPos, t);
        SetPosition(currentPos);

        if (t >= 1.0f) {
            mIsMoving = false;
        }
    }

    // Animação inicial
    float time = (SDL_GetTicks() / 1000.0f) + mAnimOffset;

    // Idle
    float baseArmAngle = Math::ToRadians(65.0f);
    float breathSway = Math::Sin(time * 2.0f) * Math::ToRadians(3.0f);


    if (mPartMeshes[(int)PartSlot::RightArm])
    {
        Quaternion rot = Quaternion(Vector3::UnitX, -(baseArmAngle + breathSway));
        mPartMeshes[(int)PartSlot::RightArm]->SetRotationOffset(rot);
    }

    if (mPartMeshes[(int)PartSlot::LeftArm])
    {
        Quaternion rot = Quaternion(Vector3::UnitX, (baseArmAngle + breathSway));
        mPartMeshes[(int)PartSlot::LeftArm]->SetRotationOffset(rot);
    }

    if (mPartMeshes[(int)PartSlot::Head])
    {
        float headAngle = Math::Sin(time * 2.0f) * Math::ToRadians(40.0f);
        Quaternion headRot = Quaternion(Vector3::UnitZ, headAngle);
        mPartMeshes[(int)PartSlot::Head]->SetRotationOffset(headRot);
    }

    // Movimentacao Walk
    if (mIsMoving && mHasDualLegs)
    {
        float walkSpeed = 10.0f;
        float walkTime = SDL_GetTicks() / 1000.0f;

        // Amplitude do passo
        float walkAngle = Math::Sin(walkTime * walkSpeed) * Math::ToRadians(40.0f);

        // Perna Esquerda
        if (mPartMeshes[(int)PartSlot::Legs]) {
            mPartMeshes[(int)PartSlot::Legs]->SetRotationOffset(Quaternion(Vector3::UnitY, walkAngle));
        }

        // Perna Direita
        if (mRightLegAuxMesh) {
            mRightLegAuxMesh->SetRotationOffset(Quaternion(Vector3::UnitY, -walkAngle));
        }
    }
    else if (!mIsMoving && mHasDualLegs)
    {
        // Se parou, reseta a rotação para 0
        if (mPartMeshes[(int)PartSlot::Legs])
            mPartMeshes[(int)PartSlot::Legs]->SetRotationOffset(Quaternion::Identity);

        if (mRightLegAuxMesh)
            mRightLegAuxMesh->SetRotationOffset(Quaternion::Identity);
    }
}

void Robot::StartSmoothMovement(const Vector3& targetWorldPos, float duration) {
    mStartPos = GetPosition();
    mTargetPos = targetWorldPos;
    mTargetPos.z = mStartPos.z;
    mMoveDuration = duration;
    mMoveTimer = 0.0f;
    mIsMoving = true;

    // Direction
    Vector3 dir = mTargetPos - mStartPos;
    dir.z = 0.0f;

    if (dir.LengthSq() > 0.001f)
    {
        dir.Normalize();

        float angle = Math::Atan2(dir.y, dir.x);
        SetRotation(Vector3(0.0f, 0.0f, angle));
    }
}




