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
      ,mCameraAngleIndex(0)
{
    // create projective perspective matrix
    mPerspProjMatrix = Matrix4::CreatePerspectiveFOV(mFovY, Game::WINDOW_WIDTH, Game::WINDOW_HEIGHT, mNear, mFar);
    mViewMatrix = Matrix4::CreateLookAt(mEye, mTarget, mUp);

    mGame->GetRenderer()->SetProjectionMatrix(mPerspProjMatrix);
    mGame->GetRenderer()->SetViewMatrix(mViewMatrix);
    mGame->GetRenderer()->SetCameraPosition(mEye);
}

void Camera::SetTargetActor(class Actor* actor)
{
    mTargetActor = actor;
}

void Camera::Update(float deltaTime)
{
    static int frameCount = 0;
    if (frameCount++ % 60 == 0) {  // Log a cada 60 frames
        SDL_Log("Camera::Update called - mCameraAngleIndex = %d", mCameraAngleIndex);
    }
    
    // Calcula a posição alvo (centro da arena)
    Vector3 targetPos;
    if (mTargetActor)
    {
        targetPos = mTargetActor->GetPosition();
    }
    else{
        targetPos = Vector3::Zero;
    }
    
    // Calcula o ângulo de rotação em radianos baseado no índice
    float angle = mCameraAngleIndex * Math::PiOver2;

    // Distância orbital da câmera
    float orbitRadius = 2500.0f;
    float cameraHeight = 1000.0f;
    
    // Calcula a posição da câmera rotacionando ao redor do eixo Z
    float cosAngle = Math::Cos(angle);
    float sinAngle = Math::Sin(angle);

    mEye.x = targetPos.x + orbitRadius * sinAngle;
    mEye.y = targetPos.y - orbitRadius * cosAngle;
    mEye.z = cameraHeight;

    mTarget = targetPos;

    static int logCounter = 0;
    if (logCounter++ % 60 == 0) {
        SDL_Log("Camera Position - Angle: %d, Eye: (%.1f, %.1f, %.1f), Target: (%.1f, %.1f, %.1f)", 
                mCameraAngleIndex,
                mEye.x, mEye.y, mEye.z,
                mTarget.x, mTarget.y, mTarget.z);
    }

    // create 'look at' matrix with update parameters
    mViewMatrix = Matrix4::CreateLookAt(mEye, mTarget, mUp);

    // send info to renderer
    mGame->GetRenderer()->SetViewMatrix(mViewMatrix);
    mGame->GetRenderer()->SetCameraPosition(mEye);
}

void Camera::RotateLeft()
{
    mCameraAngleIndex = (mCameraAngleIndex + 1) % 4;
    SDL_Log("RotateLeft: mCameraAngleIndex = %d (%.0f degrees)", mCameraAngleIndex, mCameraAngleIndex * 90.0f);

    Update(0.0f);
}

void Camera::RotateRight()
{
    mCameraAngleIndex = (mCameraAngleIndex - 1 + 4) % 4;
    SDL_Log("RotateRight: mCameraAngleIndex = %d (%.0f degrees)", mCameraAngleIndex, mCameraAngleIndex * 90.0f);
    Update(0.0f);
}