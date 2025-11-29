#pragma once

#include "Particle.h"

class Bullet : public Particle
{
public:
    Bullet(class Game* game);
    ~Bullet();

    void OnUpdate(float deltaTime) override;
    void Kill() override;
    void Awake(const Vector3 &position, const Vector3 &rotation, float lifetime) override;

private:
    class AABBColliderComponent* mCollisionComponent;
};