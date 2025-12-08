//
// Created by Lucas N. Ferreira on 09/11/25.
//

#pragma once

#include "../../Actors/Actor.h"

class Particle : public Actor
{
public:
    Particle(class Game* game);

    void OnUpdate(float deltaTime) override;

    virtual void Awake(const Vector3 &position, const Vector3 &rotation, float lifetime = 1.0f);
    virtual void Emit(const Vector3 &direction, float speed);
    virtual void Kill();

    bool IsDead() const { return mIsDead; }

    float GetLifeTime() const { return mLifeTime; }

private:
    float mLifeTime;
    bool mIsDead;
};