//
// Created by Lucas N. Ferreira on 02/11/25.
//

#pragma once

#include "../Component.h"
#include <cstddef>

class MeshComponent : public Component
{
public:
    MeshComponent(class Actor* owner);
    ~MeshComponent();

    // Draw this mesh component
    virtual void Draw(class Shader* shader);

    // Set/get the visibility of the mesh
    void SetVisible(bool visible) { mIsVisible = visible; }
    bool IsVisible() const { return mIsVisible; }

    // Set the mesh/texture index used by mesh component
    virtual void SetMesh(class Mesh* mesh) { mMesh = mesh; }
    void SetTextureOverride(class Texture* texture) { mTextureOverride = texture; }
    void SetTextureIndex(size_t index) { mTextureIndex = index; }

    // Pos
    void SetPositionOffset(const Vector3& offset) { mOffset = offset; }

    void SetRotationOffset(const Quaternion& rot) { mRotationOffset = rot; }
    void SetRotationOffset(float angle, const Vector3& axis) {
        mRotationOffset = Quaternion(axis, angle);
    }

protected:
    class Mesh* mMesh;
    size_t mTextureIndex;
    bool mIsVisible;
    class Texture* mTextureOverride;
    Vector3 mOffset;
    Quaternion mRotationOffset;
};