//
// Created by andre on 08/12/2025.
//

#include "ParticleManager.h"
#include "Game.h"
#include "Components/Particles/BasicParticle.h"
#include "Map/GridMap.h"
#include "Random.h"

ParticleManager::ParticleManager(Game* game, int poolSize)
    : mGame(game)
{
    for (int i = 0; i < poolSize; i++)
    {
        auto* particle = new BasicParticle(game);
        mParticlePool.push_back(particle);
    }
}

ParticleManager::~ParticleManager()
{
    for (auto* particle : mParticlePool)
    {
        delete particle;
    }
    mParticlePool.clear();

    SDL_Log("ParticleManager destruído - %zu partículas deletadas", mParticlePool.size());
}

BasicParticle* ParticleManager::GetAvailableParticle()
{
    for (auto* particle : mParticlePool)
    {
        if (particle->IsDead())
        {
            return particle;
        }
    }
    
    SDL_Log("Warning: ParticleManager pool exhausted!");
    return nullptr;
}

void ParticleManager::EmitProjectile(const Vector3& position, const Vector3& direction, 
                                    const ProjectileConfig& config)
{
    BasicParticle* particle = GetAvailableParticle();
    if (!particle) return;
    
    particle->SetColor(config.color);
    particle->SetScale(Vector3(config.scale, config.scale, config.scale));
    particle->SetGravity(config.gravity);
    particle->SetFadeOut(true);
    
    particle->Awake(position, Vector3::Zero, config.lifetime);
    particle->Emit(direction, config.speed);
}

void ParticleManager::CreateExplosion(const Vector3& position, const ExplosionConfig& config)
{
    for (int i = 0; i < config.particleCount; i++)
    {
        BasicParticle* particle = GetAvailableParticle();
        if (!particle) break;
        
        // Direção aleatória em uma esfera
        float theta = Random::GetFloatRange(0.0f, Math::TwoPi);
        float phi = Random::GetFloatRange(0.0f, Math::Pi);
        
        Vector3 direction(
            Math::Sin(phi) * Math::Cos(theta),
            Math::Sin(phi) * Math::Sin(theta),
            Math::Cos(phi)
        );
        
        float speed = Random::GetFloatRange(config.minSpeed, config.maxSpeed);
        
        // Variação na cor
        Vector3 colorVariation(
            Math::Clamp(config.color.x + Random::GetFloatRange(-0.1f, 0.1f), 0.0f, 1.0f),
            Math::Clamp(config.color.y + Random::GetFloatRange(-0.1f, 0.1f), 0.0f, 1.0f),
            Math::Clamp(config.color.z + Random::GetFloatRange(-0.1f, 0.1f), 0.0f, 1.0f)
        );
        
        particle->SetColor(colorVariation);
        particle->SetScale(Vector3(config.particleScale, config.particleScale, config.particleScale));
        particle->SetGravity(config.gravity);
        particle->SetFadeOut(true);
        
        particle->Awake(position, Vector3::Zero, config.lifetime);
        particle->Emit(direction, speed);
    }
}

void ParticleManager::CreateExplosionAtGrid(int gridX, int gridY, GridMap* grid, 
                                           const ExplosionConfig& config)
{
    if (!grid) return;
    
    Vector3 worldPos = grid->GetWorldPosition(gridX, gridY);
    worldPos.z = 100.0f;
    
    CreateExplosion(worldPos, config);
}

void ParticleManager::StartFireAtGrid(int gridX, int gridY, GridMap* grid, 
                                     const FireConfig& config)
{
    if (!grid) return;
    
    // Verifica se já existe fogo neste tile
    for (const auto& fire : mActiveFires)
    {
        if (fire.gridX == gridX && fire.gridY == gridY)
        {
            return;
        }
    }
    
    FireInstance newFire;
    newFire.gridX = gridX;
    newFire.gridY = gridY;
    newFire.worldPosition = grid->GetWorldPosition(gridX, gridY);
    newFire.worldPosition.z = 0.0f;
    newFire.config = config;
    newFire.emissionTimer = 0.0f;
    
    mActiveFires.push_back(newFire);
    
    SDL_Log("Fire started at grid (%d, %d)", gridX, gridY);
}

