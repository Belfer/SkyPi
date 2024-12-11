#include <terrain.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <fstream>
#include <sstream>
#include <stdexcept>

static Image LoadImage(StringView filename)
{
    CString<512> filepath = File::GetPath(filename);

    Image image{};
    if (stbi_is_16_bit(filepath))
    {
        image.data = (u8*)stbi_load_16(filepath, &image.width, &image.height, &image.channels, 0);
        image.is_16bit = true;
        image.is_hdr = false;
    }
    else if (stbi_is_hdr(filepath))
    {
        FAIL("What to do with HDR?");
        image.is_16bit = false;
        image.is_hdr = true;
    }
    else
    {
        image.data = stbi_load(filepath, &image.width, &image.height, &image.channels, 0);
        image.is_16bit = false;
        image.is_hdr = false;
    }
    return image;
}

static void SaveImage(StringView filename, const Image& image)
{
    CString<512> filepath = File::GetPath(filename);
}

static void UnloadImage(const Image& image)
{
    stbi_image_free(image.data);
}

Terrain::Terrain()
{
}

Terrain::~Terrain()
{
}

static String LoadText(StringView filename)
{
    CString<512> filepath = GAME_PATH;
    filepath.append(filename.data());

    std::ifstream file(filepath.c_str(), std::ios::in);
    if (!file.is_open())
    {
        throw std::runtime_error(fmt::format("Failed to open file: {}", filepath.c_str()));
    }

    std::ostringstream content;
    content << file.rdbuf();

    file.close();
    return content.str();
}

void Terrain::Initialize()
{
    // Create draw buffer
    {
        BufferInfo bufferInfo;
        bufferInfo.type = BufferType::UNIFORM_BUFFER;
        bufferInfo.usage = BufferUsage::DYNAMIC;
        bufferInfo.access = BufferAccess::WRITE;

        BufferData bufferData;
        bufferData.dataSize = 0;
        bufferData.pData = nullptr;

        m_drawBuffer = Graphics::Get().CreateBuffer(bufferInfo, bufferData);
    }

    // Create terrain shaders
    {
        ShaderInfo shaderInfo;
        String src = LoadText("/assets/terrain.shader");

        shaderInfo.shaderType = ShaderType::VERTEX;
        shaderInfo.source = src.c_str();
        m_vertexShader = Graphics::Get().CreateShader(shaderInfo);

        shaderInfo.shaderType = ShaderType::PIXEL;
        shaderInfo.source = src.c_str();
        m_pixelShader = Graphics::Get().CreateShader(shaderInfo);
    }

    // Create terrain resource bindings & pipeline
    {
        ResourceBindingElement resourceElems[] =
        {
            ResourceBindingElement { ShaderType::VERTEX, "ConstantBuffer", 1, ResourceBindingType::UNIFORM_BUFFER, ResourceBindingAccess::STATIC },
            ResourceBindingElement { ShaderType::VERTEX, "DrawBuffer", 1, ResourceBindingType::UNIFORM_BUFFER, ResourceBindingAccess::STATIC },
            ResourceBindingElement { ShaderType::PIXEL, "Albedo", 1, ResourceBindingType::TEXTURE, ResourceBindingAccess::DYNAMIC }
        };

        ResourceBindingInfo resourceBindingInfo;
        resourceBindingInfo.resources = resourceElems;
        resourceBindingInfo.numResources = ARRAYSIZE(resourceElems);

        m_resources = Graphics::Get().CreateResourceBinding(resourceBindingInfo);
        Graphics::Get().BindResource(m_resources, "DrawBuffer", m_drawBuffer);

        PipelineInfo pipeInfo;
        pipeInfo.numRenderTargets = 1;
        pipeInfo.renderTargetFormats[0] = Graphics::Get().GetColorBufferFormat();
        pipeInfo.depthStencilFormat = Graphics::Get().GetDepthBufferFormat();

        pipeInfo.topology = PipelineTopology::TRIANGLE_STRIP;
        pipeInfo.faceCull = PipelineFaceCull::CCW;
        pipeInfo.depthEnable = true;

        LayoutElement layoutElems[] =
        {
            LayoutElement { 0, 0, 3, GraphicsValueType::FLOAT32, false, 0, 0 },
            LayoutElement { 1, 0, 3, GraphicsValueType::FLOAT32, false, 0, 0 },
            LayoutElement { 2, 0, 3, GraphicsValueType::FLOAT32, false, 0, 0 }
        };

        pipeInfo.layoutElements = layoutElems;
        pipeInfo.numElements = ARRAYSIZE(layoutElems);

        pipeInfo.vertShader = m_vertexShader;
        pipeInfo.pixelShader = m_pixelShader;

        m_pipeline = Graphics::Get().CreatePipeline(pipeInfo);
    }

    // Create terrain texture
    {
        Image image = LoadImage("/assets/midgard-textures/21c.png");

        TextureInfo textureInfo;
        textureInfo.width = image.width;
        textureInfo.height = image.height;
        textureInfo.format = TextureFormat::RGBA8_UNORM;
        textureInfo.flags = TextureFlags::SHADER_RESOURCE;

        BufferData textureData;
        textureData.dataSize = image.width * image.height * image.channels;
        textureData.pData = image.data;

        m_texture = Graphics::Get().CreateTexture(textureInfo, textureData);
        Graphics::Get().BindResource(m_resources, "Albedo", m_texture);

        UnloadImage(image);
    }

    // Index buffer
    {
        constexpr SizeType IndexCount = (Cell::Length - 1) * (Cell::Length * 2);
        static Array<u32, IndexCount> indices;

        u32 iidx = 0;
        for (i32 i = 0; i < Cell::Length - 1; i++)
        {
            // Even rows
            if (i % 2 == 0)
            {
                for (i32 j = 0; j < Cell::Length; j++)
                {
                    indices[iidx++] = j + Cell::Length * i;       // Top vertex
                    indices[iidx++] = j + Cell::Length * (i + 1); // Bottom vertex
                }
            }
            // Odd rows
            else
            {
                for (i32 j = Cell::Length - 1; j >= 0; j--)
                {
                    indices[iidx++] = j + Cell::Length * (i + 1); // Bottom vertex
                    indices[iidx++] = j + Cell::Length * i;       // Top vertex
                }
            }
        }

        BufferData bufferData;
        bufferData.dataSize = sizeof(Array<u32, IndexCount>);
        bufferData.pData = indices.data();

        BufferInfo bufferInfo;
        bufferInfo.type = BufferType::INDEX_BUFFER;
        bufferInfo.usage = BufferUsage::DYNAMIC;
        bufferInfo.access = BufferAccess::WRITE;

        m_indexBuffer = Graphics::Get().CreateBuffer(bufferInfo, bufferData);
        m_indexCount = IndexCount;
    }
}

