//
// Created by Heitor S. on 11/23/2025.
//

#include "Robot.h"
#include "Game.h"
#include "Random.h"
#include "Json.h"
#include "Destructible.h"
#include <fstream>

Robot::Robot(class Game *game, Team team) : Actor(game)
                                            , mName("Robo")
                                            , mTeam(team)
                                            , mMoveRange(2)
                                            , mIsDead(false)
                                            , mIsMoving(false)
                                            , mHasDualLegs(false)
                                            , mStatusEffect(StatusEffect::None)
{

    for (int i = 0; i < (int)PartSlot::Count; i++)
    {
        mPartMeshes[i] = new MeshComponent(this);

        // Define onde essa peça vai ficar quando for equipada
        Vector3 offset = GetPartMountPosition((PartSlot)i);
        mPartMeshes[i]->SetPositionOffset(offset);

        mPartMeshes[i]->SetMetallic(1.0f); // Robo tem que ser metalico
    }
    mRightLegAuxMesh = new MeshComponent(this);
    Vector3 rightLegOffset = Vector3(0.0f, 0.6f, 2.5f);

    mRightLegAuxMesh->SetPositionOffset(rightLegOffset);
    mRightLegAuxMesh->SetMetallic(1.0f);

    SetScale(Vector3(60.0f, 60.0f, 60.0f));
    SetRotation(Vector3(0.0f, 0.0f, Math::ToRadians(270.0f)));

    //Anim rand
    mAnimOffset = Random::GetFloatRange(0.0f, 100.0f);
    
    // Luz pontual no robo
    mLight = new PointLightComponent(this);
    mLight->SetColor(Vector3(1.0f, 1.0f, 1.0f));
    mLight->SetIntensity(1.5f);
    mLight->SetRadius(400.0f);
    // Spotlight no robo
    mLight->SetOffset(Vector3(0.0f, 0.0f, 250.0f));
}

void Robot::UpdateGridCoords(int x, int y) {
    mGridX = x;
    mGridY = y;
}


