#include "MeshComponent.h"
#include "../../Renderer/Shader.h"
#include "../../Renderer/Mesh.h"
#include "../../Renderer/Renderer.h"
#include "../../Renderer/Texture.h"
#include "../../Renderer/VertexArray.h"
#include "../../Actors/Actor.h"
#include "../../Game.h"

MeshComponent::MeshComponent(Actor* owner)
        : Component(owner)
        , mMesh(nullptr)
        , mTextureIndex(0)
        , mIsVisible(true)
        , mTextureOverride(nullptr)
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
        // Set the world transform
        shader->SetMatrixUniform("uWorldTransform", mOwner->GetWorldTransform());

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