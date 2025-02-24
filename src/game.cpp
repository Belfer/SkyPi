#include <game.hpp>

#include <engine/window.hpp>
#include <engine/time.hpp>
#include <engine/debug.hpp>

#include <framework/world.hpp>

SkyPiGame::SkyPiGame()
{
}

bool SkyPiGame::CanAddScene()
{
    return true;
}

void SkyPiGame::Configure()
{
}

bool SkyPiGame::Initialize()
{
    //m_gameScene = CreateScene<World>();
    //SetActiveScene(m_gameScene);

    return true;
}

void SkyPiGame::Update()
{
}

void SkyPiGame::Render()
{
}

void SkyPiGame::Shutdown()
{
    //Graphics::Get().DestroyBuffer(m_constantBuffer);
    //m_terrain.Shutdown();
}