#include <GL/glew.h>
#include "Renderer.h"
#include "Shader.h"
#include "../Components/Drawing/MeshComponent.h"
#include "../UI/UIElement.h"
#include <algorithm>

Renderer::Renderer(SDL_Window *window)
: mSpriteShader(nullptr)
, mMeshShader(nullptr)
, mParticleShader(nullptr)
, mContext(nullptr)
, mWindow(window)
, mScreenWidth(1024.0f)
, mScreenHeight(768.0f)
{

}

Renderer::~Renderer()
{

}

bool Renderer::Initialize(float width, float height)
{
    mScreenWidth = width;
    mScreenHeight = height;

    // Specify version 3.3 (core profile)
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    // Enable double buffering
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    // Force OpenGL to use hardware acceleration
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);

    // Turn on vsync
    SDL_GL_SetSwapInterval(1);

    // Create an OpenGL context
    mContext = SDL_GL_CreateContext(mWindow);

    // Initialize GLEW
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        SDL_Log("Failed to initialize GLEW.");
        return false;
    }

    // Make sure we can create/compile shaders
    if (!LoadShaders())
    {
        SDL_Log("Failed to load shaders.");
        return false;
    }

    // Create quad for drawing sprites
    CreateSpriteVerts();

    // Set the clear color to light grey
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    return true;
}

void Renderer::AddMeshComp(MeshComponent* mesh)
{
    mMeshComps.emplace_back(mesh);
}

void Renderer::RemoveMeshComp(MeshComponent* mesh)
{
    auto iter = std::find(mMeshComps.begin(), mMeshComps.end(), mesh);
    mMeshComps.erase(iter);
}

void Renderer::AddUIElement(UIElement *comp)
{
    mUIComps.emplace_back(comp);

    std::sort(mUIComps.begin(), mUIComps.end(),[](UIElement* a, UIElement* b) {
        return a->GetDrawOrder() < b->GetDrawOrder();
    });
}

void Renderer::RemoveUIElement(UIElement *comp)
{
    auto iter = std::find(mUIComps.begin(), mUIComps.end(), comp);
    mUIComps.erase(iter);
}

void Renderer::UnloadData()
{
    // Destroy textures
    for (auto i : mTextures)
    {
        i.second->Unload();
        delete i.second;
    }
    mTextures.clear();

    // Destroy meshes
    for (auto i : mMeshes)
    {
        i.second->Unload();
        delete i.second;
    }
    mMeshes.clear();

    // Destroy fonts
    for (auto i : mFonts)
    {
        i.second->Unload();
        delete i.second;
    }
    mFonts.clear();
}

void Renderer::Shutdown()
{
    UnloadData();

    delete mSpriteVerts;
    mSpriteVerts = nullptr;

    mSpriteShader->Unload();
    delete mSpriteShader;

    mMeshShader->Unload();
    delete mMeshShader;

    mParticleShader->Unload();
    delete mParticleShader;

    mShaders.clear();

    SDL_GL_DeleteContext(mContext);
	SDL_DestroyWindow(mWindow);
}

void Renderer::Clear()
{
    // Clear the color buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::Draw()
{
    // Enable depth buffering/disable alpha blend
    glEnable(GL_DEPTH_TEST);
    //glDisable(GL_BLEND); HEITOR
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Set the mesh shader active
    mMeshShader->SetActive();

    // Update view-projection matrix
    mMeshShader->SetMatrixUniform("uViewProj", mView * mProjection);

    // Draw mesh components
    for (auto mc : mMeshComps)
    {
        mc->Draw(mMeshShader);
    }

    // Disable depth buffering
    glDisable(GL_DEPTH_TEST);

    // Enable alpha blending on the color buffer
    glEnable(GL_BLEND);
    glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ZERO, GL_ONE);

    // Activate sprite shader/verts
    mSpriteShader->SetActive();
    mSpriteVerts->SetActive();

    // Draw UI components
    for (auto ui : mUIComps)
    {
        ui->Draw(mSpriteShader);
    }
}

