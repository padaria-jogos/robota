//
// Created by andre on 08/12/2025.
//

#include "ParticleManager.h"

ParticleManager::ParticleManager(Game* game, int poolSize)
    : Actor(game)
{
    mSmokePool = new ParticleSystemComponent<BillboardParticle>(this, poolSize);
    mSmokeTextures.push_back(game->GetRenderer()->GetTexture("../Assets/Textures/smoke1.png"));
    mSmokeTextures.push_back(game->GetRenderer()->GetTexture("../Assets/Textures/smoke2.png"));

    mSphereMesh = mGame->GetRenderer()->GetMesh("../Assets/Cube.gpmesh");
    mExplosionPool = new ParticleSystemComponent<BasicParticle>(this, 50);
}

void ParticleManager::CreateSmokeAtGrid(int x, int y, GridMap* grid) const
{
    Vector3 worldPos = grid->GetWorldPosition(x, y);
    worldPos.z += 700.0f;
    auto* s = mSmokePool->EmitParticle(10000.0f, 0.0f, worldPos);
    if(s) {
        s->SetAnimation(mSmokeTextures, 1.0f);
        s->SetStartScale(100.0f);
    }
}

void ParticleManager::CreateExplosionSphereAtGrid(int x, int y, GridMap* grid)
{
    Vector3 worldPos = grid->GetWorldPosition(x, y);
    worldPos.z += 700.0f;
    BasicParticle* p = mExplosionPool->EmitParticle(1.5f, 0.0f, worldPos);
    if(p)
    {
        p->SetScale(Vector3(100.0f));
        p->SetMesh(mSphereMesh);
        p->SetColor(Vector3(1.0f, 0.5f, 0.0f));
        p->SetVelocity(Vector3::Zero);
        p->SetGravity(0.0f);

        p->SetGrowthRate(500.0f);
        p->SetAngularVelocity(Vector3(0.0f, 0.0f, 5.0f));
    }
}


