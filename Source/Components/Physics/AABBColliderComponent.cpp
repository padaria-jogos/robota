//
// Created by Lucas N. Ferreira on 28/09/23.
//

#include "AABBColliderComponent.h"
#include "../../Actors/Actor.h"
#include "../../Game.h"

AABBColliderComponent::AABBColliderComponent(class Actor* owner, float w, float h, float d, const Vector3 &offset,
        ColliderLayer layer, bool isStatic, int updateOrder)
        :Component(owner, updateOrder)
        ,mOffset(offset)
        ,mWidth(w)
        ,mHeight(h)
        ,mDepth(d)
        ,mIsStatic(isStatic)
        ,mLayer(layer)
{

}

AABBColliderComponent::~AABBColliderComponent()
{

}

Vector3 AABBColliderComponent::GetMin() const
{
    Vector3 v = mOwner->GetPosition();
    v.x -= mDepth * mOwner->GetScale().x / 2.0f;
    v.y -= mWidth * mOwner->GetScale().y / 2.0f;
    v.z -= mHeight * mOwner->GetScale().z / 2.0f;
    return v;
}

Vector3 AABBColliderComponent::GetMax() const
{
    Vector3 v = mOwner->GetPosition();
    v.x += mDepth * mOwner->GetScale().x / 2.0f;
    v.y += mWidth * mOwner->GetScale().y / 2.0f;
    v.z += mHeight * mOwner->GetScale().z / 2.0f;
    return v;
}

bool AABBColliderComponent::Intersect(const AABBColliderComponent *b) const
{
    Vector3 thisMin = GetMin();
    Vector3 thisMax = GetMax();
    Vector3 otherMin = b->GetMin();
    Vector3 otherMax = b->GetMax();

    bool noIntersection = thisMax.x < otherMin.x || thisMax.y < otherMin.y ||
                          thisMax.z < otherMin.z || otherMax.x < thisMin.x ||
                          otherMax.y < thisMin.y || otherMax.z < thisMin.z;

    return !noIntersection;
}
