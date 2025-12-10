//
// Created by Lucas N. Ferreira on 03/11/25.
//

#include "Camera.h"
#include "Game.h"
#include "Actors/Actor.h"
#include "Math.h"

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
      ,mIsTransitioning(false)
      ,mTransitionTime(0.5f)
      ,mTransitionElapsed(0.0f)
      ,mIsFreeCameraMode(false)
      ,mIsInCutscene(false)
      ,mCutsceneIndex(0)
{
    // define default camera pose as first
    CameraPose start_pose;
    start_pose.position = mEye;
    start_pose.target = mTarget;
    start_pose.up = mUp;

    mCameraPoses.push_back(start_pose);

    // create projective perspective matrix
    mPerspProjMatrix = Matrix4::CreatePerspectiveFOV(mFovY, Game::WINDOW_WIDTH, Game::WINDOW_HEIGHT, mNear, mFar);
    mViewMatrix = Matrix4::CreateLookAt(mEye, mTarget, mUp);

    // set initial matrices in renderer
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
    // handle camera position interpolation
    if (mIsTransitioning)
    {
        mTransitionElapsed += deltaTime;

        float t = mTransitionElapsed / mTransitionTime;
        t = Math::Clamp(t, 0.0f, 1.0f);

        // smoothstep
        // https://en.wikipedia.org/wiki/Smoothstep
        float s = t * t * (3.0f - 2.0f * t);

        // correct eye and target position by interpolation
        mEye = Vector3::Lerp(mStartPose.position, mEndPose.position, s);
        mTarget = Vector3::Lerp(mStartPose.target,  mEndPose.target,   s);

        // recompute view matrix
        mViewMatrix = Matrix4::CreateLookAt(mEye, mTarget, mUp);
        mGame->GetRenderer()->SetViewMatrix(mViewMatrix);
        mGame->GetRenderer()->SetCameraPosition(mEye);

        // current transition complete
        if (t >= 1.0f)
        {
            mIsTransitioning = false;

            if (mIsInCutscene)
            {
                // if this is the last transition from cutscene
                if (mCutsceneKeyframes.size() == mCutsceneIndex + 1)
                {
                    mIsInCutscene = false;
                    mCutsceneIndex = 0;
                    SDL_Log("Cutscene ended");
                }
                // configure the next transition
                else
                {
                    mCutsceneIndex++;

                    mTransitionTime = mCutsceneKeyframes[mCutsceneIndex].duration;
                    mStartPose = mCutsceneKeyframes[mCutsceneIndex].start;
                    mEndPose = mCutsceneKeyframes[mCutsceneIndex].end;

                    mIsTransitioning = true;
                    mTransitionElapsed = 0.0f;
                }
            }
        }

        // Skip the rest of the update while transitioning
        return;
    }

    // deal with camera operations: dolly, truck, pedestal and tilt
    if (mIsFreeCameraMode)
    {
        // SDL_Log("Free-Camera Mode: %b", mIsFreeCameraMode);
    }
}

