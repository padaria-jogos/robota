//
// Created by andre on 08/12/2025.
//

#pragma once
#include "Particle.h"

class BasicParticle : public Particle
{
public:
    BasicParticle(class Game* game);
    ~BasicParticle();

    void OnUpdate(float deltaTime) override;
    void Kill() override;
    void Emit(const Vector3& direction, float speed) override;
    void Awake(const Vector3& position, const Vector3& rotation, float lifetime) override;

    void SetColor(const Vector3& color);
    void SetGravity(float gravity) { mGravity = gravity; }
    void SetFadeOut(bool fade) { mFadeOut = fade; }
    void SetViscosity(float viscosity) { mViscosity = viscosity; }
    void SetStretchWithVelocity(bool stretch);
    void SetBaseScale(const Vector3 scale) { mBaseScale = scale; }

private:
    class ParticleMeshComponent* mMeshComponent;
    class RigidBodyComponent* mRigidBody;
    
    Vector3 mColor;
    float mGravity;
    bool mFadeOut;
    float mInitialLifetime;
    float mViscosity;
    bool mStretchWithVelocity = false;

    Vector3 mBaseScale = Vector3(10.0f, 10.0f, 10.0f);
};