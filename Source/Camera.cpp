//
// Created by Lucas N. Ferreira on 03/11/25.
//

#include "Camera.h"
#include "Game.h"
#include "Actors/Actor.h"

Camera::Camera(class Game* game, const Vector3 &eye, const Vector3 &target, const Vector3 &up,
        float foy, float near, float far)
      :mGame(game)
      ,mEye(eye)
      ,mTarget(target)
      ,mUp(up)
      ,mFovY(foy)
      ,mNear(near)
      ,mFar(far)
      ,mHDistance(300.0f)
      ,mVDistance(0.0f)
      ,mTDistance(20.0f)
{
    // create projective perspective matrix
    mPerspProjMatrix = Matrix4::CreatePerspectiveFOV(mFovY, Game::WINDOW_WIDTH, Game::WINDOW_HEIGHT, mNear, mFar);
    mViewMatrix = Matrix4::CreateLookAt(mEye, mTarget, mUp);

    mGame->GetRenderer()->SetProjectionMatrix(mPerspProjMatrix);
    mGame->GetRenderer()->SetViewMatrix(mViewMatrix);
}

void Camera::Update(float deltaTime, Actor *targetActor)
{
    if (!targetActor)
        return;

    // calculate target posision and forward vector
    Vector3 targetPos = targetActor->GetPosition();
    Vector3 forward = targetActor->GetForward();
    forward.Normalize();

    // new camera position, keep z at 0 for an interesting effect
    mEye = targetPos - forward * mHDistance + mUp * mVDistance;
    mEye.z = 0.0f;

    // new target position
    mTarget = targetPos + forward * mTDistance;

    // create 'look at' matrix with update parameters
    mViewMatrix = Matrix4::CreateLookAt(mEye, mTarget, mUp);

    // send info to renderer
    mGame->GetRenderer()->SetViewMatrix(mViewMatrix);
}