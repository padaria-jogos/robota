//
// Created by Lucas N. Ferreira on 08/09/23.
//

#include <SDL.h>
#include "../../Actors/Actor.h"
#include "../../Game.h"
#include "RigidBodyComponent.h"
#include "AABBColliderComponent.h"

const float MAX_SPEED = 5000.0f;

RigidBodyComponent::RigidBodyComponent(class Actor* owner, float mass, float friction, bool applyGravity, int updateOrder)
        :Component(owner, updateOrder)
        ,mMass(mass)
        ,mApplyGravity(applyGravity)
        ,mFrictionCoefficient(friction)
        ,mVelocity(Vector3::Zero)
        ,mAcceleration(Vector3::Zero)
{

}

void RigidBodyComponent::ApplyForce(const Vector3 &force)
{
    mAcceleration += force * (1.f/mMass);
}

void RigidBodyComponent::Update(float deltaTime)
{
    // Apply drag only in y, x and z axes
    Vector3 drag = -mFrictionCoefficient * Vector3(mVelocity.x, mVelocity.y, mVelocity.z);
    ApplyForce(drag);

    // Euler Integration
    mVelocity += mAcceleration * deltaTime;

    // Clamp velocity to max
    if (mVelocity.Length() > MAX_SPEED)
    {
        mVelocity.Normalize();
        mVelocity *= MAX_SPEED;
    }

    if(Math::NearlyZero(mVelocity.Length()))
    {
        mVelocity = Vector3::Zero;
    }

    mOwner->SetPosition(mOwner->GetPosition() + mVelocity * deltaTime);

    mAcceleration.Set(0.f, 0.f, 0.f);
}
