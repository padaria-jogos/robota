//
// Created by Lucas N. Ferreira on 04/11/25.
//

#pragma once

#include "Particle.h"

class Bullet : public Particle
{
public:
    Bullet(class Game* game);
    ~Bullet();

    void OnUpdate(float deltaTime) override;

    void Kill() override;
    void Emit(const Vector3 &direction, float speed) override;
    void Awake(const Vector3 &position, const Vector3 &rotation, float lifetime) override;

protected:
    class MeshComponent *mMeshComponent;
    class AABBColliderComponent* mCollisionComponent;
    class RigidBodyComponent *mRigidBody;
};
