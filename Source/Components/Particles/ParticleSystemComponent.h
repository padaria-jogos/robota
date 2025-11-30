#ifndef PARTICLESYSTEMCOMPONENT_TPP
#define PARTICLESYSTEMCOMPONENT_TPP

#include "../Component.h"
#include "Particle.h"
#include <vector>
#include <functional>

template <typename T>
class ParticleSystemComponent : public Component {

public:
    ParticleSystemComponent(class Actor* owner, int poolSize = 100, int updateOrder = 10);
    ~ParticleSystemComponent();

    void EmitParticle(float lifetime, float speed, const Vector3& offsetPosition = Vector3::Zero);
    void EmitProjectile(const Vector3& position, const Vector3& direction, float speed, float lifetime);
    // Emite partícula em posição absoluta (world position)
    void EmitParticleAt(float lifetime, const Vector3& worldPosition);

private:
    std::vector<T*> mParticles;
};

#include "ParticleSystemComponent.tpp"   // <-- required

#endif