void Camera::HandleFreeCameraInput(int key)
{
    float tiltAngle = 0.0f;
    float panAngle  = 0.0f;

    float amount = 25.0f;
    Vector3 movement = Vector3::Zero;

    Vector3 cam_forward = Vector3::Normalize(mTarget - mEye);
    Vector3 cam_right   = Vector3::Normalize(Vector3::Cross(mUp, cam_forward));
    Vector3 cam_up      = mUp;

    switch (key)
    {
        // dolly (forward/backward)
        case SDLK_w:
            movement = cam_forward * amount;
            break;
        case SDLK_s:
            movement = cam_forward * -amount;
            break;

        // truck (left/right)
        case SDLK_a:
            movement = cam_right * -amount;
            break;
        case SDLK_d:
            movement = cam_right * amount;
            break;

        // pedestal (up/down)
        case SDLK_q:
            movement = cam_up * amount;
            break;
        case SDLK_e:
            movement = cam_up * -amount;
            break;

        // tilt
        case SDLK_UP:
            tiltAngle = Math::ToRadians(-1.0f);
            break;
        case SDLK_DOWN:
            tiltAngle = Math::ToRadians(1.0f);
            break;

        // pan
        case SDLK_LEFT:
            panAngle = Math::ToRadians(-1.0f);
            break;
        case SDLK_RIGHT:
            panAngle = Math::ToRadians(1.0f);
            break;
    }

    // apply translation
    mEye += movement;
    mTarget += movement;

    // compute distance to target (to maintain it after tilt/pan) (if something is wrong, set to 0)
    float dist = (mTarget - mEye).Length();

    // apply tilt
    if (tiltAngle != 0.0f)
    {
        Vector3 forward = Vector3::Normalize(mTarget - mEye);
        Vector3 right   = Vector3::Normalize(Vector3::Cross(mUp, forward));

        Quaternion q(right, tiltAngle);
        forward = Vector3::Normalize(Vector3::Transform(forward, q));

        mTarget = mEye + forward * dist;
    }

    // apply pan
    if (panAngle != 0.0f)
    {
        Vector3 forward = Vector3::Normalize(mTarget - mEye);

        Quaternion q(mUp, panAngle);
        forward = Vector3::Normalize(Vector3::Transform(forward, q));

        mTarget = mEye + forward * dist;
    }

    // update view matrix
    mViewMatrix = Matrix4::CreateLookAt(mEye, mTarget, mUp);
    mGame->GetRenderer()->SetViewMatrix(mViewMatrix);
    mGame->GetRenderer()->SetCameraPosition(mEye);

    // print camera position
    if (key == SDLK_RETURN)
        SDL_Log("Current Camera Pose: Position (%.1f, %.1f, %.1f) Target (%.1f, %.1f, %.1f)",
                mEye.x, mEye.y, mEye.z,
                mTarget.x, mTarget.y, mTarget.z);
}

void Camera::StartCutscene()
{
    SDL_Log("Starting Cutscene...");
    mTransitionTime = mCutsceneKeyframes[mCutsceneIndex].duration;
    mStartPose = mCutsceneKeyframes[mCutsceneIndex].start;
    mEndPose = mCutsceneKeyframes[mCutsceneIndex].end;

    mIsInCutscene = true;
    mIsTransitioning = true;
    mTransitionElapsed = 0.0f;
}


void Camera::HandleKeyPress(int key)
{
    // dont read input if transitioning
    if (mIsTransitioning)
        return;

    // toggle free camera mode (F)
    if (key == SDLK_F12)
    {
        mIsFreeCameraMode = mIsFreeCameraMode ? false : true;

        if (mIsFreeCameraMode)
            SDL_Log("Free-Camera Mode: ON \n Dolly (forward/backward): W/S \n Truck (left/right): A/D "
                    "\n Pedestal (up/down): Q/E \n Tilt: UP/DOWN \n Pan: LEFT/RIGHT");
        else
            SDL_Log("Free-Camera Mode: OFF");
    }

    // dont process other inputs in free camera mode
    if (mIsFreeCameraMode)
    {
        HandleFreeCameraInput(key);
        return;
    }

    // start cutscene
    if (key == SDLK_p)
    {
        if (mCutsceneKeyframes.empty())
            return;

        StartCutscene();
    }

    // cycle through camera poses
    if (key == SDLK_c)
    {
        // camera transition time
        mTransitionTime = 0.5f;

        mCameraAngleIndex = (int)(mCameraAngleIndex + 1) % mCameraPoses.size();
        CameraPose targetPose = mCameraPoses[mCameraAngleIndex];

        SDL_Log("Transitioning to Camera Pose %d", mCameraAngleIndex);

        mStartPose = { mEye, mTarget, mUp };
        mEndPose = targetPose;
        mIsTransitioning = true;
        mTransitionElapsed = 0.0f;
    }

}


void Camera::TransitionToSkyPose()
{
    // camera transition time
    mTransitionTime = 0.5f;

    mCameraAngleIndex = mCameraPoses.size() - 1;
    CameraPose targetPose = mCameraPoses[mCameraAngleIndex];

    SDL_Log("Transitioning to Camera Pose %d", mCameraAngleIndex);

    mStartPose = { mEye, mTarget, mUp };
    mEndPose = targetPose;
    mIsTransitioning = true;
    mTransitionElapsed = 0.0f;
}