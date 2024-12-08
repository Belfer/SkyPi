#pragma once

#include <engine/string.hpp>
#include <engine/graphics.hpp>
#include <engine/math.hpp>
#include <framework/image.hpp>

struct TerrainDrawData
{
    Vec4 color{ 1, 1, 1, 1 };
    Vec3 lightDir{ 0, 1, 0 };
    f32 lightI{ 1 };
};

class Terrain
{
public:
	Terrain();
	~Terrain();

    void Initialize();
    void Shutdown();

	void Generate(StringView heightmap);
	void Render();

    inline GraphicsHandle GetResources() const { return m_resources; }

private:
    template <typename T>
	friend class Inspector;

    TerrainDrawData m_drawData{};
    GraphicsHandle m_drawBuffer = INVALID_GRAPHICS_HANDLE;

    GraphicsHandle m_vertexShader = INVALID_GRAPHICS_HANDLE;
    GraphicsHandle m_pixelShader = INVALID_GRAPHICS_HANDLE;
    GraphicsHandle m_pipeline = INVALID_GRAPHICS_HANDLE;

    GraphicsHandle m_resources = INVALID_GRAPHICS_HANDLE;

    Image m_heightmap{};
    GraphicsHandle m_texture = INVALID_GRAPHICS_HANDLE;

    GraphicsHandle m_vertexBuffer = INVALID_GRAPHICS_HANDLE;
    GraphicsHandle m_indexBuffer = INVALID_GRAPHICS_HANDLE;
    u32 m_indexCount = 0;
};