//
// Created by heito on 12/23/25.
//

#include "BillboardParticle.h"
#include  "Game.h"
#include  "Components/Drawing/MeshComponent.h"
#include "Camera.h"
#include "Components/Drawing/ParticleMeshComponent.h"

BillboardParticle::BillboardParticle(Game* game)
    : Particle(game), mScale(100.0f)
{
    // Usa um Quad simples (Plane.gpmesh ou Quad.gpmesh)
    mMeshComp = new MeshComponent(this, true);
    mMeshComp->SetMesh(game->GetRenderer()->GetMesh("../Assets/Tile.gpmesh"));
    mMeshComp->SetUnlit(true); // Brilha no escuro (estilo anime)
    mMeshComp->SetVisible(false);
}

void BillboardParticle::Awake(const Vector3& position, const Vector3& rotation, float lifetime)
{
    Particle::Awake(position, rotation, lifetime);
    mMeshComp->SetVisible(true);
    SetScale(Vector3(mScale, mScale, mScale));
    
    SDL_Log("BillboardParticle::Awake - Posição: (%.1f, %.1f, %.1f), Lifetime: %.2f", 
            position.x, position.y, position.z, lifetime);
}

void BillboardParticle::OnUpdate(float deltaTime)
{
    Particle::OnUpdate(deltaTime);
    if (IsDead()) {
        mMeshComp->SetVisible(false);
        return;
    }

    // --- LÓGICA DE BILLBOARD (Olhar para a câmera) ---
    Vector3 camPos = GetGame()->GetCamera()->GetPosition();
    Vector3 myPos = GetPosition();
    Vector3 toCam = camPos - myPos;

    float angle = Math::Atan2(toCam.y, toCam.x);
    SDL_Log("BillboardParticle::OnUpdate - Câmera: (%.1f, %.1f, %.1f), Minha Pos: (%.1f, %.1f, %.1f), Direção: (%.1f, %.1f, %.1f), Ângulo: %.2f rad (%.2f°)",
            camPos.x, camPos.y, camPos.z, myPos.x, myPos.y, myPos.z, toCam.x, toCam.y, toCam.z, angle, Math::ToDegrees(angle));

    SetRotation(Vector3(Math::ToRadians(90.0f), 0.0f, angle));
    SetScale(Vector3(mScale));
}

void BillboardParticle::SetAnimation(const std::vector<class Texture*>& textures, float fps)
{
    mAnimTextures = textures;
    mAnimFPS = fps;
    mAnimTimer = 0.0f;
    mCurrentFrameIndex = 0;

    if (!mAnimTextures.empty()) {
        mMeshComp->SetTextureOverride(mAnimTextures[0]);
    }

}

void BillboardParticle::SetTexture(const std::string& textureName)
{
    auto* tex = GetGame()->GetRenderer()->GetTexture(textureName);
    if (tex) mMeshComp->SetTextureOverride(tex);
}