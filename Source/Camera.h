//
// Created by Lucas N. Ferreira on 03/11/25.
//

#pragma once

#include "Math.h"
#include <vector>

struct CameraPose {
    Vector3 position;
    Vector3 target;
    Vector3 up;
};

struct TransictionKeyframe {
    CameraPose start;
    CameraPose end;
    float duration;
};

class Camera
{
public:
    Camera(class Game* game, const Vector3 &eye, const Vector3 &target, const Vector3 &up,
           float foy, float near, float far);
    void Update(float deltaTime);

    // Get/set position
    Vector3 GetPosition() const { return mEye; }
    Vector3 GetTarget() const { return mTarget; }

    void SetPosition(const Vector3& pos) { mEye = pos; }
    void SetHDistance(const float hDist) { mHDistance = hDist; }
    void SetVDistance(const float hDist) { mVDistance = hDist; }
    void SetTDistance(const float hDist) { mTDistance = hDist; }
    void SetTargetActor(class Actor* actor);
    void SetCutsceneKeyframes(const std::vector<TransictionKeyframe> keyframes) { mCutsceneKeyframes = keyframes; }

    bool IsFreeCameraMode() const { return mIsFreeCameraMode; }
    void AddCameraPose(CameraPose cp) { mCameraPoses.push_back(cp); }
    void HandleKeyPress(int key);
    void StartCutscene();

private:
    // Game
    class Game* mGame;

    float mHDistance;
    float mVDistance;
    float mTDistance;

    float mFovY;
    float mNear;
    float mFar;

    Actor* mTargetActor;

    Vector3 mEye;
    Vector3 mTarget;
    Vector3 mUp;

    int mCameraAngleIndex;

    // elementos para transição
    bool mIsTransitioning;
    float mTransitionTime; // segundos
    float mTransitionElapsed;
    CameraPose mStartPose;
    CameraPose mEndPose;

    // vector that holds predefined camera poses
    std::vector<CameraPose> mCameraPoses;

    // free camera mode
    bool mIsFreeCameraMode;
    void HandleFreeCameraInput(int key);

    // cutscenes
    std::vector<TransictionKeyframe> mCutsceneKeyframes;
    bool mIsInCutscene;
    int mCutsceneIndex;

    // camera matrices
    Matrix4 mPerspProjMatrix;
    Matrix4 mViewMatrix;
};
