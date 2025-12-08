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
        , mOffset(Vector3::Zero)
        , mRotationOffset(Quaternion::Identity)
        , mMetallic(0.7f)
        , mUnlit(false)
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
        //Rotation
        Matrix4 rotMat = Matrix4::CreateFromQuaternion(mRotationOffset);

        // Translation
        Matrix4 transMat = Matrix4::CreateTranslation(mOffset);

        Matrix4 localTransform = rotMat * transMat;
        Matrix4 world = mOwner->GetWorldTransform();


        Matrix4 finalWorld = localTransform * world;

        // Set the world transform
        shader->SetMatrixUniform("uWorldTransform", finalWorld);

        // Set the metallic property
        shader->SetFloatUniform("uMetallic", mMetallic);
        
        // Set the unlit property
        shader->SetIntUniform("uUnlit", mUnlit ? 1 : 0);

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