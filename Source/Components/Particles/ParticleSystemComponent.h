//
// Created by Lucas N. Ferreira on 18/09/25.
//

#pragma once

#include "../Component.h"
#include "Particle.h"
#include <vector>
#include <functional>

template <typename T>
class ParticleSystemComponent : public Component {

public:
    ParticleSystemComponent(class Actor* owner, int poolSize = 100, int updateOrder = 10);
    ~ParticleSystemComponent();

    T* EmitParticle(float lifetime, float speed, const Vector3& offsetPosition);

private:

    std::vector<T*> mParticles;
};

#include "ParticleSystemComponent.tpp"   // <-- required