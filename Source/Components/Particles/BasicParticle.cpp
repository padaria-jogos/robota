//
// Created by andre on 08/12/2025.
//

#include "BasicParticle.h"
#include "../../Game.h"
#include "../Drawing/ParticleMeshComponent.h"
#include "../Physics/RigidBodyComponent.h"
#include "../../Renderer/Mesh.h"

BasicParticle::BasicParticle(Game* game)
    : Particle(game)
    , mColor(Vector3(1.0f, 1.0f, 1.0f))
    , mGravity(0.0f)
    , mFadeOut(true)
    , mInitialLifetime(1.0f)
    , mGrowthRate(0.0f)
    , mViscosity(0.0f)
    , mStretchWithVelocity(false)
{
    mMeshComponent = new ParticleMeshComponent(this, true);
    mMeshComponent->SetVisible(false);
}

BasicParticle::~BasicParticle()
{
}

void BasicParticle::SetMesh(Mesh* mesh)
{
    if (mMeshComponent)
    {
        mMeshComponent->SetMesh(mesh);
        mMeshComponent->SetVisible(true);
        return;
    }
    SDL_Log("Mesh is null in the particle");
}


void BasicParticle::Kill()
{
    Particle::Kill();
    mMeshComponent->SetVisible(false);

    // Reset completo da partícula
    mVelocity = Vector3::Zero;
    mAngularVelocity = Vector3::Zero;
    mGravity = 0.0f;
    mViscosity = 0.0f;
    mGrowthRate = 0.0f;
    mStretchWithVelocity = false;

    SetScale(Vector3(10.0f, 10.0f, 10.0f)); // Escala padrão
    SetRotation(Vector3::Zero);
}

void BasicParticle::Awake(const Vector3& position, const Vector3& rotation, float lifetime)
{
    Particle::Awake(position, rotation, lifetime);

    mInitialLifetime = lifetime;
    mMeshComponent->SetVisible(true);

    // Garantir reset ao despertar
    mStretchWithVelocity = false;
    mViscosity = 0.0f;
    mGravity = 0.0f;
}

void BasicParticle::Emit(const Vector3& direction, float speed)
{
    SetVelocity(direction * speed);
}

void BasicParticle::SetColor(const Vector3& color)
{
    mColor = color;
    if (mMeshComponent) {
        mMeshComponent->SetParticleColor(color);
    }
}

void BasicParticle::OnUpdate(float deltaTime)
{
    Particle::OnUpdate(deltaTime);

    if (IsDead()) {
        mMeshComponent->SetVisible(false);
        return;
    }

    if (Math::Abs(mGravity) > 0.001f)
    {
        mVelocity.z -= mGravity * deltaTime;
    }

    if (mAngularVelocity.LengthSq() > 0.001f)
    {
        Vector3 currentRot = GetRotation();
        Vector3 nextRot = currentRot + (mAngularVelocity * deltaTime);

        SetRotation(nextRot);
    }

    // Viscosity effect (For Honey)
    if (mViscosity > 0.0f)
    {
        mVelocity *= (1.0f - mViscosity * deltaTime);
    }

    SetPosition(GetPosition() + mVelocity * deltaTime);

    // Growth Effect (For explosions etc)
    if (Math::Abs(mGrowthRate) > 0.01f)
    {
        Vector3 currentScale = GetScale();
        float growthAmount = mGrowthRate * deltaTime;
        Vector3 newScale = currentScale + Vector3(growthAmount);

        if (newScale.x < 0.0f) newScale = Vector3::Zero;
        SetScale(newScale);
    }

    // StretchEffect
    if (mStretchWithVelocity) {
        float speed = mVelocity.Length();
        if (speed > 10.0f) {
            float stretchFactor = Math::Clamp(speed / 100.0f, 1.0f, 3.0f);

            Vector3 baseScale = GetScale();
            SetScale(Vector3(baseScale.x, baseScale.y, baseScale.z * stretchFactor));
        }
    }

    //FadeOut
    if (mFadeOut && mMeshComponent) {
        float currentLife = GetLifeTime(); // Quanto tempo falta
        float totalLife = mInitialLifetime;

        if (totalLife > 0.0f) {
            // Começa opaco (1.0) e vai sumindo
            float alpha = Math::Clamp(currentLife / totalLife, 0.0f, 1.0f);
            mMeshComponent->SetAlpha(alpha);
        }
    }

}

void BasicParticle::SetStretchWithVelocity(bool stretch)
{
    mStretchWithVelocity = stretch;
}