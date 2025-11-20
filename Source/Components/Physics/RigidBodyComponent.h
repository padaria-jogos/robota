//
// Created by Lucas N. Ferreira on 08/09/23.
//

#pragma once
#include "../Component.h"
#include "../../Math.h"

class RigidBodyComponent : public Component
{
public:
    // Lower update order to update first
    RigidBodyComponent(class Actor* owner, float mass = 1.0f, float friction = 0.0f,
                       bool applyGravity = true, int updateOrder = 10);

    void Update(float deltaTime) override;

    const Vector3& GetVelocity() const { return mVelocity; }
    void SetVelocity(const Vector3& velocity) { mVelocity = velocity; }

    const Vector3& GetAcceleration() const { return mAcceleration; }
    void SetAcceleration(const Vector3& acceleration) { mAcceleration = acceleration; }

    void SetApplyGravity(const bool applyGravity) { mApplyGravity = applyGravity; }

    void ApplyForce(const Vector3 &force);

private:
    bool mApplyGravity;

    // Physical properties
    float mFrictionCoefficient;
    float mMass;

    Vector3 mVelocity;
    Vector3 mAcceleration;
};