void ParticleManager::StopFireAtGrid(int gridX, int gridY)
{
    mActiveFires.erase(
        std::remove_if(mActiveFires.begin(), mActiveFires.end(),
            [gridX, gridY](const FireInstance& fire) {
                return fire.gridX == gridX && fire.gridY == gridY;
            }),
        mActiveFires.end()
    );
}

void ParticleManager::StopAllFires()
{
    mActiveFires.clear();
}

void ParticleManager::Update(float deltaTime)
{
    // Debug: mostra quantos efeitos estão ativos
    static float debugTimer = 0.0f;
    debugTimer += deltaTime;
    if (debugTimer >= 2.0f) {
        // SDL_Log("Fogos ativos: %zu | Méis ativos: %zu",
        //         mActiveFires.size(),
        //         mActiveHoneyDrips.size());
        debugTimer = 0.0f;
    }

    // Atualiza o fogo
    for (auto& fire : mActiveFires)
    {
        EmitFireParticles(fire, deltaTime);
    }

    // Atualiza mel
    for (auto& honey : mActiveHoneyDrips)
    {
        EmitHoneyParticles(honey, deltaTime);
    }
}

void ParticleManager::EmitFireParticles(FireInstance& fire, float deltaTime)
{
    fire.emissionTimer += deltaTime;

    float emissionInterval = 1.0f / fire.config.particlesPerSecond;

    // Limita acúmulo excessivo do timer (previne explosão de partículas)
    float maxAccumulation = emissionInterval * 5.0f; // no máximo 5 intervalos acumulados
    if (fire.emissionTimer > maxAccumulation) {
        fire.emissionTimer = maxAccumulation;
    }

    int emittedThisFrame = 0;
    const int maxParticlesPerFrame = 10; // limite de segurança

    while (fire.emissionTimer >= emissionInterval && emittedThisFrame < maxParticlesPerFrame)
    {
        fire.emissionTimer -= emissionInterval;

        BasicParticle* particle = GetAvailableParticle();
        if (!particle) {
            break;
        }

        // Posição com offset aleatório
        Vector3 emitPos = fire.worldPosition;
        emitPos.x += Random::GetFloatRange(-fire.config.spreadRadius, fire.config.spreadRadius);
        emitPos.y += Random::GetFloatRange(-fire.config.spreadRadius, fire.config.spreadRadius);

        // Direção para cima com variação
        Vector3 direction(
            Random::GetFloatRange(-0.8f, 0.5f),
            Random::GetFloatRange(-0.2f, 0.2f),
            Random::GetFloatRange(0.8f, 1.0f)
        );
        direction.Normalize();

        // Cor varia entre base e ponta
        float colorMix = Random::GetFloatRange(0.0f, 1.0f);
        Vector3 particleColor = Vector3::Lerp(fire.config.baseColor, fire.config.tipColor, colorMix);

        float speed = Random::GetFloatRange(fire.config.minSpeed, fire.config.maxSpeed);

        particle->SetColor(particleColor);
        particle->SetScale(Vector3(fire.config.particleScale, fire.config.particleScale, fire.config.particleScale));
        particle->SetGravity(0.0f);
        particle->SetFadeOut(true);

        particle->Awake(emitPos, Vector3::Zero, fire.config.particleLifetime);
        particle->Emit(direction, speed);

        emittedThisFrame++;
    }
}

