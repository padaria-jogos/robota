//
// Created by Lucas N. Ferreira on 18/09/25.
//

#include "../../Game.h"
#include "ParticleSystemComponent.h"

template <typename T>
ParticleSystemComponent<T>::ParticleSystemComponent(class Actor* owner, int poolSize, int updateOrder)
    : Component(owner, updateOrder)
{
    // Create a pool of particles
    for (int i = 0; i < poolSize; i++)
    {
        auto* p = new T(owner->GetGame());
        mParticles.push_back(p);
    }
}

template <typename T>
ParticleSystemComponent<T>::~ParticleSystemComponent()
{
    mParticles.clear();
}

template <typename T>
void ParticleSystemComponent<T>::EmitParticle(float lifetime, float speed, const Vector3& offsetPosition)
{
    for (auto p : mParticles)
    {
        if (p->IsDead())
        {
            // Wake up the particle
            Vector3 spawnPos = mOwner->GetPosition() + offsetPosition;
            p->Awake(spawnPos, mOwner->GetRotation(), lifetime);

            // Forward velocity
            Vector3 direction = mOwner->GetForward();
            p->Emit(direction, speed);

            // Break inner loop to emit only one particle per iteration
            break;
        }
    }
}

template <typename T>
void ParticleSystemComponent<T>::EmitParticleAt(float lifetime, const Vector3& worldPosition)
{
    for (auto p : mParticles)
    {
        if (p->IsDead())
        {
            // Spawn na posição world absoluta (SEM offset do owner)
            p->Awake(worldPosition, Vector3::Zero, lifetime);

            // Sem velocidade (partícula estática)
            p->Emit(Vector3::Zero, 0.0f);

            break;
        }
    }
}

template <typename T>
void ParticleSystemComponent<T>::EmitProjectile(const Vector3& position, const Vector3& direction, float speed, float lifetime)
{
    for (auto p : mParticles)
    {
        if (p->IsDead())
        {
            // 1. Set Start Position (Absolute)
            p->Awake(position, Vector3::Zero, lifetime);

            // 2. Set Velocity (Direction * Speed)
            p->Emit(direction, speed);

            break;
        }
    }
}