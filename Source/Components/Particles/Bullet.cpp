//
// Created by Lucas N. Ferreira on 04/11/25.
//

#include "Bullet.h"
#include "../../Actors/Actor.h"
#include "../../Game.h"
#include "../Drawing/MeshComponent.h"
#include "../Physics/AABBColliderComponent.h"
#include "../Physics/RigidBodyComponent.h"


Bullet::Bullet(class Game* game)
        :Particle(game)
{
    // mesh
    Mesh *mesh = game->GetRenderer()->GetMesh("../Assets/Laser.gpmesh");

    if (!mesh)
        SDL_Log("Failed to load Laser");

    mMeshComponent = new class MeshComponent(this);
    mMeshComponent->SetMesh(mesh);

    SetScale(Vector3(5.0f, 5.0f, 5.0f));

    // collider
    mCollisionComponent = new AABBColliderComponent(this, 10.0f, 10.0f, 10.0f, Vector3::Zero, ColliderLayer::Bullet, false);

    // rigid body
    mRigidBody = new RigidBodyComponent(this, 1.0f, 0.0f);
    mRigidBody->SetVelocity(Vector3::Zero);

    // invisible until awake
    mMeshComponent->SetVisible(false);
    mCollisionComponent->SetEnabled(false);
}

Bullet::~Bullet()
{
}

void Bullet::Kill()
{
    Particle::Kill();

    mMeshComponent->SetVisible(false);
    mCollisionComponent->SetEnabled(false);
    mRigidBody->SetVelocity(Vector3::Zero);
}

void Bullet::Awake(const Vector3 &position, const Vector3 &rotation, float lifetime)
{
    Particle::Awake(position, rotation, lifetime);

    mCollisionComponent->SetEnabled(true);
    mMeshComponent->SetVisible(true);
}

void Bullet::Emit(const Vector3 &direction, float speed)
{
    mRigidBody->SetVelocity(direction * speed);
}

void Bullet::OnUpdate(float deltaTime)
{
    Particle::OnUpdate(deltaTime);

    if (IsDead()) return;
}