void Terrain::Shutdown()
{
    if (m_vertexShader != INVALID_GRAPHICS_HANDLE)
        Graphics::Get().DestroyShader(m_vertexShader);
    if (m_pixelShader != INVALID_GRAPHICS_HANDLE)
        Graphics::Get().DestroyShader(m_pixelShader);
    if (m_pipeline != INVALID_GRAPHICS_HANDLE)
        Graphics::Get().DestroyPipeline(m_pipeline);
    if (m_resources != INVALID_GRAPHICS_HANDLE)
        Graphics::Get().DestroyResourceBinding(m_resources);

    //for (const auto& cell : m_cells)
    //{
    //    if (cell.vertexBuffer != INVALID_GRAPHICS_HANDLE)
    //        Graphics::Get().DestroyBuffer(cell.vertexBuffer);
    //    if (cell.indexBuffer != INVALID_GRAPHICS_HANDLE)
    //    Graphics::Get().DestroyBuffer(cell.indexBuffer);
    //}

    if (m_texture != INVALID_GRAPHICS_HANDLE)
        Graphics::Get().DestroyTexture(m_texture);
}

void Terrain::Import(StringView srcPath, StringView dstPath)
{
    Image heightmap = LoadImage(srcPath);
    ENSURE(heightmap.is_16bit);

    OutputFileStream outFile(File::GetPath(dstPath), std::ios::binary);

    const i32 width = heightmap.width;
    const i32 height = heightmap.height;
    const u16* data = reinterpret_cast<const u16*>(heightmap.data);

    const i32 cellSize = Cell::Length;
    const i32 cellsX = (width + cellSize - 1) / cellSize;
    const i32 cellsY = (height + cellSize - 1) / cellSize;

    // Write header
    outFile.seekp(0);
    outFile.write((char*)&cellsX, sizeof(i32));
    outFile.write((char*)&cellsY, sizeof(i32));

    // Write cells
    static Cell::HeightData cellHeightData{};
    for (i32 cy = 0; cy < cellsY; ++cy)
    {
        for (i32 cx = 0; cx < cellsX; ++cx)
        {
            i32 globalX = cx * (cellSize - 1);
            i32 globalY = cy * (cellSize - 1);
            u32 idx = 0;
            for (i32 i = globalY - 1; i < (globalY + cellSize + 1); ++i)
            {
                for (i32 j = globalX - 1; j < (globalX + cellSize + 1); ++j)
                {
                    i32 y = i < 0 ? 0 : i >= height ? height - 1 : i;
                    i32 x = j < 0 ? 0 : j >= width ? width - 1 : j;
                    cellHeightData[idx++] = data[y * width + x];
                }
            }

            outFile.write((char*)&cx, sizeof(i32));
            outFile.write((char*)&cy, sizeof(i32));
            outFile.write((char*)cellHeightData.data(), sizeof(Cell::HeightData));
        }
    }

    outFile.close();
    UnloadImage(heightmap);
}

