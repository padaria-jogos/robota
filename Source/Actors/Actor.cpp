// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Actor.h"
#include "../Game.h"
#include "../Components/Component.h"
#include <algorithm>

Actor::Actor(Game* game)
        : mState(ActorState::Active)
        , mPosition(Vector3::Zero)
        , mScale(Vector3::One)
        , mRotation(0.0f)
        , mGame(game)
        , mIsTransformDirty(false)
{
    mGame->AddActor(this);
}

Actor::~Actor()
{
    mGame->RemoveActor(this);

    for(auto component : mComponents)
    {
        delete component;
    }
    mComponents.clear();
}

void Actor::Update(float deltaTime)
{
    if (mState == ActorState::Active)
    {
        for (auto comp : mComponents)
        {
            if (comp->IsEnabled()) {
                comp->Update(deltaTime);
            }
        }

        OnUpdate(deltaTime);
    }
}

void Actor::OnUpdate(float deltaTime)
{

}

void Actor::ProcessInput(const Uint8* keyState)
{
    if (mState == ActorState::Active)
    {
        for (auto comp : mComponents)
        {
            if (comp->IsEnabled()) {
                comp->ProcessInput(keyState);
            }
        }

        OnProcessInput(keyState);
    }
}

void Actor::OnProcessInput(const Uint8* keyState)
{

}

const Matrix4& Actor::GetWorldTransform()
{
    if (mIsTransformDirty)
    {
        mIsTransformDirty = false;
        mWorldTransform = Matrix4::CreateScale(mScale) *
                          Matrix4::CreateRotationZ(mRotation.z) *
                          Matrix4::CreateRotationX(mRotation.x) *
                          Matrix4::CreateRotationY(mRotation.y) *
                          Matrix4::CreateTranslation(mPosition);
    }

    return mWorldTransform;
}

void Actor::AddComponent(Component* c)
{
    mComponents.emplace_back(c);
    std::sort(mComponents.begin(), mComponents.end(), [](Component* a, Component* b) {
        return a->GetUpdateOrder() < b->GetUpdateOrder();
    });
}