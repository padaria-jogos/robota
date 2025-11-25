//
// Created by Lucas N. Ferreira on 02/11/25.
//

#include "Ship.h"
#include "../Game.h"
#include "../Components/Drawing/MeshComponent.h"
#include "../Components/Physics/AABBColliderComponent.h"
#include "../Components/Physics/RigidBodyComponent.h"
#include "../UI/Screens/HUD.h"
#include "../UI/Screens/GameOver.h"

Ship::Ship(class Game* game)
        : Actor(game)
        , mMeshComponent(nullptr)
        , mCollisionComponent(nullptr)
        , mRigidBody(nullptr)
        , mLaserCooldown(0.f)
        , mInvinsibleCooldown(0.f)
        , mHealth(3)
        , mIsInvincible(false)
        , mTurnDirection(0.0f)
        ,mTurnAcceleration(0.0f)
        ,mTurnVelocity(0.0f)
        ,mSpeed(400.0f)
{
    // mesh component
    Mesh* shipMesh = game->GetRenderer()->GetMesh("../Assets/Arwing.gpmesh");
    mMeshComponent = new MeshComponent(this);
    mMeshComponent->SetMesh(shipMesh);
    SetScale(Vector3(2.0f, 2.0f, 2.0f));

    // collision component
    mCollisionComponent = new AABBColliderComponent(this, 15.0f, 40.0f, 25.0f, Vector3::Zero, ColliderLayer::Player, false);

    // rigid body component
    mRigidBody = new RigidBodyComponent(this, 1.0f, 2.0f);
    mRigidBody->SetVelocity(Vector3(0.0f, 0.0f, 0.0f));

    // lasers particle system
    mLaserWeapon = new ParticleSystemComponent<Bullet>(this, 50);
}

Ship::~Ship()
{
}

void Ship::DealDamage(const int damage)
{

}

void Ship::SetSpeed(const float speed)
{
    mSpeed = speed;
    mRigidBody->SetVelocity(Vector3(mSpeed, 0.0f, 0.0f));
}

void Ship::OnUpdate(float deltaTime)
{
    // floor bound
    Vector3 pos = GetPosition();
    if (pos.z < -225.0f) pos.z = -225.0f;
    SetPosition(pos);
}

void Ship::OnProcessInput(const Uint8 *keyState)
{
    float force = 1000.0f;

    // base directions
    Vector3 up(0.0f, 0.0f, 1.0f);
    Vector3 down(0.0f, 0.0f, -1.0f);
    Vector3 right(0.0f, 1.0f, 0.0f);
    Vector3 left(0.0f, -1.0f, 0.0f);
    Vector3 forward(1.0f, 0.0f, 0.0f);
    Vector3 backward(-1.0f, 0.0f, 0.0f);

    // move up (+Z)
    if (keyState[SDL_SCANCODE_W])
        mRigidBody->ApplyForce(up * force);

    // move down (-Z)
    if (keyState[SDL_SCANCODE_S])
        mRigidBody->ApplyForce(down * force);

    // move right (+Y)
    if (keyState[SDL_SCANCODE_D])
        mRigidBody->ApplyForce(right * force);

    // move left (-Y)
    if (keyState[SDL_SCANCODE_A])
        mRigidBody->ApplyForce(left * force);

    // move forward (+X)
    if (keyState[SDL_SCANCODE_E])
        mRigidBody->ApplyForce(forward * force);

    // move backward (-X)
    if (keyState[SDL_SCANCODE_Q])
        mRigidBody->ApplyForce(backward * force);

    // shooting
    if (mLaserCooldown > 0.0f)
        mLaserCooldown -= 1.0f / 60.0f;

    // if cooldown is over, shoot when space is pressed
    if (keyState[SDL_SCANCODE_SPACE] && mLaserCooldown <= 0.0f)
    {
        mLaserCooldown = 0.25f;

        float lifetime = 1000.0f;
        float speed = mSpeed * 3.0f;
        Vector3 offset = GetForward() * 40.0f;

        // spawn bullet particle with 1 second lifetime
        mLaserWeapon->EmitParticle(lifetime, speed, offset);
    }
}