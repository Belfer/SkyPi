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
    TestAsset() {}
    TestAsset(u32 v) : value(v) {}
    ~TestAsset() { BX_LOGI(Log, "~TestAsset"); }

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
    //auto asset = Asset::FromMemory(Object<TestAsset>::New(64));
    //asset.Save<TestAsset>("[assets]/save_asset.json");

    auto save_asset = Asset::FromFile("[assets]/save_asset.json");
    save_asset.Load<TestAsset>();

    auto load_asset = Asset::FromFile("[assets]/save_asset.json");
    load_asset.GetObject().As<TestAsset>()->value = 8;
    load_asset.Save<TestAsset>("[assets]/load_asset.json");
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