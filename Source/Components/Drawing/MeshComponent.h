//
// Created by Lucas N. Ferreira on 02/11/25.
//

#pragma once

#include "../Component.h"
#include <cstddef>

class MeshComponent : public Component
{
public:
    MeshComponent(class Actor* owner, bool useCustomShader = false);
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

    // Controle de cor/alpha para partículas
    void SetColor(const Vector3& color) { mColor = color; }
    void SetAlpha(float alpha) { mAlpha = alpha; }
    const Vector3& GetColor() const { return mColor; }
    float GetAlpha() const { return mAlpha; }
    void SetCustomShader(class Shader* shader) { mCustomShader = shader; }
    void SetUseCustomShader(bool use) { mUseCustomShader = use; }

protected:
    class Mesh* mMesh;
    size_t mTextureIndex;
    bool mIsVisible;
    class Texture* mTextureOverride;

    class Shader* mCustomShader;
    bool mUseCustomShader;

    Vector3 mColor;
    float mAlpha;
};