void Robot::TakeDamage(int damage, PartSlot slotHit) {
    int index = (int)slotHit;

    // Se a parte alvo já está quebrada, sorteia uma parte que não esteja quebrada
    if (mParts[index].isBroken) {
        std::vector<PartSlot> validParts;
        for (int i = 0; i < (int)PartSlot::Count; i++)
        {
            if (!mParts[i].isBroken) {
                validParts.push_back((PartSlot)i);
            }
        }

        if (!validParts.empty())
        {
            // Sorteia uma parte válida não quebrada
            int randomIndex = Random::GetIntRange(0, validParts.size() - 1);
            index = (int)validParts[randomIndex];
            SDL_Log("Parte original quebrada, redirecionando dano para: %s", mParts[index].name.c_str());
        }
        else
        {
            // Se todas estão quebradas, robô já deveria estar morto
            SDL_Log("AVISO: Todas as partes já estão quebradas!");
            CheckDeath();
            return;
        }
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
    Actor* target = grid->GetUnitAt(targetX, targetY);
    
    if (!target) {
        SDL_Log("Miss");
        return;
    }
    
    // Verifica se é um Robot
    Robot* victim = dynamic_cast<Robot*>(target);
    if (victim)
    {
        // Sorteia uma parte aleatória para receber o dano  TODO: Futuramente conseguir decidir onde o irá mirar
        int randomPart = Random::GetIntRange(0, (int)PartSlot::Count - 1);
        PartSlot targetSlot = (PartSlot)randomPart;
        
        SDL_Log("%s ATACOU %s visando %s!", GetName().c_str(), victim->GetName().c_str(), GetSlotName(targetSlot).c_str());
        victim->TakeDamage(damageDealt, targetSlot);
        return;
    }
    
    // Verifica se é um Destructible
    Destructible* destructible = dynamic_cast<Destructible*>(target);
    if (destructible)
    {
        SDL_Log("%s DESTRUIU um objeto!", GetName().c_str());
        destructible->OnDestroy();
        destructible->SetState(ActorState::Destroy);
        grid->SetUnitAt(nullptr, targetX, targetY);
        grid->SetTerrainType(targetX, targetY, TerrainType::Floor);
        return;
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

bool Robot::EquipPartFromJson(const std::string& partJsonPath)
{
    std::ifstream file(partJsonPath);
    if (!file.is_open()) {
        SDL_Log("ERRO: Não foi possível abrir arquivo de parte: %s", partJsonPath.c_str());
        return false;
    }

    nlohmann::json j;
    try {
        file >> j;
    } catch (const std::exception& e) {
        SDL_Log("ERRO: Falha ao parsear JSON da parte: %s", e.what());
        return false;
    }

    // Helper para converter string de skill para enum
    auto stringToSkillType = [](const std::string& str) -> SkillType {
        if (str == "Punch") return SkillType::Punch;
        if (str == "Missile") return SkillType::Missile;
        if (str == "Dash") return SkillType::Dash;
        if (str == "Shield") return SkillType::Shield;
        if (str == "Repair") return SkillType::Repair;
        return SkillType::None;
    };

    // Helper para converter string de slot para enum
    auto stringToPartSlot = [](const std::string& str) -> PartSlot {
        if (str == "Torso") return PartSlot::Torso;
        if (str == "Head") return PartSlot::Head;
        if (str == "RightArm") return PartSlot::RightArm;
        if (str == "LeftArm") return PartSlot::LeftArm;
        if (str == "Legs") return PartSlot::Legs;
        return PartSlot::Null;
    };

    PartSlot slot = stringToPartSlot(j["slot"].get<std::string>());
    if (slot == PartSlot::Null) {
        SDL_Log("ERRO: Slot inválido em %s", partJsonPath.c_str());
        return false;
    }

    RobotPart part(
        j["name"].get<std::string>(),
        j["meshPath"].get<std::string>(),
        j["maxHP"].get<int>(),
        stringToSkillType(j["skill"].get<std::string>()),
        j["damage"].get<int>(),
        j["range"].get<int>()
    );

    EquipPart(slot, part);
    SDL_Log("Parte '%s' equipada no slot %s", part.name.c_str(), j["slot"].get<std::string>().c_str());
    return true;
}

bool Robot::LoadFromJson(const std::string& jsonPath)
{
    std::ifstream file(jsonPath);
    if (!file.is_open()) {
        SDL_Log("ERRO: Não foi possível abrir o arquivo de robô: %s", jsonPath.c_str());
        return false;
    }

    nlohmann::json j;
    try {
        file >> j;
    } catch (const std::exception& e) {
        SDL_Log("ERRO: Falha ao parsear JSON do robô: %s", e.what());
        return false;
    }

    // Nome do robô
    if (j.contains("name")) {
        SetName(j["name"].get<std::string>());
    }

    // Helper para converter string de skill para enum
    auto stringToSkillType = [](const std::string& str) -> SkillType {
        if (str == "Punch") return SkillType::Punch;
        if (str == "Missile") return SkillType::Missile;
        if (str == "Dash") return SkillType::Dash;
        if (str == "Shield") return SkillType::Shield;
        if (str == "Repair") return SkillType::Repair;
        return SkillType::None;
    };

    // Helper para converter string de slot para enum
    auto stringToPartSlot = [](const std::string& str) -> PartSlot {
        if (str == "Torso") return PartSlot::Torso;
        if (str == "Head") return PartSlot::Head;
        if (str == "RightArm") return PartSlot::RightArm;
        if (str == "LeftArm") return PartSlot::LeftArm;
        if (str == "Legs") return PartSlot::Legs;
        return PartSlot::Null;
    };

    // Carregar partes
    if (j.contains("parts")) {
        for (auto& [slotName, partData] : j["parts"].items()) {
            PartSlot slot = stringToPartSlot(slotName);
            if (slot == PartSlot::Null) {
                SDL_Log("AVISO: Slot desconhecido '%s' ignorado", slotName.c_str());
                continue;
            }

            // Se partData é uma string, é um caminho para outro JSON
            if (partData.is_string()) {
                std::string partPath = partData.get<std::string>();
                if (!EquipPartFromJson(partPath)) {
                    SDL_Log("ERRO: Falha ao carregar parte de %s", partPath.c_str());
                }
            }
            // Se é um objeto, carrega inline (compatibilidade com formato antigo)
            else if (partData.is_object()) {
                RobotPart part(
                    partData["name"].get<std::string>(),
                    partData["meshPath"].get<std::string>(),
                    partData["maxHP"].get<int>(),
                    stringToSkillType(partData["skill"].get<std::string>()),
                    partData["damage"].get<int>(),
                    partData["range"].get<int>()
                );
                EquipPart(slot, part);
            }
        }
    }

    SDL_Log("Robô '%s' carregado com sucesso de %s", GetName().c_str(), jsonPath.c_str());
    return true;
}

bool Robot::SaveToJson(const std::string& jsonPath)
{
    // Helper para converter SkillType para string
    auto skillTypeToString = [](SkillType skill) -> std::string {
        switch (skill) {
            case SkillType::Punch: return "Punch";
            case SkillType::Missile: return "Missile";
            case SkillType::Dash: return "Dash";
            case SkillType::Shield: return "Shield";
            case SkillType::Repair: return "Repair";
            default: return "None";
        }
    };

    // Helper para converter PartSlot para string
    auto partSlotToString = [](PartSlot slot) -> std::string {
        switch (slot) {
            case PartSlot::Torso: return "Torso";
            case PartSlot::Head: return "Head";
            case PartSlot::RightArm: return "RightArm";
            case PartSlot::LeftArm: return "LeftArm";
            case PartSlot::Legs: return "Legs";
            default: return "Null";
        }
    };

    nlohmann::json j;
    j["name"] = mName;

    // Salvar todas as partes equipadas
    for (int i = 1; i < (int)PartSlot::Count; i++) {
        PartSlot slot = (PartSlot)i;
        const RobotPart& part = mParts[i];
        
        // Pular partes vazias/quebradas se quiser
        if (part.name == "Empty") continue;

        std::string slotName = partSlotToString(slot);
        j["parts"][slotName] = {
            {"name", part.name},
            {"meshPath", part.meshPath},
            {"maxHP", part.maxHP},
            {"skill", skillTypeToString(part.skill)},
            {"damage", part.damage},
            {"range", part.range}
        };
    }

    // Escrever arquivo
    std::ofstream file(jsonPath);
    if (!file.is_open()) {
        SDL_Log("ERRO: Não foi possível salvar robô em: %s", jsonPath.c_str());
        return false;
    }

    file << j.dump(2);
    SDL_Log("Robô '%s' salvo com sucesso em %s", mName.c_str(), jsonPath.c_str());
    return true;
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
        Vector3 currentScale = GetScale();
        SetScale(currentScale * 1.02f);
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

void Robot::SyncAnimationState(const Robot* other) {
    if (!other) return;
    this->mAnimOffset = other->mAnimOffset;
}

// Status Effects
void Robot::ApplyStatusEffect(StatusEffect effect)
{
    mStatusEffect = effect;
    
    std::string effectName;
    switch (effect) {
        case StatusEffect::Stunned:
            effectName = "STUNNED (Preso no mel!)";
            break;
        case StatusEffect::Burning:
            effectName = "BURNING (Pegando fogo!)";
            break;
        default:
            effectName = "None";
            break;
    }
    
    SDL_Log("%s recebeu efeito: %s", mName.c_str(), effectName.c_str());
}

void Robot::RemoveStatusEffect(StatusEffect effect)
{
    if (mStatusEffect == effect) {
        SDL_Log("%s removeu o efeito de status!", mName.c_str());
        mStatusEffect = StatusEffect::None;
    }
}

bool Robot::HasStatusEffect(StatusEffect effect) const
{
    return mStatusEffect == effect;
}
