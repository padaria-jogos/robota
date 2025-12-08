#include <GL/glew.h>
#include "Renderer.h"
#include "Shader.h"
#include "../Components/Drawing/MeshComponent.h"
#include "../Components/Lighting/PointLightComponent.h"
#include "../UI/UIElement.h"
#include <algorithm>

Renderer::Renderer(SDL_Window *window)
: mSpriteShader(nullptr)
, mMeshShader(nullptr)
, mContext(nullptr)
, mWindow(window)
, mScreenWidth(1024.0f)
, mScreenHeight(768.0f)
, mCameraPos(Vector3::Zero)
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

void Renderer::AddPointLight(PointLightComponent* light)
{
    mPointLights.emplace_back(light);
}

void Renderer::RemovePointLight(PointLightComponent* light)
{
    auto iter = std::find(mPointLights.begin(), mPointLights.end(), light);
    if (iter != mPointLights.end())
    {
        mPointLights.erase(iter);
    }
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

    if (mParticleShader) {
        mParticleShader->Unload();
        delete mParticleShader;
    }

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
    
    // Three-Point Lighting Setup
    // Luzes fixas no mundo (não seguem a câmera)
    
    // Key Light: Acima da arena (como sol)
    Vector3 keyLightPos = Vector3(0.0f, -1000.0f, 3000.0f);
    mMeshShader->SetVectorUniform("uKeyLightPos", keyLightPos);
    
    // Fill Light: Lateral direita e alto
    Vector3 fillLightPos = Vector3(-1500.0f, -500.0f, 2500.0f);
    mMeshShader->SetVectorUniform("uFillLightPos", fillLightPos);
    
    // Rim Light: Atras esquerda e alto
    Vector3 rimLightPos = Vector3(1500.0f, 500.0f, 2800.0f);
    mMeshShader->SetVectorUniform("uRimLightPos", rimLightPos);
    
    // Posição da câmera para efeito especular metálico
    mMeshShader->SetVectorUniform("uCameraPos", mCameraPos);
    
    // Intensidade das luzes do mundo
    mMeshShader->SetFloatUniform("uWorldLightIntensity", mWorldLightIntensity);
    
    mMeshShader->SetIntUniform("uCelLevels", 3);  // 3 faixas de luz
    
    // Luzes dinamicas
    int numLights = Math::Min(static_cast<int>(mPointLights.size()), 8);
    mMeshShader->SetIntUniform("uNumPointLights", numLights);
    
    for (int i = 0; i < numLights; i++)
    {
        auto light = mPointLights[i];
        if (light->IsEnabled())
        {
            std::string posName = "uPointLightPositions[" + std::to_string(i) + "]";
            std::string colorName = "uPointLightColors[" + std::to_string(i) + "]";
            std::string intensityName = "uPointLightIntensities[" + std::to_string(i) + "]";
            std::string radiusName = "uPointLightRadii[" + std::to_string(i) + "]";
            
            mMeshShader->SetVectorUniform(posName.c_str(), light->GetPosition());
            mMeshShader->SetVectorUniform(colorName.c_str(), light->GetColor());
            mMeshShader->SetFloatUniform(intensityName.c_str(), light->GetIntensity());
            mMeshShader->SetFloatUniform(radiusName.c_str(), light->GetRadius());
        }
    }

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

    // Carrega shader de partículas
    mParticleShader = new Shader();
    if (!mParticleShader->Load("../Shaders/Particle"))
    {
        SDL_Log("Warning: Failed to load Particle shader");
        mParticleShader = nullptr;
    }

    return true;
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