void Terrain::OpenStream(StringView heightmapPath)
{
    CString<512> filepath = File::GetPath(heightmapPath);
    m_fileStream.open(filepath, std::ios::binary);

    i32 cellsX = 0, cellsY = 0;
    m_fileStream.seekg(0);
    m_fileStream.read((char*)&cellsX, sizeof(i32));
    m_fileStream.read((char*)&cellsY, sizeof(i32));

    m_cells.resize(cellsX * cellsY);
    for (u32 i = 0; i < cellsY; i++)
        for (u32 j = 0; j < cellsY; j++)
            ReadCell(i, j, m_cells[i * cellsX + j]);

    //m_cells.resize(m_maxCells);
    //for (u32 i = 0; i < 5; i++)
    //    for (u32 j = 0; j < 5; j++)
    //        ReadCell(i, j, m_cells[i * 5 + j]);
    
    //ReadCell(2, 3, m_cells[0]);
}

void Terrain::CloseStream()
{
    m_fileStream.close();
    m_cells.clear();
}

void Terrain::ReadCell(u32 cx, u32 cy, Terrain::Cell& cell)
{
    ENSURE(m_fileStream.is_open());

    // Read header
    i32 cellsX = 0, cellsY = 0;
    m_fileStream.seekg(0);
    m_fileStream.read((char*)&cellsX, sizeof(i32));
    m_fileStream.read((char*)&cellsY, sizeof(i32));

    ENSURE(cx < cellsX && cy < cellsY);

    const u32 headerSize = sizeof(i32) * 2;
    const u32 cellDataSize = sizeof(i32) * 2 + sizeof(Cell::HeightData);
    const u32 cellIndex = cy * cellsX + cx;
    const u32 offset = headerSize + (cellIndex * cellDataSize);

    // Read cell data
    m_fileStream.seekg(offset);

    u32 cellX = 0, cellY = 0;
    m_fileStream.read((char*)&cellX, sizeof(u32));
    m_fileStream.read((char*)&cellY, sizeof(u32));

    ENSURE(cellX == cx && cellY == cy); // Validate cell coordinates

    static Cell::HeightData cellHeightData{};
    m_fileStream.read((char*)cellHeightData.data(), sizeof(Cell::HeightData));

    // Update terrain buffers
    BufferData bufferData;

    const i32 w = Cell::Length + 2;
    const i32 h = Cell::Length + 2;
    const u16* d = cellHeightData.data();

    // Vertex buffer
    const f32 yScale = 500.f / 0xFFFF;
    const f32 worldX = cx * (Cell::Length - 1);
    const f32 worldY = cy * (Cell::Length - 1);

    cell.aabb.min = Vec3{ Math::F32Max, Math::F32Max, Math::F32Max };
    cell.aabb.max = Vec3{ -Math::F32Max,-Math::F32Max,-Math::F32Max };

    u32 vidx = 0;
    for (i32 i = 1; i < h - 1; i++)
    {
        for (i32 j = 1; j < w - 1; j++)
        {
            u16 h = d[j + w * i];
            Vec3 pos
            {
                (f32)worldX + j,
                h * yScale,
                (f32)worldY + i
            };

            cell.aabb.min = Vec3::Min(cell.aabb.min, pos);
            cell.aabb.max = Vec3::Max(cell.aabb.max, pos);

            cell.vertices[vidx++].position = pos;
        }
    }

    // Calculate normals and tangents
    for (i32 i = 1; i < h - 1; i++)
    {
        for (i32 j = 1; j < w - 1; j++)
        {
            // Sample height values (left, right, up, down)
            f32 hl = d[(i    ) * w + (j - 1)];
            f32 hr = d[(i    ) * w + (j + 1)];
            f32 hu = d[(i - 1) * w + (j    )];
            f32 hd = d[(i + 1) * w + (j    )];

            Vec3 tangentX{ 1.0f, (hr - hl) * yScale, 0.0f };
            Vec3 tangentZ{ 0.0f, (hd - hu) * yScale, 1.0f };

            auto& v = cell.vertices[(i - 1) * Cell::Length + (j - 1)];
            v.normal = Vec3::Cross(tangentZ, tangentX).Normalized();
            v.tangent = tangentX.Normalized();
        }
    }

    bufferData.dataSize = sizeof(Cell::VertexArray);
    bufferData.pData = cell.vertices.data();

    if (cell.vertexBuffer == INVALID_GRAPHICS_HANDLE)
    {
        BufferInfo bufferInfo;
        bufferInfo.type = BufferType::VERTEX_BUFFER;
        bufferInfo.usage = BufferUsage::DYNAMIC;
        bufferInfo.access = BufferAccess::WRITE;
        bufferInfo.strideBytes = sizeof(Vertex);

        cell.vertexBuffer = Graphics::Get().CreateBuffer(bufferInfo, bufferData);
    }
    else
    {
        Graphics::Get().UpdateBuffer(cell.vertexBuffer, bufferData);
    }
}

