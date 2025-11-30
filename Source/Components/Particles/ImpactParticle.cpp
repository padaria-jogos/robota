#include "ImpactParticle.h"
#include "../Drawing/MeshComponent.h"
#include "../../Game.h"

ImpactParticle::ImpactParticle(class Game* game)
    : Particle(game, ParticleType::Billboard)
{
    // Use cube mesh for impact
    Mesh* mesh = game->GetRenderer()->GetMesh("../Assets/Cube.gpmesh");
    
    if (!mesh)
        SDL_Log("Failed to load Cube mesh for impact");
    
    mMeshComponent = new MeshComponent(this, true);
    mMeshComponent->SetMesh(mesh);
    mMeshComponent->SetCustomShader(game->GetRenderer()->GetShader("Particle"));
    mMeshComponent->SetVisible(false);
    
    // Blue/white impact color
    SetColor(Vector3(0.3f, 0.7f, 1.0f));
    SetScale(Vector3(2.0f, 2.0f, 2.0f));
    
    SetFadeOut(true);
    SetGrow(false); // Impacts don't grow
}

ImpactParticle::~ImpactParticle()
{
}