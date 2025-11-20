//
// Created by Lucas N. Ferreira on 28/09/23.
//

#pragma once
#include "../Component.h"
#include "../../Math.h"
#include "RigidBodyComponent.h"
#include <vector>
#include <set>

enum class ColliderLayer
{
    Player,
    Block,
    Bullet
};

class AABBColliderComponent : public Component
{
public:

    AABBColliderComponent(class Actor* owner, float w, float h, float d, const Vector3 &offset,
                                ColliderLayer layer, bool isStatic = false, int updateOrder = 10);
    ~AABBColliderComponent() override;

    bool Intersect(const AABBColliderComponent* b) const;

    Vector3 GetMin() const;
    Vector3 GetMax() const;
    ColliderLayer GetLayer() const { return mLayer; }

private:
    Vector3 mOffset;

    float mWidth;
    float mHeight;
    float mDepth;

    bool mIsStatic;

    ColliderLayer mLayer;
};