void Renderer::Present()
{
	// Swap the buffers
	SDL_GL_SwapWindow(mWindow);
}

bool Renderer::LoadShaders()
{
    // Create sprite shader
    mSpriteShader = new Shader();
    if (!mSpriteShader->Load("../Shaders/Sprite"))
    {
        return false;
    }

    mSpriteShader->SetActive();

    // Set the view-projection matrix
    Matrix4 viewProj = Matrix4::CreateSimpleViewProj(mScreenWidth, mScreenHeight);
    mSpriteShader->SetMatrixUniform("uViewProj", viewProj);

    // Create basic mesh shader
    mMeshShader = new Shader();
    if (!mMeshShader->Load("../Shaders/Mesh"))
    {
        return false;
    }

    mMeshShader->SetActive();

    // Set the view-projection matrix
    mView = Matrix4::Identity;
    mProjection = Matrix4::CreateOrtho(mScreenWidth, mScreenHeight, 1000.0f, -1000.0f);
    mMeshShader->SetMatrixUniform("uViewProj", mView * mProjection);

    // Create particle shader
    mParticleShader = new Shader();
    if (!mParticleShader->Load("../Shaders/Particle"))
    {
        return false;
    }
    
    mParticleShader->SetActive();
    mParticleShader->SetMatrixUniform("uViewProj", mView * mProjection);
    
    // Add shaders to map
    mShaders["Sprite"] = mSpriteShader;
    mShaders["Mesh"] = mMeshShader;
    mShaders["Particle"] = mParticleShader;

    return true;
}

Shader* Renderer::GetShader(const std::string& shaderName)
{
    auto iter = mShaders.find(shaderName);
    if (iter != mShaders.end())
    {
        return iter->second;
    }
    return nullptr;
}

void Renderer::CreateSpriteVerts()
{
    float vertices[] = {
            -0.5f, 0.5f,  0.f, 0.f, 0.f, 0.0f, 0.f, 0.f, // top left
            0.5f,  0.5f,  0.f, 0.f, 0.f, 0.0f, 1.f, 0.f, // top right
            0.5f,  -0.5f, 0.f, 0.f, 0.f, 0.0f, 1.f, 1.f, // bottom right
            -0.5f, -0.5f, 0.f, 0.f, 0.f, 0.0f, 0.f, 1.f	 // bottom left
    };

    unsigned int indices[] = {0, 1, 2, 2, 3, 0};

    mSpriteVerts = new VertexArray(vertices, 4, indices, 6);
}


Texture* Renderer::GetTexture(const std::string& fileName)
{
    Texture* tex = nullptr;
    auto iter = mTextures.find(fileName);
    if (iter != mTextures.end())
    {
        tex = iter->second;
    }
    else
    {
        tex = new Texture();
        if (tex->Load(fileName))
        {
            mTextures.emplace(fileName, tex);
            return tex;
        }
        else
        {
            delete tex;
            return nullptr;
        }
    }
    return tex;
}

Font* Renderer::GetFont(const std::string& fileName)
{
    auto iter = mFonts.find(fileName);
    if (iter != mFonts.end())
    {
        return iter->second;
    }
    else
    {
        Font* font = new Font();
        if (font->Load(fileName))
        {
            mFonts.emplace(fileName, font);
        }
        else
        {
            font->Unload();
            delete font;
            font = nullptr;
        }
        return font;
    }
}

Mesh* Renderer::GetMesh(const std::string& fileName)
{
    Mesh* m = nullptr;
    auto iter = mMeshes.find(fileName);
    if (iter != mMeshes.end())
    {
        m = iter->second;
    }
    else
    {
        m = new Mesh();
        if (m->Load(fileName, this))
        {
            mMeshes.emplace(fileName, m);
            return m;
        }
        else
        {
            delete m;
            return nullptr;
        }
    }
    return m;
}