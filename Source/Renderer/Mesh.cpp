#include <fstream>
#include <sstream>
#include <SDL2/SDL_log.h>
#include "Mesh.h"
#include "Renderer.h"
#include "Texture.h"
#include "VertexArray.h"
#include "../Json.h"

Mesh::Mesh()
        : mVertexArray(nullptr)
        , mRadius(0.0f)
{
}

Mesh::~Mesh()
{
}

bool Mesh::Load(const std::string& fileName, Renderer* renderer)
{
    std::ifstream file(fileName);
    if (!file.is_open())
    {
        SDL_Log("File not found: Mesh %s", fileName.c_str());
        return false;
    }

    nlohmann::json meshData = nlohmann::json::parse(file);

    if (meshData.is_null()) {
        SDL_Log("Failed to parse sprite sheet data file: %s", fileName.c_str());
        return false;
    }

    int ver = meshData["version"].get<int>();

    // Check the version
    if (ver != 1)
    {
        SDL_Log("Mesh %s not version 1", fileName.c_str());
        return false;
    }

    mShaderName = meshData["shader"].get<std::string>();

    // Skip the vertex format/shader for now
    // (This is changed in a later chapter's code)
    size_t vertSize = 8;

    // Load textures using nlohmann
    nlohmann::json textures = meshData["textures"];

    if (!textures.is_array() || textures.empty())
    {
        SDL_Log("Mesh %s has no textures, there should be at least one", fileName.c_str());
        return false;
    }

    for (size_t i = 0; i < textures.size(); ++i)
    {
        // Is this texture already loaded?
        std::string texName = textures[i].get<std::string>();
        Texture* t = renderer->GetTexture(texName);
        if (t == nullptr)
        {
            // If it's null, use the default texture
            t = renderer->GetTexture("../Assets/Textures/Default.png");
        }
        mTextures.emplace_back(t);
    }

    // Load in the vertices
    nlohmann::json vertsJson = meshData["vertices"];

    if (!vertsJson.is_array() || vertsJson.empty())
    {
        SDL_Log("Mesh %s has no vertices", fileName.c_str());
        return false;
    }

    std::vector<float> vertices;
    vertices.reserve(vertsJson.size() * vertSize);

    mRadius = 0.0f;
    Vector3 minPoint(Math::Infinity);
    Vector3 maxPoint(Math::NegInfinity);

    for (size_t i = 0; i < vertsJson.size(); ++i)
    {
        // For now, just assume we have 8 elements
        const auto& vert = vertsJson[i];
        if (!vert.is_array() || vert.size() != 8)
        {
            SDL_Log("Unexpected vertex format for %s", fileName.c_str());
            return false;
        }

        Vector3 pos(
                static_cast<float>(vert[0].get<double>()),
                static_cast<float>(vert[1].get<double>()),
                static_cast<float>(vert[2].get<double>())
        );
        mRadius = Math::Max(mRadius, pos.LengthSq());

        // Update min/max points
        minPoint.x = Math::Min(minPoint.x, pos.x);
        minPoint.y = Math::Min(minPoint.y, pos.y);
        minPoint.z = Math::Min(minPoint.z, pos.z);
        maxPoint.x = Math::Max(maxPoint.x, pos.x);
        maxPoint.y = Math::Max(maxPoint.y, pos.y);
        maxPoint.z = Math::Max(maxPoint.z, pos.z);

        // Add the floats
        for (size_t j = 0; j < vert.size(); ++j)
        {
            vertices.emplace_back(static_cast<float>(vert[j].get<double>()));
        }
    }

    // We were computing length squared earlier
    mRadius = Math::Sqrt(mRadius);

    // Now calculate the bounds array
    mBounds[0] = minPoint;
    mBounds[1] = Vector3(maxPoint.x, minPoint.y, minPoint.z);
    mBounds[2] = Vector3(minPoint.x, maxPoint.y, minPoint.z);
    mBounds[3] = Vector3(minPoint.x, minPoint.y, maxPoint.z);
    mBounds[4] = Vector3(minPoint.x, maxPoint.y, maxPoint.z);
    mBounds[5] = Vector3(maxPoint.x, minPoint.y, maxPoint.z);
    mBounds[6] = Vector3(maxPoint.x, maxPoint.y, minPoint.z);
    mBounds[7] = maxPoint;

    // Load in the indices
    nlohmann::json indJson = meshData["indices"];

    if (!indJson.is_array() || indJson.empty())
    {
        SDL_Log("Mesh %s has no indices", fileName.c_str());
        return false;
    }

    std::vector<unsigned int> indices;
    indices.reserve(indJson.size() * 3);

    for (size_t i = 0; i < indJson.size(); ++i)
    {
        const auto& ind = indJson[i];
        if (!ind.is_array() || ind.size() != 3)
        {
            SDL_Log("Invalid indices for %s", fileName.c_str());
            return false;
        }

        indices.emplace_back(ind[0].get<unsigned int>());
        indices.emplace_back(ind[1].get<unsigned int>());
        indices.emplace_back(ind[2].get<unsigned int>());
    }

    // Now create a vertex array
    mVertexArray = new VertexArray(
            vertices.data(), static_cast<unsigned>(vertices.size()) / static_cast<unsigned>(vertSize),
            indices.data(), static_cast<unsigned>(indices.size()));
    return true;
}

void Mesh::Unload()
{
    delete mVertexArray;
    mVertexArray = nullptr;
}

Texture* Mesh::GetTexture(size_t index)
{
    if (index < mTextures.size())
    {
        return mTextures[index];
    }
    else
    {
        return nullptr;
    }
}
