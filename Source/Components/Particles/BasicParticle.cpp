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
    , mViscosity(0.0f)
{
    Mesh* mesh = game->GetRenderer()->GetMesh("../Assets/Particle.gpmesh");
    if (!mesh) {
        SDL_Log("Warning: Particle.gpmesh not found, using Cube.gpmesh");
        mesh = game->GetRenderer()->GetMesh("../Assets/Cube.gpmesh");
    }

    mMeshComponent = new ParticleMeshComponent(this);
    mMeshComponent->SetMesh(mesh);

    mRigidBody = new RigidBodyComponent(this, 0.1f, 0.0f, false);

    SetScale(Vector3(10.0f, 10.0f, 10.0f));
    mMeshComponent->SetVisible(false);
}

BasicParticle::~BasicParticle()
{
}

void BasicParticle::Kill()
{
    Particle::Kill();
    mMeshComponent->SetVisible(false);
    mRigidBody->SetVelocity(Vector3::Zero);
    mRigidBody->SetAcceleration(Vector3::Zero);

    // Reset completo da partícula
    mStretchWithVelocity = false;
    SetScale(Vector3(10.0f, 10.0f, 10.0f)); // Escala padrão
    mViscosity = 0.0f;
    mGravity = 0.0f;
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
    mRigidBody->SetAcceleration(Vector3::Zero);
}

void BasicParticle::Emit(const Vector3& direction, float speed)
{
    mRigidBody->SetVelocity(direction * speed);

    if (mGravity != 0.0f) {
        Vector3 gravity(0.0f, 0.0f, mGravity);
        mRigidBody->ApplyForce(gravity);
    }
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

    // Estica partícula baseado na velocidade (efeito de gota caindo)
    if (mStretchWithVelocity && mRigidBody) {
        Vector3 velocity = mRigidBody->GetVelocity();
        float speed = velocity.Length();

        if (speed > 10.0f) {
            // Quanto mais rápido, mais esticado
            float stretchFactor = Math::Clamp(speed / 100.0f, 1.0f, 3.0f);

            Vector3 stretchedScale(
                mBaseScale.x,
                mBaseScale.y,
                mBaseScale.z * stretchFactor
            );

            SetScale(stretchedScale);
        } else {
            SetScale(mBaseScale);
        }
    }

    // Aplica viscosidade (amortecimento da velocidade)
    if (mViscosity > 0.0f && mRigidBody) {
        Vector3 currentVel = mRigidBody->GetVelocity();
        Vector3 dampedVel = currentVel * (1.0f - mViscosity * deltaTime);
        mRigidBody->SetVelocity(dampedVel);
    }

    if (mFadeOut && mMeshComponent) {
        float currentLife = GetLifeTime();
        float initialLife = mInitialLifetime;
        
        if (initialLife > 0.0f) {
            // Calcula alpha baseado no lifetime restante (0.0 = morto, 1.0 = novo)
            float alpha = Math::Clamp(currentLife / initialLife, 0.0f, 1.0f);
            
            // Fade mais agressivo nos últimos 30% da vida
            if (alpha < 0.3f) {
                alpha = alpha / 0.3f;  // Remapeia 0-0.3 para 0-1
            }
            
            mMeshComponent->SetAlpha(alpha);
        }
    }
}

void BasicParticle::SetStretchWithVelocity(bool stretch)
{
    mStretchWithVelocity = stretch;
}