//
// Created by andre on 08/12/2025.
//

#include "ParticleMeshComponent.h"
#include "../../Renderer/Shader.h"
#include "../../Renderer/Mesh.h"
#include "../../Renderer/Renderer.h"
#include "../../Renderer/Texture.h"
#include "../../Renderer/VertexArray.h"
#include "../../Actors/Actor.h"
#include "../../Game.h"

ParticleMeshComponent::ParticleMeshComponent(Actor* owner)
    : MeshComponent(owner)
{
    SetUnlit(true);
}

void ParticleMeshComponent::Draw(Shader* shader)
{
    if (!mMesh || !mIsVisible) return;

    Shader* particleShader = mOwner->GetGame()->GetRenderer()->GetParticleShader();
    if (!particleShader) {
        // Fallback para shader normal
        MeshComponent::Draw(shader);
        return;
    }

    Shader* previousShader = shader;

    particleShader->SetActive();

    // Transform
    Matrix4 rotMat = Matrix4::CreateFromQuaternion(mRotationOffset);
    Matrix4 transMat = Matrix4::CreateTranslation(mOffset);
    Matrix4 localTransform = rotMat * transMat;
    Matrix4 world = mOwner->GetWorldTransform();
    Matrix4 finalWorld = localTransform * world;

    particleShader->SetMatrixUniform("uWorldTransform", finalWorld);
    particleShader->SetMatrixUniform("uViewProj",
        mOwner->GetGame()->GetRenderer()->GetViewMatrix() *
        mOwner->GetGame()->GetRenderer()->GetProjectionMatrix());

    particleShader->SetVectorUniform("uParticleColor", mParticleColor);
    particleShader->SetFloatUniform("uAlpha", mAlpha);

    // Textura
    Texture* t = mTextureOverride;
    if (!t) {
        t = mMesh->GetTexture(mTextureIndex);
    }
    if (t) {
        t->SetActive();
    }

    VertexArray* va = mMesh->GetVertexArray();
    va->SetActive();
    glDrawElements(GL_TRIANGLES, va->GetNumIndices(), GL_UNSIGNED_INT, nullptr);

    if (previousShader) {
        previousShader->SetActive();
    }
}