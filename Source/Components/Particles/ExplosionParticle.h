#pragma once

#include "Particle.h"

class ExplosionParticle : public Particle
{
public:
    ExplosionParticle(class Game* game);
    ~ExplosionParticle();

protected:
    void UpdateVisuals(float deltaTime) override;
};
