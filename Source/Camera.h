//
// Created by Lucas N. Ferreira on 03/11/25.
//

#pragma once

#include "Math.h"

class Camera
{
public:
    Camera(class Game* game, const Vector3 &eye, const Vector3 &target, const Vector3 &up,
           float foy, float near, float far);
    void Update(float deltaTime, class Actor *targetActor);

    // Get/set position
    Vector3 GetPosition() const { return mEye; }
    void SetPosition(const Vector3& pos) { mEye = pos; }

    void SetHDistance(const float hDist) { mHDistance = hDist; }
    void SetVDistance(const float hDist) { mVDistance = hDist; }
    void SetTDistance(const float hDist) { mTDistance = hDist; }

private:
    // Game
    class Game* mGame;

    float mHDistance;
    float mVDistance;
    float mTDistance;

    float mFovY;
    float mNear;
    float mFar;

    Vector3 mEye;
    Vector3 mTarget;
    Vector3 mUp;

    Matrix4 mPerspProjMatrix;
    Matrix4 mViewMatrix;
};
