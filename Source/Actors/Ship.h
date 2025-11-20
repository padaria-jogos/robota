//
// Created by Lucas N. Ferreira on 02/11/25.
//

#pragma once

#include "Actor.h"
#include "../AudioSystem.h"
#include "../Components/Particles/Bullet.h"
#include "../Components/Particles/ParticleSystemComponent.h"

class Ship : public Actor {
public:
    Ship(class Game* game);
    ~Ship();

    void OnProcessInput(const Uint8* keyState) override;
    void OnUpdate(float deltaTime) override;

    // Get/Set Health
    int GetHealth() const { return mHealth; }
    void DealDamage(const int damage);

    // Get/Set Speed
    void SetSpeed(const float speed);
    float GetSpeed() const { return mSpeed; }

private:
    int mHealth;

    bool mIsInvincible;

    float mLaserCooldown;
    float mInvinsibleCooldown;
    float mTurnDirection;
    float mTurnAcceleration;
    float mTurnVelocity;
    float mSpeed;

    SoundHandle mShipSound;
    SoundHandle mAlertSound;

    class MeshComponent *mMeshComponent;
    class AABBColliderComponent* mCollisionComponent;
    class RigidBodyComponent *mRigidBody;
    class ParticleSystemComponent<class Bullet> *mLaserWeapon;
};