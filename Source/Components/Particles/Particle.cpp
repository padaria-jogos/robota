#include "Particle.h"
#include "../Drawing/MeshComponent.h"
#include "../Physics/RigidBodyComponent.h"
#include "../../Game.h"
#include "../../Renderer/Shader.h"

Particle::Particle(class Game* game, ParticleType type)
    : Actor(game)
    , mType(type)
    , mIsDead(true)
    , mLifeTime(1.0f)
    , mMaxLifeTime(1.0f)
    , mFadeOut(true)
    , mGrow(false)
    , mGrowSpeed(1.0f)
    , mInitialScale(1.0f)
    , mMeshComponent(nullptr)
    , mRigidBody(nullptr)
{
    SetState(ActorState::Paused);
}

Particle::~Particle()
{
}

void Particle::Kill()
{
    mIsDead = true;
    SetState(ActorState::Paused);
    
    if (mMeshComponent)
    {
        mMeshComponent->SetVisible(false);
    }
        
    if (mRigidBody)
    {
        mRigidBody->SetVelocity(Vector3::Zero);
    }
}

void Particle::Awake(const Vector3 &position, const Vector3 &rotation, float lifetime)
{
    mLifeTime = lifetime;
    mMaxLifeTime = lifetime;
    mIsDead = false;
    mInitialScale = GetScale().x;

    SetState(ActorState::Active);
    SetPosition(position);
    SetRotation(rotation);
    
    if (mMeshComponent)
    {
        mMeshComponent->SetVisible(true);
        mMeshComponent->SetAlpha(1.0f);
    }
}

void Particle::Emit(const Vector3 &direction, float speed)
{
    if (mRigidBody)
    {
        mRigidBody->SetVelocity(direction * speed);
    }
}

void Particle::OnUpdate(float deltaTime)
{
    if (mIsDead) return;
    
    // Update lifetime
    mLifeTime -= deltaTime;
    if (mLifeTime <= 0)
    {
        Kill();
        return;
    }
    
    // Update visual effects
    UpdateVisuals(deltaTime);
}

void Particle::UpdateVisuals(float deltaTime)
{
    if (!mMeshComponent) return;
    
    float normalizedLife = mLifeTime / mMaxLifeTime; // 1.0 = just spawned, 0.0 = about to die
    
    // Fade out effect
    if (mFadeOut)
    {
        mMeshComponent->SetAlpha(normalizedLife);
    }
    
    // Grow effect
    if (mGrow)
    {
        float growFactor = 1.0f + (1.0f - normalizedLife) * mGrowSpeed;
        float newScale = mInitialScale * growFactor;
        SetScale(Vector3(newScale, newScale, newScale));
    }
}

void Particle::SetColor(const Vector3& color)
{
    if (mMeshComponent)
    {
        mMeshComponent->SetColor(color);
    }
}

void Particle::SetAlpha(float alpha)
{
    if (mMeshComponent)
    {
        mMeshComponent->SetAlpha(alpha);
    }
}

void Particle::SetGrow(bool grow, float growSpeed)
{
    mGrow = grow;
    mGrowSpeed = growSpeed;
}