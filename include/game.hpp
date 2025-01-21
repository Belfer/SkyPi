#pragma once

#include <engine/application.hpp>
#include <engine/graphics.hpp>
#include <engine/math.hpp>

#include <terrain.hpp>

struct ConstantData
{
    Mat4 viewProjMtx{};
};

class SkyPiGame final : public Game
{
public:
    SkyPiGame();

    void Configure() override;
    bool Initialize() override;
    void Update() override;
    void Render() override;
    void Shutdown() override;
    
private:
    friend class SkyPiEditor;

    // Global data
    ConstantData m_constantData{};
    GraphicsHandle m_constantBuffer = INVALID_GRAPHICS_HANDLE;

    // Camera data
    Vec2 m_mousePos{ 0, 0 };
    Vec3 m_cameraPos{ 0, 5, 0 };
    Vec3 m_cameraRot{ 0, 0, 0 };
    f32 m_lookSpeed = 0.1f;
    f32 m_moveSpeed = 10.0f;
    Camera m_camera;

    // Terrain data
    Terrain m_terrain;
};