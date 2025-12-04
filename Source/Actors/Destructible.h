//
// Created by Heitor S. on 12/4/2025.
//

#pragma once
#include "Actor.h"

class Destructible : public Actor {
public:
    Destructible(class Game* game);
    
    void SetMesh(class Mesh* mesh);
    void SetTexture(const std::string& texturePath);

    virtual void OnDestroy();
    
private:
    class MeshComponent* mMeshComponent;
};
