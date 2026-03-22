//
// Created by andre on 08/12/2025.
//

#include "Game.h"
#include "ParticleManager.h"

#include "Random.h"

ParticleManager::ParticleManager(Game* game, int poolSize)
    : Actor(game)
{
    mBillboardPool = new ParticleSystemComponent<BillboardParticle>(this, poolSize);
    mSmokeTextures.push_back(game->GetRenderer()->GetTexture("../Assets/Textures/smoke1.png"));
    mSmokeTextures.push_back(game->GetRenderer()->GetTexture("../Assets/Textures/smoke2.png"));

    mSphereMesh = mGame->GetRenderer()->GetMesh("../Assets/Cube.gpmesh");
    mMeshPool = new ParticleSystemComponent<BasicParticle>(this, poolSize);
}


void ParticleManager::PlayEffect(const ParticleConfig& config, const Vector3& pos, const Vector3& direction)
{
    Renderer* renderer = GetGame()->GetRenderer();
    for (int i = 0; i < config.Count; i++)
    {
        if (config.Is3D)
        {
            //Pede e pego uma particula da pool
            auto* p = mMeshPool->EmitParticle(config.LifeTime, 0.0f, pos);
            if (!p) continue;

            //Escolho uma mesh
            Mesh* mesh = renderer->GetMesh(config.MeshPath);
            p->SetMesh(mesh);

            //Caracterizo ela com as config
            p->SetColor(config.Color);
            p->SetScale(Vector3(config.Scale));
            p->SetGrowthRate(config.GrowthRate);
            p->SetGravity(config.Gravity);

            // Rotacao
            if (config.RandomSpin)
            {
                p->SetAngularVelocity(Random::GetVector(Vector3(-5), Vector3(5)));
            }else
            {
                p->SetAngularVelocity(Vector3::Zero);
            }

            // Velocidade e direcao
            if (config.BaseSpeed > 0.0f)
            {
                Vector3 dir;
                if (config.Count > 1)
                {
                    //TODO: mudar pra ser aleatorio mas no range da direcao
                    dir = Random::GetVector(Vector3(-1.0f, -1.0f, 0.5f), Vector3(1, 1, 2));
                }else
                {
                    dir = direction;
                }
                dir.Normalize();
                p->SetVelocity(dir * config.BaseSpeed);
            }else
            {
                p->SetVelocity(Vector3::Zero);
            }

        }
        //2D
        else
        {
            //TODO Billboard config etc
            auto* p = mBillboardPool->EmitParticle(config.LifeTime, 0.0f, pos);
            if (!p) continue;
            return;
        }
    }
}