void ParticleManager::StartHoneyDripAtGrid(int gridX, int gridY, GridMap* grid, 
                                          const HoneyConfig& config)
{
    if (!grid) return;
    
    // Verifica se já existe mel pingando neste tile
    for (const auto& honey : mActiveHoneyDrips)
    {
        if (honey.gridX == gridX && honey.gridY == gridY)
        {
            SDL_Log("Mel já está pingando em (%d, %d)", gridX, gridY);
            return;
        }
    }
    
    HoneyInstance newHoney;
    newHoney.gridX = gridX;
    newHoney.gridY = gridY;
    newHoney.worldPosition = grid->GetWorldPosition(gridX, gridY);
    newHoney.worldPosition.z = 200.0f;  // Alto (no topo do robô)
    newHoney.config = config;
    newHoney.emissionTimer = 0.0f;
    
    mActiveHoneyDrips.push_back(newHoney);
    
    SDL_Log("MEL INICIADO em grid (%d, %d) = mundo (%.1f, %.1f, %.1f) | Total: %zu",
            gridX, gridY,
            newHoney.worldPosition.x,
            newHoney.worldPosition.y,
            newHoney.worldPosition.z,
            mActiveHoneyDrips.size());
}

void ParticleManager::StopHoneyDripAtGrid(int gridX, int gridY)
{
    size_t before = mActiveHoneyDrips.size();
    
    mActiveHoneyDrips.erase(
        std::remove_if(mActiveHoneyDrips.begin(), mActiveHoneyDrips.end(),
            [gridX, gridY](const HoneyInstance& honey) {
                return honey.gridX == gridX && honey.gridY == gridY;
            }),
        mActiveHoneyDrips.end()
    );
    
    size_t after = mActiveHoneyDrips.size();
    if (before > after) {
        SDL_Log("Mel parou de pingar em (%d, %d)", gridX, gridY);
    }
}

void ParticleManager::StopAllHoneyDrips()
{
    if (!mActiveHoneyDrips.empty()) {
        SDL_Log("Parando todos os méis", mActiveHoneyDrips.size());
        mActiveHoneyDrips.clear();
    }
}

void ParticleManager::EmitHoneyParticles(HoneyInstance& honey, float deltaTime)
{
    honey.emissionTimer += deltaTime;
    
    float emissionInterval = 1.0f / honey.config.particlesPerSecond;
    
    // Limita acúmulo excessivo do timer (previne explosão de partículas)
    float maxAccumulation = emissionInterval * 5.0f; // no máximo 5 intervalos acumulados
    if (honey.emissionTimer > maxAccumulation) {
        honey.emissionTimer = maxAccumulation;
    }
    
    int emittedThisFrame = 0;
    const int maxParticlesPerFrame = 10; // limite de segurança
    
    while (honey.emissionTimer >= emissionInterval && emittedThisFrame < maxParticlesPerFrame)
    {
        honey.emissionTimer -= emissionInterval;

        BasicParticle* particle = GetAvailableParticle();
        if (!particle) {
            break;
        }

        float heightOffset = Random::GetFloatRange(0.0f, 150.0f);

        // Posição de origem (topo do robô) com leve variação
        Vector3 emitPos = honey.worldPosition;
        emitPos.x += Random::GetFloatRange(-honey.config.spreadRadius * 0.5f, honey.config.spreadRadius * 0.5f);
        emitPos.y += Random::GetFloatRange(-honey.config.spreadRadius * 0.5f, honey.config.spreadRadius * 0.5f);
        emitPos.z += heightOffset;

        // Direção principal: para baixo com leve variação lateral
        Vector3 direction(
            Random::GetFloatRange(-0.15f, 0.15f),
            Random::GetFloatRange(-0.15f, 0.15f),
            Random::GetFloatRange(-1.0f, -0.7f)
        );
        direction.Normalize();

        // Variação na cor (tons de mel)
        float colorVar = Random::GetFloatRange(-0.05f, 0.05f);
        Vector3 particleColor(
            Math::Clamp(honey.config.color.x + colorVar, 0.8f, 1.0f),
            Math::Clamp(honey.config.color.y + colorVar, 0.7f, 0.9f),
            Math::Clamp(honey.config.color.z + colorVar, 0.1f, 0.3f)
        );

        float speed = Random::GetFloatRange(honey.config.minSpeed, honey.config.maxSpeed);

        // Tamanho variável (gotas grandes e pequenas)
        float sizeVariation = Random::GetFloatRange(0.7f, 1.3f);
        float finalScale = honey.config.particleScale * sizeVariation;

        // Salva a escala base ANTES de aplicar stretch
        Vector3 baseScale(finalScale, finalScale, finalScale * 1.5f);

        particle->SetColor(particleColor);
        particle->SetBaseScale(baseScale);   // Define a escala base
        particle->SetScale(baseScale);       // Define a escala atual
        particle->SetGravity(honey.config.gravity);
        particle->SetViscosity(honey.config.viscosity);
        particle->SetFadeOut(false);
        particle->SetStretchWithVelocity(true);

        particle->Awake(emitPos, Vector3::Zero, honey.config.particleLifetime);
        particle->Emit(direction, speed);

        emittedThisFrame++;
    }
}

