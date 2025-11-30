#include "ExplosionParticle.h"
#include "../Drawing/MeshComponent.h"
#include "../../Game.h"

ExplosionParticle::ExplosionParticle(class Game* game)
    : Particle(game, ParticleType::Billboard)
{
    // Use cube mesh for explosion
    Mesh* mesh = game->GetRenderer()->GetMesh("../Assets/Cube.gpmesh");
    
    if (!mesh)
        SDL_Log("Failed to load Cube mesh for explosion");
    
    mMeshComponent = new MeshComponent(this, true); // Use particle shader
    mMeshComponent->SetMesh(mesh);
    mMeshComponent->SetCustomShader(game->GetRenderer()->GetShader("Particle"));
    mMeshComponent->SetVisible(false);
    
    // Orange/red explosion color
    SetColor(Vector3(1.0f, 0.4f, 0.1f));
    SetScale(Vector3(1.0f, 1.0f, 1.0f));
    
    // Enable visual effects
    SetFadeOut(true);
    SetGrow(true, 3.0f); // Grows 3x its initial size
}

ExplosionParticle::~ExplosionParticle()
{
}

void ExplosionParticle::UpdateVisuals(float deltaTime)
{
    Particle::UpdateVisuals(deltaTime);
    
    // Additional explosion-specific visual effects can go here
    // For example: color shift from orange to red over time
}