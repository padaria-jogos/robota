#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <SDL2/SDL.h>
#include "../Math.h"
#include "VertexArray.h"
#include "Texture.h"
#include "Mesh.h"
#include "Font.h"

enum class RendererMode
{
    TRIANGLES,
    LINES
};

class Renderer
{
public:
	Renderer(SDL_Window* window);
	~Renderer();

	bool Initialize(float width, float height);
	void Shutdown();
    void UnloadData();

    void AddMeshComp(class MeshComponent* mesh);
    void RemoveMeshComp(class MeshComponent* mesh);

    void AddUIElement(class UIElement *comp);
    void RemoveUIElement(class UIElement *comp);

    void Clear();
    void Draw();
    void Present();

    // Getters
    class Texture* GetTexture(const std::string& fileName);
    class Font* GetFont(const std::string& fileName);
    class Mesh* GetMesh(const std::string& fileName);

    void SetViewMatrix(const Matrix4& view) { mView = view; }
    void SetProjectionMatrix(const Matrix4& proj) { mProjection = proj; }

private:
    void Draw(RendererMode mode, const Matrix4 &modelMatrix, const Vector2 &cameraPos, VertexArray *vertices,
              const Vector3 &color,  Texture *texture = nullptr, const Vector4 &textureRect = Vector4::UnitRect, float textureFactor = 1.0f);

	bool LoadShaders();
    void CreateSpriteVerts();

	// Sprite shader
	class Shader* mSpriteShader;
    // Mesh shader
    class Shader* mMeshShader;

    // Sprite vertex array
    class VertexArray *mSpriteVerts;

	// Window
	SDL_Window* mWindow;

	// OpenGL context
	SDL_GLContext mContext;

    // View/projection for 3D shaders
    Matrix4 mView;
    Matrix4 mProjection;

    // Map of textures loaded
    std::unordered_map<std::string, class Texture*> mTextures;
    // Map of fonts loaded
    std::unordered_map<std::string, class Font*> mFonts;
    // Map of meshes loaded
    std::unordered_map<std::string, Mesh*> mMeshes;

    // All mesh components drawn
    std::vector<class MeshComponent*> mMeshComps;

    // UI screens to draw
    std::vector<class UIElement*> mUIComps;

    // Width/height of screem
    float mScreenWidth;
    float mScreenHeight;
};