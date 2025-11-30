#include "Bullet.h"
#include "../../Actors/Actor.h"
#include "../../Game.h"
#include "../Drawing/MeshComponent.h"
#include "../Physics/AABBColliderComponent.h"
#include "../Physics/RigidBodyComponent.h"

Bullet::Bullet(class Game* game)
    : Particle(game, ParticleType::Mesh)
{
    // Mesh component with particle shader
    Mesh *mesh = game->GetRenderer()->GetMesh("../Assets/Laser.gpmesh");
    
    if (!mesh)
        SDL_Log("Failed to load Laser");

    mMeshComponent = new MeshComponent(this, true); // true = use custom shader
    mMeshComponent->SetMesh(mesh);
    mMeshComponent->SetCustomShader(game->GetRenderer()->GetShader("Particle"));
    mMeshComponent->SetVisible(false);

    // Set bullet color (yellow/orange laser)
    SetColor(Vector3(1.0f, 0.8f, 0.2f));
    SetScale(Vector3(5.0f, 5.0f, 5.0f));
    SetFadeOut(false); // Bullets don't fade

    // Collider
    mCollisionComponent = new AABBColliderComponent(
        this, 10.0f, 10.0f, 10.0f, 
        Vector3::Zero, ColliderLayer::Bullet, false
    );
    mCollisionComponent->SetEnabled(false);

    // Rigid body
    mRigidBody = new RigidBodyComponent(this, 1.0f, 0.0f);
    mRigidBody->SetVelocity(Vector3::Zero);
}

Bullet::~Bullet()
{
}

void Bullet::Kill()
{
    Particle::Kill();
    mCollisionComponent->SetEnabled(false);
}

void Bullet::Awake(const Vector3 &position, const Vector3 &rotation, float lifetime)
{
    Particle::Awake(position, rotation, lifetime);
    mCollisionComponent->SetEnabled(true);
}

void Bullet::OnUpdate(float deltaTime)
{
    Particle::OnUpdate(deltaTime);

    if (IsDead()) return;
    
    // Add bullet-specific logic here (collision detection, etc)
}