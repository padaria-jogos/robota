    //
// Created by andre on 08/12/2025.
//

#pragma once
#include "Game.h"
#include "Actors/Actor.h"
#include "ParticleSystemComponent.h"
#include "BasicParticle.h"
#include "BillboardParticle.h"



class ParticleManager : public Actor
{
    public:
        ParticleManager(Game* game, int poolSize = 100);
        void CreateSmokeAtGrid(int x, int y, GridMap* grid) const;
        void CreateExplosionSphereAtGrid(int x, int y, GridMap* grid);
    private:
        ParticleSystemComponent<BillboardParticle>* mSmokePool;
        ParticleSystemComponent<BasicParticle>* mExplosionPool;

        std::vector<Texture*> mSmokeTextures;
        Mesh* mSphereMesh;

        BasicParticle* GetFreeExplosionParticle();
};
