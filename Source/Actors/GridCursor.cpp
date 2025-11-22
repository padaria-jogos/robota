//
// Created by Heitor S. on 11/22/2025.
//

#include "GridCursor.h"

#include "Game.h"
#include "Components/Drawing/MeshComponent.h"


GridCursor::GridCursor(Game* game)
        : Actor(game)
        , mGridX(0)
        , mGridY(0)
        , mAnimTimer(0.0f)
        , mBaseHeight(500.0f)
{
        MeshComponent* mc = new MeshComponent(this);
        Mesh* mesh = game->GetRenderer()->GetMesh("../Assets/Cursor.gpmesh");
        mc->SetMesh(mesh);

        SetScale(Vector3(100.0f, 100.0f, 100.0f));
        Move(0, 0);
        SetRotation(Vector3(Math::ToRadians(180.0f), 0.0f, 0.0f));
}

void GridCursor::Move(int xOffset, int yOffset)
{
        // Calcular nova posição
        int newX = mGridX + xOffset;
        int newY = mGridY + yOffset;

        // Obter o GridMap para checar limites
        GridMap* grid = GetGame()->GetGrid();
        if (!grid) {
                SDL_Log("ERRO: GridCursor tentou mover mas GetGrid() retornou NULL!");
                return;
        }

        if (!grid) return;

        // Verificar se saiu do mapa (Clamp)
        if (newX < 0) newX = 0;
        if (newX >= grid->GetCols()) newX = grid->GetCols() - 1;
        if (newY < 0) newY = 0;
        if (newY >= grid->GetRows()) newY = grid->GetRows() - 1;

        mGridX = newX;
        mGridY = newY;

        // Atualizar Visual
        Vector3 worldPos = grid->GetWorldPosition(mGridX, mGridY);
        worldPos.z = mBaseHeight;
        SetPosition(worldPos);

        // Highlight
        grid->SetSelectedTile(mGridX, mGridY);
}

void GridCursor::OnUpdate(float deltaTime)
{
        // Animação de Flutuar
        mAnimTimer += deltaTime;
        Vector3 pos = GetPosition();


        float bobbing = std::sin(mAnimTimer * 5.0f) * 20.0f;

        pos.z = mBaseHeight + bobbing;

        SetPosition(pos);
}

void GridCursor::OnProcessInput(const uint8_t* state)
{
        // Game.cpp para evitar movimento rápido demais.
}

void GridCursor::OnKeyDown(int key)
{
        switch (key)
        {
                case SDLK_w:
                case SDLK_UP:
                        Move(0, 1);
                        break;

                case SDLK_s:
                case SDLK_DOWN:
                        Move(0, -1);
                        break;

                case SDLK_a:
                case SDLK_LEFT:
                        Move(1, 0);
                        break;

                case SDLK_d:
                case SDLK_RIGHT:
                        Move(-1, 0);
                        break;

                case SDLK_SPACE:
                case SDLK_RETURN:
                        SDL_Log("Selecionou Tile: %d, %d", mGridX, mGridY);
                        // TODO: mGame->GetTurnResolver()->SelectUnit(mGridX, mGridY);
                        break;
        }
}
