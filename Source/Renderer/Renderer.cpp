#include <GL/glew.h>
#include "Renderer.h"
#include "Shader.h"
#include "../Actors/Actor.h"
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

    // Opacos
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE); // Habilita escrita no Z-Buffer
    glDisable(GL_BLEND);  // Desliga transparência por enquanto (performance e correção)

    // Ativa Shader de Mesh
    mMeshShader->SetActive();

    // Atualiza matrizes
    mMeshShader->SetMatrixUniform("uViewProj", mView * mProjection);

    // Luz
    Vector3 keyLightPos = Vector3(0.0f, -1000.0f, 3000.0f);
    mMeshShader->SetVectorUniform("uKeyLightPos", keyLightPos);
    Vector3 fillLightPos = Vector3(-1500.0f, -500.0f, 2500.0f);
    mMeshShader->SetVectorUniform("uFillLightPos", fillLightPos);
    Vector3 rimLightPos = Vector3(1500.0f, 500.0f, 2800.0f);
    mMeshShader->SetVectorUniform("uRimLightPos", rimLightPos);
    mMeshShader->SetVectorUniform("uCameraPos", mCameraPos);
    mMeshShader->SetFloatUniform("uWorldLightIntensity", mWorldLightIntensity);
    mMeshShader->SetIntUniform("uCelLevels", 3);

    // Luzes dinamicas
    int numLights = Math::Min(static_cast<int>(mPointLights.size()), 8);
    mMeshShader->SetIntUniform("uNumPointLights", numLights);

    for (int i = 0; i < numLights; i++)
    {
        auto light = mPointLights[i];
        if (light->IsEnabled())
        {
            std::string idx = std::to_string(i);
            mMeshShader->SetVectorUniform(("uPointLightPositions[" + idx + "]").c_str(), light->GetPosition());
            mMeshShader->SetVectorUniform(("uPointLightColors[" + idx + "]").c_str(), light->GetColor());
            mMeshShader->SetFloatUniform(("uPointLightIntensities[" + idx + "]").c_str(), light->GetIntensity());
            mMeshShader->SetFloatUniform(("uPointLightRadii[" + idx + "]").c_str(), light->GetRadius());
        }
    }

    // Desenha apenas os OPACOS
    for (auto mc : mMeshComps)
    {
        if (mc->IsVisible() && !mc->GetIsTransparent())
        {
            mc->Draw(mMeshShader);
        }
    }

    // Algoritmo do pintor nos transparentes
    Vector3 camPos = mCameraPos;
    std::sort(mMeshComps.begin(), mMeshComps.end(),
        [camPos](MeshComponent* a, MeshComponent* b) {
            Vector3 posA = a->GetOwner()->GetPosition();
            Vector3 posB = b->GetOwner()->GetPosition();

            float distSqA = (posA - camPos).LengthSq();
            float distSqB = (posB - camPos).LengthSq();

            return distSqA > distSqB;
        }
    );

    // Transparentes

    // Habilita a mistura de cores (Alpha Blending)
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Desligar a escrita no Depth Mask
    // Assim, a parte preta transparente da textura não fura o chão.
    glDepthMask(GL_FALSE);

    // Desenha apenas os transparentes
    for (auto mc : mMeshComps)
    {
        if (mc->IsVisible() && mc->GetIsTransparent())
        {
            mc->Draw(mMeshShader);
        }
    }

    // UI (Sprites 2D)

    // Restaura Depth Mask para o próximo frame
    glDepthMask(GL_TRUE);

    // Desabilita Depth Test para a UI ficar sempre na frente
    glDisable(GL_DEPTH_TEST);

    glEnable(GL_BLEND);
    glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ZERO, GL_ONE);

    mSpriteShader->SetActive();
    mSpriteVerts->SetActive();

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