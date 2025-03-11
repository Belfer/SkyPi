#include <game.hpp>

#include <engine/window.hpp>
#include <engine/time.hpp>
#include <engine/debug.hpp>

#include <framework/world.hpp>

#include <engine/asset.hpp>

SkyPiGame::SkyPiGame()
{
}

struct BX_API TestAsset
{
    TestAsset(u32 v) : value(v) {}
    u32 value{ 32 };
    BX_TYPE(TestAsset)
};

BX_TYPE_REGISTRATION
{
    rttr::registration::class_<TestAsset>("TestAsset")
    .constructor<u32>()(rttr::policy::ctor::as_object)
    .property("value", &TestAsset::value);
}

void SkyPiGame::Configure()
{
    auto obj = Object<TestAsset>::New(64);
    auto asset = Asset::CreateAsset(obj, "[assets]/asset.json");
    asset.Save<TestAsset>();
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