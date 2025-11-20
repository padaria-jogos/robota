#pragma once
#include <vector>
#include <array>
#include <string>
#include "../Math.h"

class Mesh
{
public:
    Mesh();
    ~Mesh();
    // Load/unload mesh
    bool Load(const std::string& fileName, class Renderer* renderer);
    void Unload();
    // Get the vertex array associated with this mesh
    class VertexArray* GetVertexArray() { return mVertexArray; }
    // Get a texture from specified index
    class Texture* GetTexture(size_t index);
    // Get name of shader
    const std::string& GetShaderName() const { return mShaderName; }
    // Get object space bounding sphere radius
    float GetRadius() const { return mRadius; }
    // Get bounds points
    const std::array<Vector3, 8>& GetBounds() const { return mBounds; }

    float GetDepth() const { return Math::Abs(mBounds[7].x - mBounds[0].x); }
    float GetWidth() const { return Math::Abs(mBounds[7].y - mBounds[0].y); }
    float GetHeight() const { return Math::Abs(mBounds[7].z - mBounds[0].z); }

private:
    // 8 corner points for bounds to help calculate rotated min/max
    std::array<Vector3, 8> mBounds;
    // Textures associated with this mesh
    std::vector<class Texture*> mTextures;
    // Vertex array associated with this mesh
    class VertexArray* mVertexArray;
    // Name of shader specified by mesh
    std::string mShaderName;
    // Stores object space bounding sphere radius
    float mRadius;
};