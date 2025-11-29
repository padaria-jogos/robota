#include "MeshComponent.h"
#include "../../Renderer/Shader.h"
#include "../../Renderer/Mesh.h"
#include "../../Renderer/Renderer.h"
#include "../../Renderer/Texture.h"
#include "../../Renderer/VertexArray.h"
#include "../../Actors/Actor.h"
#include "../../Game.h"

MeshComponent::MeshComponent(Actor* owner, bool useCustomShader)
    : Component(owner)
    , mMesh(nullptr)
    , mTextureIndex(0)
    , mIsVisible(true)
    , mTextureOverride(nullptr)
    , mCustomShader(nullptr)
    , mUseCustomShader(useCustomShader)
    , mColor(Vector3(1.0f, 1.0f, 1.0f))
    , mAlpha(1.0f)
{
    mOwner->GetGame()->GetRenderer()->AddMeshComp(this);
}

MeshComponent::~MeshComponent()
{
    mOwner->GetGame()->GetRenderer()->RemoveMeshComp(this);
}

void MeshComponent::Draw(Shader* shader)
{
    if (mMesh && mIsVisible)
    {
        // Use custom shader if enabled
        Shader* activeShader = (mUseCustomShader && mCustomShader) ? mCustomShader : shader;
        activeShader->SetActive();

        // Set the world transform
        activeShader->SetMatrixUniform("uWorldTransform", mOwner->GetWorldTransform());

        // If using custom shader (particle shader), set color and alpha uniforms
        if (mUseCustomShader && mCustomShader)
        {
            activeShader->SetVectorUniform("uParticleColor",
                Vector4(mColor.x, mColor.y, mColor.z, 1.0f));
            activeShader->SetFloatUniform("uAlpha", mAlpha);
        }

        // Set the active texture
        Texture* t = mTextureOverride;
        if (!t) {
            t = mMesh->GetTexture(mTextureIndex);
        }

        if (t) {
            t->SetActive();
        }

        // Set the mesh's vertex array as active
        VertexArray* va = mMesh->GetVertexArray();
        va->SetActive();

        // Draw
        glDrawElements(GL_TRIANGLES, va->GetNumIndices(), GL_UNSIGNED_INT, nullptr);
    }
}