void Terrain::Update(const Vec3& position)
{
    if (!m_fileStream.is_open())
        return;
}

void Terrain::Render(const Camera& camera)
{
    if (!m_fileStream.is_open())
        return;

    BufferData bufferData;
    bufferData.dataSize = sizeof(TerrainDrawData);
    bufferData.pData = &m_drawData;
    Graphics::Get().UpdateBuffer(m_drawBuffer, bufferData);

    Graphics::Get().SetPipeline(m_pipeline);
    Graphics::Get().CommitResources(m_pipeline, m_resources);

    if (m_updateFrustum)
        m_frustum = camera.GetFrustum();
    if (m_debugDraw) {} // TODO: Draw frustum

    for (const auto& cell : m_cells)
    {
        if (m_debugDraw)
            DebugDraw::Get().Box(cell.aabb, 0xFFFFFFFF);

        if (cell.vertexBuffer == INVALID_GRAPHICS_HANDLE)
            continue;

        //if (m_debugDraw)
        //{
        //    i32 cellSize = Cell::Length;
        //    for (i32 i = 0; i < Cell::Length - 1; i++)
        //    {
        //        for (i32 j = 0; j < Cell::Length - 1; j++)
        //        {
        //            DebugDraw::Get().Line(cell.vertices[(i + 0) * cellSize + (j + 0)].position, cell.vertices[(i + 1) * cellSize + (j + 0)].position, 0xFFFFFFFF);
        //            DebugDraw::Get().Line(cell.vertices[(i + 0) * cellSize + (j + 0)].position, cell.vertices[(i + 0) * cellSize + (j + 1)].position, 0xFFFFFFFF);
        //            DebugDraw::Get().Line(cell.vertices[(i + 1) * cellSize + (j + 1)].position, cell.vertices[(i + 1) * cellSize + (j + 0)].position, 0xFFFFFFFF);
        //            DebugDraw::Get().Line(cell.vertices[(i + 1) * cellSize + (j + 1)].position, cell.vertices[(i + 0) * cellSize + (j + 1)].position, 0xFFFFFFFF);
        //        }
        //    }
        //}

        if (!Shape::Overlaps(m_frustum, cell.aabb))
            continue;

        const u64 offset = 0;
        GraphicsHandle pBuffers[] = { cell.vertexBuffer };

        Graphics::Get().SetVertexBuffers(0, 1, pBuffers, &offset);
        Graphics::Get().SetIndexBuffer(m_indexBuffer, 0);

        DrawIndexedAttribs attribs;
        attribs.indexType = GraphicsValueType::UINT32;
        attribs.numIndices = m_indexCount;
        Graphics::Get().DrawIndexed(attribs);

        //DrawAttribs attribs;
        //attribs.numVertices = (Cell::Length + 1) * (Cell::Length + 1);
        //Graphics::Get().Draw(attribs);
    }
}