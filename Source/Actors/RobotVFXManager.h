#pragma once

#include "../Components/Particles/ParticleSystemComponent.h"
#include "../Components/Particles/Bullet.h"
#include "../Components/Particles/ExplosionParticle.h"
#include "../Components/Particles/ImpactParticle.h"

class Robot; // Forward declaration

class RobotVFXManager
{
public:
    RobotVFXManager(Robot* robot);
    ~RobotVFXManager();

    // Trigger visual effects
    void PlayAttackEffect(const Vector3& targetPosition);
    void PlayHitEffect(const Vector3& hitPosition);
    void PlayMissEffect(const Vector3& missPosition);
    
    // For testing - spawn particle at cursor
    void SpawnTestParticle(const Vector3& position);

private:
    Robot* mOwnerRobot;
    
    // Particle systems
    ParticleSystemComponent<Bullet>* mBulletSystem;
    ParticleSystemComponent<ExplosionParticle>* mExplosionSystem;
    ParticleSystemComponent<ImpactParticle>* mImpactSystem;
};