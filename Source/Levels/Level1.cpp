//
// Created by mateus on 20/11/2025.
//

#include "Level1.h"
#include "Actors/RobotVFXManager.h"
// #include "Map/GridMap.h"

Level1::Level1(Game* game, HUD *hud) : Level(game, hud)
{

}

void Level1::OnUpdate(float deltaTime)
{

}

void Level1::ProcessInput(SDL_Event &event)
{
    // Chama o processamento do Level base PRIMEIRO
    Level::ProcessInput(event);
    
    // Adiciona testes de VFX
    if (event.type == SDL_KEYDOWN)
    {
        // TESTE: Pressione 'P' para spawnar projétil
        if (event.key.keysym.sym == SDLK_p)
        {
            Robot* player = GetPlayerRobot();
            if (player && player->GetVFXManager())
            {
                int cursorX = GetCursor()->GetGridX();
                int cursorY = GetCursor()->GetGridY();
                Vector3 cursorPos = GetGrid()->GetWorldPosition(cursorX, cursorY);
                
                player->GetVFXManager()->SpawnTestParticle(cursorPos);
                SDL_Log("[DEBUG] Test particle spawned at cursor (%d, %d)", cursorX, cursorY);
            }
        }
        
        // TESTE: Pressione 'O' para explosão
        if (event.key.keysym.sym == SDLK_o)
        {
            Robot* player = GetPlayerRobot();
            if (player && player->GetVFXManager())
            {
                int cursorX = GetCursor()->GetGridX();
                int cursorY = GetCursor()->GetGridY();
                Vector3 cursorPos = GetGrid()->GetWorldPosition(cursorX, cursorY);
                
                player->GetVFXManager()->PlayHitEffect(cursorPos);
                SDL_Log("[DEBUG] Explosion spawned at cursor (%d, %d)", cursorX, cursorY);
            }
        }
    }
}

