//
// Created by andre on 08/12/2025.
//

#pragma once
#include "MeshComponent.h"

class ParticleMeshComponent : public MeshComponent
{
public:
    ParticleMeshComponent(class Actor* owner, bool isTransparent = false);

    void Draw(class Shader* shader) override;

    void SetParticleColor(const Vector3& color) { mParticleColor = Vector4(color.x, color.y, color.z, 1.0f); }
    void SetParticleColor(const Vector4& color) { mParticleColor = color; }
    void SetAlpha(float alpha) { mAlpha = alpha; }

private:
    Vector4 mParticleColor = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
    float mAlpha = 1.0f;
};