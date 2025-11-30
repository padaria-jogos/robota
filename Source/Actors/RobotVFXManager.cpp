#include "RobotVFXManager.h"
#include "Robot.h"
#include "../Game.h"

RobotVFXManager::RobotVFXManager(Robot* robot)
    : mOwnerRobot(robot)
{
    // Create particle systems with pool sizes
    mBulletSystem = new ParticleSystemComponent<Bullet>(robot, 50);
    mExplosionSystem = new ParticleSystemComponent<ExplosionParticle>(robot, 30);
    mImpactSystem = new ParticleSystemComponent<ImpactParticle>(robot, 40);
}

RobotVFXManager::~RobotVFXManager() = default;

void RobotVFXManager::PlayAttackEffect(const Vector3& targetPosition)
{
    Vector3 robotPos = mOwnerRobot->GetPosition();

    // 1. Calculate Direction
    Vector3 direction = targetPosition - robotPos;
    direction.Normalize();

    // 2. Calculate Spawn Position (Robot Center + Offset in direction of target)
    // Using 60.0f to push it slightly outside the robot mesh
    Vector3 spawnPos = robotPos + (direction * 60.0f);

    // 3. Fire using the new function
    mBulletSystem->EmitProjectile(spawnPos, direction, 300.0f, 2.0f);
}

void RobotVFXManager::PlayHitEffect(const Vector3& hitPosition)
{
    // Ajusta Z para ficar visível (acima do chão)
    Vector3 adjustedPos = hitPosition;
    adjustedPos.z += 100.0f;

    // tiro com posição absoluta
    mExplosionSystem->EmitParticleAt(0.8f, adjustedPos);
    mImpactSystem->EmitParticleAt(0.5f, adjustedPos);
    
    SDL_Log("[VFX] Hit effect at (%.1f, %.1f, %.1f)", 
            hitPosition.x, hitPosition.y, hitPosition.z);
}

void RobotVFXManager::PlayMissEffect(const Vector3& missPosition)
{
    // Just a small impact effect
    mImpactSystem->EmitParticle(
        0.3f,           // short lifetime
        0.0f,
        Vector3::Zero
    );
    
    SDL_Log("[VFX] Miss effect at (%.1f, %.1f, %.1f)", 
            missPosition.x, missPosition.y, missPosition.z);
}

void RobotVFXManager::SpawnTestParticle(const Vector3& position)
{
    // Para teste - projétil usa EmitParticle (relativo)
    Vector3 robotPos = mOwnerRobot->GetPosition();
    Vector3 direction = position - robotPos;
    direction.Normalize();
    
    Vector3 offset = direction * 50.0f;
    
    mBulletSystem->EmitParticle(1.5f, 200.0f, offset);
    
    SDL_Log("[VFX TEST] Particle spawned towards (%.1f, %.1f, %.1f)", 
            position.x, position.y, position.z);
}