void ParticleManager::CreateSlashEffect(const Vector3& position, const Vector3& direction,
                                       const SlashConfig& config)
{
    SDL_Log("⚔️ Criando slash com %d partículas em (%.1f, %.1f, %.1f)", 
            config.particleCount, position.x, position.y, position.z);
    
    // Calcula vetores perpendiculares ao slash
    Vector3 slashDir = Vector3::Normalize(direction);
    
    // Vetor "up" do mundo para calcular perpendicular
    Vector3 worldUp = Vector3::UnitZ;
    Vector3 perpendicular = Vector3::Normalize(Vector3::Cross(slashDir, worldUp));
    
    // Se a direção é muito vertical, usa outro eixo
    if (Vector3::Dot(slashDir, worldUp) > 0.95f) {
        perpendicular = Vector3::UnitX;
    }
    
    float arcAngleRad = Math::ToRadians(config.arcAngle);
    float startAngle = -arcAngleRad / 2.0f;
    float angleStep = arcAngleRad / (config.particleCount - 1);
    
    for (int i = 0; i < config.particleCount; i++)
    {
        BasicParticle* particle = GetAvailableParticle();
        if (!particle) break;
        
        // Ângulo atual no arco
        float angle = startAngle + angleStep * i;
        
        // Rotaciona o vetor perpendicular ao redor da direção do slash
        // Para criar um arco
        float cosAngle = Math::Cos(angle);
        float sinAngle = Math::Sin(angle);
        
        // Posição no arco
        Vector3 arcOffset = perpendicular * cosAngle * config.arcRadius;
        arcOffset += Vector3::Cross(slashDir, perpendicular) * sinAngle * config.arcRadius;
        
        Vector3 particlePos = position + arcOffset;
        
        // Direção da partícula: do centro para fora no arco
        Vector3 particleDir = Vector3::Normalize(arcOffset);
        
        // Variação na cor (brilho)
        float brightness = Random::GetFloatRange(0.8f, 1.0f);
        Vector3 particleColor = config.color * brightness;
        
        // Variação no tamanho
        float sizeVar = Random::GetFloatRange(0.7f, 1.3f);
        float finalScale = config.particleScale * sizeVar;
        
        particle->SetColor(particleColor);
        particle->SetScale(Vector3(finalScale, finalScale, finalScale * 0.5f));  // Achatado
        particle->SetGravity(0.0f);  // Sem gravidade (efeito de energia)
        particle->SetViscosity(0.0f);  // Sem viscosidade
        particle->SetFadeOut(true);  // Desaparece rapidamente
        
        // Velocidade diminui conforme se afasta do centro (efeito de impacto)
        float speedMultiplier = 1.0f - (Math::Abs(angle) / (arcAngleRad / 2.0f)) * 0.5f;
        float finalSpeed = config.speed * speedMultiplier;
        
        particle->Awake(particlePos, Vector3::Zero, config.lifetime);
        particle->Emit(particleDir, finalSpeed);
    }
    
    SDL_Log("⚔️ Slash criado com %d partículas", config.particleCount);
}

void ParticleManager::CreateSlashEffectAtGrid(int gridX, int gridY, GridMap* grid,
                                             const Vector3& direction,
                                             const SlashConfig& config)
{
    if (!grid) return;
    
    Vector3 worldPos = grid->GetWorldPosition(gridX, gridY);
    worldPos.z = 0.0f;  // Altura do robô
    
    CreateSlashEffect(worldPos, direction, config);
}