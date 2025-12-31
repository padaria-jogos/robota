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
T* ParticleSystemComponent<T>::EmitParticle(float lifetime, float speed, const Vector3& offsetPosition)
{
    for (auto p : mParticles)
    {
        if (p->IsDead())
        {
            // Acorda a partícula
            Vector3 spawnPos = mOwner->GetPosition() + offsetPosition;
            p->Awake(spawnPos, mOwner->GetRotation(), lifetime);

            // Define velocidade inicial (se for usada)
            Vector3 direction = mOwner->GetForward();
            p->Emit(direction, speed);

            // Retorna o ponteiro para quem chamou
            return p;
        }
    }

    // Se percorreu toda a piscina e não achou ninguém morto (pool cheia)
    SDL_Log("AVISO: Pool de partículas cheio!");
    return nullptr;
}