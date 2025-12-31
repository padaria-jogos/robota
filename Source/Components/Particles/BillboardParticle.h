//
// Created by heito on 12/23/25.
//

#pragma once
#include "Particle.h"

//TODO Olhar para camera

class BillboardParticle : public Particle
{
    public:
    BillboardParticle(class Game* game);

    void Awake(const Vector3& position, const Vector3& rotation, float lifetime) override;

    void SetTexture(const std::string& textureName);
    void SetAnimation(const std::vector<class Texture*>& textures, float fps);
    void SetStartScale(float scale) { mScale = scale; }

    void OnUpdate(float deltaTime) override;

    private:
     class MeshComponent* mMeshComp;
     float mScale;
     std::vector<class Texture*> mAnimTextures;
     float mAnimFPS;
     float mAnimTimer;
     int mCurrentFrameIndex;
};
