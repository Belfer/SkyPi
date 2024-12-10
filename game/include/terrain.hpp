#pragma once

#include <engine/string.hpp>
#include <engine/array.hpp>
#include <engine/list.hpp>
#include <engine/graphics.hpp>
#include <engine/math.hpp>
#include <engine/file.hpp>

#include <framework/image.hpp>
#include <framework/camera.hpp>

struct TerrainDrawData
{
    Vec4 color{ 1, 1, 1, 1 };
    Vec3 lightDir{ 0, -1, 0 };
    f32 lightI{ 1 };
};

class Terrain
{
public:
	Terrain();
	~Terrain();

    void Initialize();
    void Shutdown();

	void Import(StringView srcPath, StringView dstPath);

    void OpenStream(StringView heightmapPath);
    void CloseStream();

    void Update(const Vec3& position);
    void Render(const Camera& camera);

    inline void SetMaxCells(u32 maxCells) { m_maxCells = maxCells; }
    inline u32 GetMaxCells() const { return m_maxCells; }

    inline void SetViewDistance(f32 viewDistance) { m_viewDistance = viewDistance; }
    inline f32 GetViewDistance() const { return m_viewDistance; }

    inline GraphicsHandle GetResources() const { return m_resources; }

private:
    struct Vertex
    {
        Vec3 position;
        Vec3 normal;
        Vec3 tangent;
    };

    struct Cell
    {
        static constexpr u32 Length = 129; // A cell is 128+1 x 128+1 pixels
        static constexpr u32 ByteSize = sizeof(u16) * Length * Length;
        using Buffer = Array<u16, ByteSize>;

        // Cell index
        u32 x{ 0 };
        u32 y{ 0 };

        // Cell AABB
        Box3 aabb{};

        // Height data (16bit depth)
        Buffer buffer{};

        // Graphics handles
        GraphicsHandle vertexBuffer{ INVALID_GRAPHICS_HANDLE };
        GraphicsHandle indexBuffer{ INVALID_GRAPHICS_HANDLE };
        u32 indexCount{ 0 };
    };

    void ReadCell(u32 x, u32 y, Cell& cell);

private:
    template <typename T>
	friend class Inspector;

    InputFileStream m_fileStream{};

    TerrainDrawData m_drawData{};
    GraphicsHandle m_drawBuffer{ INVALID_GRAPHICS_HANDLE };

    GraphicsHandle m_vertexShader{ INVALID_GRAPHICS_HANDLE };
    GraphicsHandle m_pixelShader{ INVALID_GRAPHICS_HANDLE };
    GraphicsHandle m_pipeline{ INVALID_GRAPHICS_HANDLE };

    GraphicsHandle m_resources{ INVALID_GRAPHICS_HANDLE };

    GraphicsHandle m_texture{ INVALID_GRAPHICS_HANDLE };

    u32 m_maxCells{ 100 };
    f32 m_viewDistance{ 1000.f };
    List<Cell> m_cells;

    Frustrum m_prevFrustrum;
};