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

    void SetMesh(Mesh* mesh);
    void SetColor(const Vector3& color);
    void SetGravity(float gravity) { mGravity = gravity; }
    void SetVelocity(const Vector3& vel) { mVelocity = vel; }
    void SetAngularVelocity(const Vector3& angVel) { mAngularVelocity = angVel; }
    void SetFadeOut(bool fade) { mFadeOut = fade; }
    void SetGrowthRate(float rate) { mGrowthRate = rate; }
    void SetViscosity(float viscosity) { mViscosity = viscosity; }
    void SetStretchWithVelocity(bool stretch);
    void SetBaseScale(const Vector3 scale) { mBaseScale = scale; }

private:
    class ParticleMeshComponent* mMeshComponent;
    
    Vector3 mColor;
    Vector3 mVelocity;
    Vector3 mAngularVelocity;
    float mGravity;
    bool mFadeOut;
    float mInitialLifetime;
    float mGrowthRate;
    float mViscosity;
    bool mStretchWithVelocity = false;

    Vector3 mBaseScale = Vector3(10.0f, 10.0f, 10.0f);
};