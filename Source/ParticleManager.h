    //
// Created by andre on 08/12/2025.
//

#pragma once
#include "Actors/Actor.h"
#include "Components/Particles/ParticleSystemComponent.h"
#include "Components/Particles/BasicParticle.h"
#include "Components/Particles/BillboardParticle.h"

class Game;
class GridMap;
class Mesh;
class Texture;

struct ParticleConfig
{
    std::string MeshPath;
    bool Is3D = true;

    int Count = 1;

    //Aparencia
    Vector3 Color = Vector3::One;
    float LifeTime = 1.0f;

    //Tamanho
    float Scale = 1.0f;
    float GrowthRate = 0.0f;

    //Física
    float Gravity = 0.0f;          // 980.0f
    float BaseSpeed = 0.0f;

    //Rotacao
    bool RandomSpin = false;
    Vector3 AngularVelocity = Vector3::Zero;
};

class ParticleManager : public Actor
{
    public:
        ParticleManager(Game* game, int poolSize = 100);
        void PlayEffect(const ParticleConfig& config, const Vector3& pos, const Vector3& direction = Vector3::UnitX);
    private:
        ParticleSystemComponent<BillboardParticle>* mBillboardPool;
        ParticleSystemComponent<BasicParticle>* mMeshPool;

        std::vector<Texture*> mSmokeTextures;
        Mesh* mSphereMesh;

        BasicParticle* GetFreeExplosionParticle();
};
