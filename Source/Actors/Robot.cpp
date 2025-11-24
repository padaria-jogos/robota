//
// Created by Heitor S. on 11/23/2025.
//

#include "Robot.h"

#include "Game.h"
#include "Random.h"
#include "Components/Drawing/MeshComponent.h"
#include "Map/GridMap.h"

Robot::Robot(class Game *game, Team team)
    : Actor(game)
    , mTeam(team)
    , mMoveRange(2)
{
    MeshComponent* mc = new MeshComponent(this);
    Mesh* mesh = game->GetRenderer()->GetMesh("../Assets/Roboto.gpmesh");
    mc->SetMesh(mesh);

    for (int i = 0; i < (int)PartSlot::Count; i++)
    {
        mPartMeshes[i] = new MeshComponent(this);
    }

    SetScale(Vector3(200.0f, 200.0f, 200.0f));
    SetRotation(Vector3(0.0f, 0.0f, Math::ToRadians(180.0f)));

    if (mTeam == Team::Enemy) {
        Texture* selectTex = GetGame()->GetRenderer()->GetTexture("../Assets/RobotoEvil.png");
        mc->SetTextureOverride(selectTex);
    }


}

void Robot::SetGridPosition(int x, int y)
{
    GridMap* grid = GetGame()->GetGrid();
    if (grid)
    {
        grid->SetUnitAt(mGridX, mGridY, nullptr); //limpa antigo

        mGridX = x;
        mGridY = y;

        grid->SetUnitAt(mGridX, mGridY, this); //seta o novo

        // VISUAL:
        Vector3 worldPos = grid->GetWorldPosition(mGridX, mGridY);
        worldPos.z = -100.0f;

        SetPosition(worldPos);
    }
}

void Robot::MoveTo(int newX, int newY) {
    GridMap* grid = GetGame()->GetGrid();
    if (!grid) return;
    grid->SetUnitAt(mGridX, mGridY, nullptr);
    mGridX = newX;
    mGridY = newY;
    grid->SetUnitAt(mGridX, mGridY, this);

    Vector3 worldPos = grid->GetWorldPosition(mGridX, mGridY);
    worldPos.z = -100.0f;
    SetPosition(worldPos);
    SDL_Log("Robo moveu para %d, %d", mGridX, mGridY);
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
        // mPartMeshes[index]->SetVisible(false);
    }

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

void Robot::Attack(Robot* target, PartSlot slotUsed)
{
    if (!CanUseSkill(slotUsed)) return;
    int damageDealt = mParts[(int)slotUsed].damage;

    // Onde vai acertar no inimigo (test torso)
    PartSlot targetSlot = PartSlot::Torso;

    // Chance de acertar cabeça sla(TODO: colocar na opção do ataque onde mirar)
    if (Random::GetFloat() > 0.8f) targetSlot = PartSlot::Head;

    SDL_Log("Atacando %s com %s causando %d de dano!",
            target->GetTeam() == Team::Enemy ? "Inimigo" : "Aliado",
            mParts[(int)slotUsed].name.c_str(),
            damageDealt);

    target->TakeDamage(damageDealt, targetSlot);
}

void Robot::EquipPart(PartSlot slot, const RobotPart& part)
{
    int index = (int)slot;
    mParts[index] = part;

    // Mesh
    if (!part.meshPath.empty())
    {
        auto* renderer = GetGame()->GetRenderer();
        auto* mesh = renderer->GetMesh(part.meshPath);

        if (mPartMeshes[index]) {
            mPartMeshes[index]->SetMesh(mesh);
        }
    }
}

