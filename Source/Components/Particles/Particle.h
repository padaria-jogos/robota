#pragma once

#include "../../Actors/Actor.h"

enum class ParticleType
{
    Mesh,       // Partícula com mesh 3D (bullets, projéteis)
    Billboard,  // Partícula billboard (explosões, impactos)
    Trail       // Rastro
};

class Particle : public Actor
{
public:
    Particle(class Game* game, ParticleType type = ParticleType::Mesh);
    virtual ~Particle();

    void OnUpdate(float deltaTime) override;

    virtual void Awake(const Vector3 &position, const Vector3 &rotation, float lifetime = 1.0f);
    virtual void Emit(const Vector3 &direction, float speed);
    virtual void Kill();

    bool IsDead() const { return mIsDead; }
    
    // Configurações visuais
    void SetColor(const Vector3& color);
    void SetAlpha(float alpha);
    void SetFadeOut(bool fade) { mFadeOut = fade; }
    void SetGrow(bool grow, float growSpeed = 1.0f);
    
    ParticleType GetParticleType() const { return mType; }

protected:
    // Update visual properties (fade, scale, etc)
    virtual void UpdateVisuals(float deltaTime);
    
    // Components
    class MeshComponent* mMeshComponent;
    class RigidBodyComponent* mRigidBody;
    
    // Properties
    ParticleType mType;
    float mLifeTime;
    float mMaxLifeTime;
    bool mIsDead;
    
    // Visual effects
    bool mFadeOut;
    bool mGrow;
    float mGrowSpeed;
    float mInitialScale;
};