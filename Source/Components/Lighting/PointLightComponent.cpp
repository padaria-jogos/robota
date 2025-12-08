#include "PointLightComponent.h"
#include "../../Actors/Actor.h"
#include "../../Game.h"
#include "../../Renderer/Renderer.h"

PointLightComponent::PointLightComponent(Actor* owner, int updateOrder)
    : Component(owner, updateOrder)
    , mColor(1.0f, 1.0f, 1.0f)  // Branco por padrão
    , mIntensity(1.0f)
    , mRadius(1000.0f)
    , mEnabled(true)
    , mOffset(0.0f, 0.0f, 0.0f)
{
    // Registra a luz no renderer
    mOwner->GetGame()->GetRenderer()->AddPointLight(this);
}

PointLightComponent::~PointLightComponent()
{
    // Remove a luz do renderer
    mOwner->GetGame()->GetRenderer()->RemovePointLight(this);
}

void PointLightComponent::Update(float deltaTime)
{
    // Atualiza automaticamente baseado na posição do owner
}

Vector3 PointLightComponent::GetPosition() const
{
    return mOwner->GetPosition() + mOffset;
}
