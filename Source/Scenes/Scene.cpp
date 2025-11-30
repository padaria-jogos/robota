//
// Created by mateus on 29/11/2025.
//

#include "Scene.h"

Scene::Scene(Game* game) :
    mGame(game)
{

}

Scene::~Scene()
{

}

void Scene::Update(float deltaTime)
{
    // Draw all UI elements
    Draw();
}

void Scene::Draw()
{
    // Default implementation does nothing
}