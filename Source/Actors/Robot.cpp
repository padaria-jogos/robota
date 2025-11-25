//
// Created by Heitor S. on 11/23/2025.
//

#include "Robot.h"
// #include "Game.h"

#include "Game.h"
// #include "Random.h"
// #include "Components/Drawing/MeshComponent.h"

// Robot::Robot(class Game *game, Team team) : Actor(game)
//     , mName("Robo")
//     , mTeam(team)
//     , mMoveRange(2)
// {
//
// }

// void Robot::SetGridPosition(int x, int y)
// {
//
// }
//
// void Robot::MoveTo(int newX, int newY)
// {
//
// }
//
// void Robot::UndoMove()
// {
//
// }
//
// void Robot::TakeDamage(int damage, PartSlot slotHit)
// {
//
// }
//
// bool Robot::CanUseSkill(PartSlot slot) const
// {
//
// }
//
// void Robot::AttackLocation(int targetX, int targetY, PartSlot slotUsed)
// {
//
// }
//
// void Robot::EquipPart(PartSlot slot, const RobotPart& part)
// {
//
// }
//
// void Robot::CheckDeath()
// {
//
// }
//
// void Robot::Kill()
// {
//
// }

// #include "Map/GridMap.h"

Robot::Robot(class Game *game, Team team) : Actor(game)
    , mName("Robo")
    , mTeam(team)
    , mMoveRange(2)
{
    MeshComponent* mc = new MeshComponent(this);
    Mesh* mesh = mGame->GetRenderer()->GetMesh("../Assets/Roboto.gpmesh");
    mc->SetMesh(mesh);

    for (int i = 0; i < (int)PartSlot::Count; i++)
    {
        mPartMeshes[i] = new MeshComponent(this);
    }

    SetScale(Vector3(200.0f, 200.0f, 200.0f));
    SetRotation(Vector3(0.0f, 0.0f, Math::ToRadians(180.0f)));

    if (mTeam == Team::Enemy)
    {
        Texture* selectTex = mGame->GetRenderer()->GetTexture("../Assets/RobotoEvil.png");
        mc->SetTextureOverride(selectTex);
    }
}

void Robot::SetGridPosition(int x, int y)
{
    mGridX = x;
    mGridY = y;

    // GridMap* grid = mGame->GetLevel()->GetGrid();
    //
    // if (grid)
    // {
    //     grid->SetUnitAt(mGridX, mGridY, nullptr); //limpa antigo
    //
    //     mGridX = x;
    //     mGridY = y;
    //
    //     grid->SetUnitAt(mGridX, mGridY, this); //seta o novo
    //
    //     // VISUAL:
    //     Vector3 worldPos = grid->GetWorldPosition(mGridX, mGridY);
    //     worldPos.z = -100.0f;
    //
    //     SetPosition(worldPos);
    // }
}

void Robot::MoveTo(int newX, int newY) {
    // GridMap* grid = mGame->GetLevel()->GetGrid();
    //
    // if (!grid) return;
    //
    // grid->SetUnitAt(mGridX, mGridY, nullptr);
    //
    // // Armazena de onde veio caso queira voltar
    // mPrevGridX = mGridX;
    // mPrevGridY = mGridY;
    //
    // mGridX = newX;
    // mGridY = newY;
    // grid->SetUnitAt(mGridX, mGridY, this);
    //
    // Vector3 worldPos = grid->GetWorldPosition(mGridX, mGridY);
    // worldPos.z = -100.0f;
    // SetPosition(worldPos);
    // SDL_Log("Robo moveu para %d, %d", mGridX, mGridY);
}

void Robot::UndoMove()
{
    // MoveTo(mPrevGridX, mPrevGridY);
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
        SDL_Log("ACERTOU %s!", victim->GetName().c_str());

        // Sempre acerta o Torso se não mirar especificamente
        victim->TakeDamage(damageDealt, PartSlot::Torso);
    }
    else
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
        Kill();
    }
}

// TODO: lembrar de destruir no vector da grid
void Robot::Kill() {
    // GridMap* grid = mGame->GetLevel()->GetGrid();
    // if (grid) {
    //     grid->SetUnitAt(mGridX, mGridY);
    // }
    //
    SetState(ActorState::Destroy);
}


