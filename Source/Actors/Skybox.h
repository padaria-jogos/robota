//
// Created by mateus on 05/12/2025.
//

#pragma once
#include "Actor.h"

const float SIZE = 10000.0f;

class MeshComponent;

class Skybox : public Actor
{
public:
    Skybox(class Game* game);
    ~Skybox();

    void SetTexture(const std::string& path);
    void SetSize(float scale);

private:
    MeshComponent* mMeshComponent;
};