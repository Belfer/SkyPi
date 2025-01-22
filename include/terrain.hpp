#pragma once

#include <engine/string.hpp>
#include <engine/array.hpp>
#include <engine/list.hpp>
#include <engine/graphics.hpp>
#include <engine/math.hpp>
#include <engine/file.hpp>

#include <framework/image.hpp>
#include <framework/camera.hpp>

#include <pga3d.hpp>

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

    void Update(const Camera& camera);
    void Render(const Camera& camera);

    inline void SetMaxCells(u32 maxCells) { m_maxCells = maxCells; }
    inline u32 GetMaxCells() const { return m_maxCells; }

    inline void SetViewDistance(f32 viewDistance) { m_viewDistance = viewDistance; }
    inline f32 GetViewDistance() const { return m_viewDistance; }

    inline GraphicsHandle GetResources() const { return m_resources; }

public:
    struct Vertex
    {
        Vec3 position;
        Vec3 normal;
        Vec3 tangent;
    };

    struct MetaCell
    {
        u32 x{ 0 };
        u32 y{ 0 };
        u32 h{ 0 };
        bool isLoaded{ false };
    };

    struct Cell
    {
        static constexpr u32 Length = 128 + 1;
        using HeightData = Array<u16, (Length + 2) * (Length + 2)>;
        using VertexArray = Array<Vertex, Length * Length>;

        u32 idx{ 0 };
        Box3 aabb{};
        Vec3 center{};
        VertexArray vertices{};
        GraphicsHandle vertexBuffer{ INVALID_GRAPHICS_HANDLE };

        i32 lod{ 0 };
    };

    void ReadCell(u32 x, u32 y, Cell& cell);

private:
    template <typename T>
	friend class Editor;

    InputFileStream m_fileStream{};

    TerrainDrawData m_drawData{};
    GraphicsHandle m_drawBuffer{ INVALID_GRAPHICS_HANDLE };

    GraphicsHandle m_vertexShader{ INVALID_GRAPHICS_HANDLE };
    GraphicsHandle m_pixelShader{ INVALID_GRAPHICS_HANDLE };
    GraphicsHandle m_pipeline{ INVALID_GRAPHICS_HANDLE };

    GraphicsHandle m_resources{ INVALID_GRAPHICS_HANDLE };

    GraphicsHandle m_texture{ INVALID_GRAPHICS_HANDLE };

    GraphicsHandle m_vertexBuffer{ INVALID_GRAPHICS_HANDLE };

    struct IndexBuffer
    {
        GraphicsHandle buffer{ INVALID_GRAPHICS_HANDLE };
        u32 count{ 0 };
    };
    IndexBuffer m_indexBuffers[8]{};

    i32 m_lod{ -1 };
    
    i32 m_cellsX{ 0 };
    i32 m_cellsY{ 0 };
    u32 m_maxCells{ 33 };
    f32 m_viewDistance{ 1000.f };

    List<MetaCell> m_metaCells{};
    List<Cell> m_cells{};

    bool m_debugDraw{ false };
    bool m_updateFrustum{ true };
    bool m_updateCamera{ true };
    Frustrum m_frustum{};
    Vec3 m_cameraPos{};
};