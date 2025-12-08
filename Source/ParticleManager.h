//
// Created by andre on 08/12/2025.
//

#pragma once
#include "Math.h"
#include <vector>

class Game;
class BasicParticle;
class GridMap;

// Configuração para projéteis
struct ProjectileConfig
{
    Vector3 color = Vector3(1.0f, 1.0f, 1.0f);
    float speed = 1000.0f;
    float lifetime = 2.0f;
    float scale = 10.0f;
    float gravity = 0.0f;
};

// Configuração para explosões
struct ExplosionConfig
{
    Vector3 color = Vector3(1.0f, 0.5f, 0.0f);
    int particleCount = 20;
    float minSpeed = 200.0f;
    float maxSpeed = 500.0f;
    float lifetime = 1.0f;
    float particleScale = 15.0f;
    float gravity = -500.0f;
};

// Configuração para fogo
struct FireConfig
{
    Vector3 baseColor = Vector3(1.0f, 0.5f, 0.0f);
    Vector3 tipColor = Vector3(1.0f, 1.0f, 0.0f);
    int particlesPerSecond = 30;
    float minSpeed = 100.0f;
    float maxSpeed = 300.0f;
    float particleLifetime = 0.8f;
    float particleScale = 12.0f;
    float spreadRadius = 50.0f;
};

struct HoneyConfig
{
    Vector3 color = Vector3(1.0f, 0.8f, 0.2f);      // Amarelo mel
    int particlesPerSecond = 15;                                // Menos partículas (mel é denso)
    float minSpeed = 50.0f;                                     // Bem devagar
    float maxSpeed = 120.0f;
    float particleLifetime = 2.0f;                              // Longa duração
    float particleScale = 25.0f;                                // Gotas grandes
    float spreadRadius = 80.0f;
    float gravity = -150.0f;                                    // Gravidade moderada (líquido viscoso)
    float viscosity = 0.95f;                                    // Resistência ao movimento (0.0-1.0)
};

struct SlashConfig
{
    Vector3 color = Vector3(1.0f, 1.0f, 1.0f);      // Branco brilhante
    int particleCount = 30;                                     // Partículas em arco
    float speed = 800.0f;                                       // Rápido
    float lifetime = 0.3f;                                      // Curto (flash rápido)
    float particleScale = 25.0f;
    float arcAngle = 120.0f;                                    // Ângulo do arco (graus)
    float arcRadius = 150.0f;                                   // Distância do centro
};

class ParticleManager
{
public:
    ParticleManager(Game* game, int poolSize = 500);
    ~ParticleManager();

    // Emite um projétil em uma direção
    void EmitProjectile(const Vector3& position, const Vector3& direction,
                       const ProjectileConfig& config = ProjectileConfig());

    // Cria uma explosão em uma posição do mundo 3D
    void CreateExplosion(const Vector3& position, const ExplosionConfig& config = ExplosionConfig());

    // Cria uma explosão em uma posição do grid
    void CreateExplosionAtGrid(int gridX, int gridY, GridMap* grid,
                              const ExplosionConfig& config = ExplosionConfig());

    void CreateSlashEffect(const Vector3& position, const Vector3& direction,
                          const SlashConfig& config = SlashConfig());
    
    // Cria slash em uma posição do grid
    void CreateSlashEffectAtGrid(int gridX, int gridY, GridMap* grid,
                                const Vector3& direction,
                                const SlashConfig& config = SlashConfig());

    // Inicia fogo em um tile do grid
    void StartFireAtGrid(int gridX, int gridY, GridMap* grid,
                        const FireConfig& config = FireConfig());

    // Para o fogo em um tile específico
    void StopFireAtGrid(int gridX, int gridY);

    // Para todos os fogos
    void StopAllFires();

    // Atualiza as partículas de fogo (chamar no update do Level)
    void Update(float deltaTime);

    // Inicia mel pingando de um robô
    void StartHoneyDripAtGrid(int gridX, int gridY, GridMap* grid,
                             const HoneyConfig& config = HoneyConfig());

    // Para o mel em um tile específico
    void StopHoneyDripAtGrid(int gridX, int gridY);

    // Para todos os méis
    void StopAllHoneyDrips();

private:
    struct FireInstance
    {
        int gridX;
        int gridY;
        Vector3 worldPosition;
        FireConfig config;
        float emissionTimer;
    };

    struct HoneyInstance
    {
        int gridX;
        int gridY;
        Vector3 worldPosition;
        HoneyConfig config;
        float emissionTimer;
    };

    Game* mGame;
    std::vector<BasicParticle*> mParticlePool;
    std::vector<FireInstance> mActiveFires;
    std::vector<HoneyInstance> mActiveHoneyDrips;

    BasicParticle* GetAvailableParticle();
    void EmitFireParticles(FireInstance& fire, float deltaTime);
    void EmitHoneyParticles(HoneyInstance& honey, float deltaTime);
};