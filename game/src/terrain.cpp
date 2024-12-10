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

        pipeInfo.topology = PipelineTopology::TRIANGLES;
        pipeInfo.faceCull = PipelineFaceCull::CW;
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

    for (const auto& cell : m_cells)
    {
        if (cell.vertexBuffer != INVALID_GRAPHICS_HANDLE)
            Graphics::Get().DestroyBuffer(cell.vertexBuffer);
        if (cell.indexBuffer != INVALID_GRAPHICS_HANDLE)
        Graphics::Get().DestroyBuffer(cell.indexBuffer);
    }

    if (m_texture != INVALID_GRAPHICS_HANDLE)
        Graphics::Get().DestroyTexture(m_texture);
}

void Terrain::Import(StringView srcPath, StringView dstPath)
{
    // Full resolution: 20480+1 x 16384+1 pixels
    // Each cell: 128+1 x 128+1 pixels
    // Low resolution: 32x32 cells including padding
    Image heightmap = LoadImage(srcPath);
    ENSURE(heightmap.is_16bit);

    OutputFileStream outFile(File::GetPath(dstPath), std::ios::binary);

    const i32 width = heightmap.width;
    const i32 height = heightmap.height;
    const u16* data = reinterpret_cast<const u16*>(heightmap.data);

    const u32 cellSize = Cell::Length; // Each cell is 128+1
    const i32 cellsX = (width + cellSize - 1) / cellSize;  // Number of cells in X direction
    const i32 cellsY = (height + cellSize - 1) / cellSize; // Number of cells in Y direction

    // Write header
    outFile.seekp(0);
    outFile.write((char*)&width, sizeof(i32));
    outFile.write((char*)&height, sizeof(i32));

    // Write cells
    static Cell::Buffer cellBuffer;
    for (u32 y = 0; y < cellsY; ++y)
    {
        for (u32 x = 0; x < cellsX; ++x)
        {
            // Fill the cell buffer, including the padding
            for (i32 cy = 0; cy < cellSize; ++cy)
            {
                for (i32 cx = 0; cx < cellSize; ++cx)
                {
                    i32 globalX = x * (cellSize - 1) + cx;  // Adjusted for 128 data points
                    i32 globalY = y * (cellSize - 1) + cy;  // Adjusted for 128 data points

                    if (globalX < width && globalY < height)
                    {
                        // Fill the buffer with the actual data for this cell position
                        cellBuffer[cy * cellSize + cx] = data[globalY * width + globalX];
                    }
                    else
                    {
                        // Handle the padding by copying values from adjacent cells if out of bounds
                        if (x > 0 && cx == 0)  // If we're on the leftmost column, copy from the previous cell
                        {
                            cellBuffer[cy * cellSize + cx] = cellBuffer[cy * cellSize + 1];  // Copy from the right column
                        }
                        else if (y > 0 && cy == 0)  // If we're on the topmost row, copy from the previous cell
                        {
                            cellBuffer[cy * cellSize + cx] = cellBuffer[(cy + 1) * cellSize + cx];  // Copy from the bottom row
                        }
                        else
                        {
                            // Default behavior: use 0 for the padding outside the valid range
                            cellBuffer[cy * cellSize + cx] = 0;
                        }
                    }
                }
            }

            // Ensure the padding between adjacent cells is consistent
            if (x > 0)
            {
                for (i32 cy = 0; cy < cellSize; ++cy)
                {
                    // Copy the padding column from the previous cell
                    cellBuffer[cy * cellSize] = cellBuffer[cy * cellSize + 1];
                }
            }

            if (y > 0)
            {
                for (i32 cx = 0; cx < cellSize; ++cx)
                {
                    // Copy the padding row from the previous cell
                    cellBuffer[cx] = cellBuffer[cellSize + cx];
                }
            }

            // Write cell coordinates and data
            outFile.write((char*)&x, sizeof(u32));
            outFile.write((char*)&y, sizeof(u32));
            outFile.write((char*)cellBuffer.data(), Cell::ByteSize);
        }
    }

    outFile.close();
    UnloadImage(heightmap);
}

void Terrain::OpenStream(StringView heightmapPath)
{
    CString<512> filepath = File::GetPath(heightmapPath);
    m_fileStream.open(filepath, std::ios::binary);

    m_cells.resize(m_maxCells);
    for (u32 i = 0; i < 5; i++)
        for (u32 j = 0; j < 5; j++)
            ReadCell(i, j, m_cells[i * 5 + j]);
}

void Terrain::CloseStream()
{
    m_fileStream.close();
    m_cells.clear();
}

void Terrain::ReadCell(u32 x, u32 y, Terrain::Cell& cell)
{
    ENSURE(m_fileStream.is_open());

    // Read header
    i32 width = 0, height = 0;
    m_fileStream.seekg(0);
    m_fileStream.read((char*)&width, sizeof(i32));
    m_fileStream.read((char*)&height, sizeof(i32));

    const u32 cellSize = Cell::Length;
    const i32 cellsX = (width + cellSize - 1) / cellSize;
    const i32 cellsY = (height + cellSize - 1) / cellSize;

    ENSURE(x < cellsX && y < cellsY);

    const u32 headerSize = sizeof(i32) * 2;
    const u32 cellDataSize = sizeof(u32) * 2 + Cell::ByteSize;
    const u32 cellIndex = y * cellsX + x;
    const u32 offset = headerSize + (cellIndex * cellDataSize);

    // Read cell data
    m_fileStream.seekg(offset);

    u32 cellX = 0, cellY = 0;
    m_fileStream.read((char*)&cellX, sizeof(u32));
    m_fileStream.read((char*)&cellY, sizeof(u32));

    ENSURE(cellX == x && cellY == y); // Validate cell coordinates
    m_fileStream.read((char*)cell.buffer.data(), Cell::ByteSize);
    
    // Update terrain buffers
    BufferData bufferData;

    const i32 w = cellSize;
    const i32 h = cellSize;
    const u16* d = cell.buffer.data();

    // Vertex buffer
    const f32 yScaleNorm = 1.f / 0xFFFF;
    const f32 yScale = 500.f;
    const f32 yShift = 0;
    const f32 cellWorldOffsetX = x * (cellSize - 1);
    const f32 cellWorldOffsetZ = y * (cellSize - 1);

    cell.aabb.min = Vec3{ FLT_MAX, FLT_MAX, FLT_MAX };
    cell.aabb.max = Vec3{ -FLT_MAX, -FLT_MAX, -FLT_MAX };

    List<Vertex> vertices;
    vertices.reserve(w * h);
    for (i32 i = 0; i < h; i++)
    {
        for (i32 j = 0; j < w; j++)
        {
            u16 heightValue = d[j + w * i];
            Vec3 pos
            {
                cellWorldOffsetX + j,
                heightValue* yScaleNorm* yScale - yShift,
                cellWorldOffsetZ + i
            };

            cell.aabb.min = Vec3::Min(cell.aabb.min, pos);
            cell.aabb.max = Vec3::Max(cell.aabb.max, pos);

            vertices.emplace_back(Vertex{ pos, Vec3{}, Vec3{} });
        }
    }

    // Calculate normals and tangents
    for (i32 i = 1; i < h - 1; i++)
    {
        for (i32 j = 1; j < w - 1; j++)
        {
            if (i > 0 && i < h - 1 && j > 0 && j < w - 1)
            {
                Vec3 center = vertices[i * w + j].position;
                Vec3 left = vertices[i * w + (j - 1)].position;
                Vec3 right = vertices[i * w + (j + 1)].position;
                Vec3 down = vertices[(i + 1) * w + j].position;
                Vec3 up = vertices[(i - 1) * w + j].position;

                vertices[i * w + j].normal = Vec3::Cross(down - up, right - left).Normalized();
                vertices[i * w + j].tangent = (right - left).Normalized();
            }
        }
    }

    bufferData.dataSize = (u32)(vertices.size() * sizeof(Vertex));
    bufferData.pData = vertices.data();

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

    // Index buffer
    std::vector<u32> indices;
    indices.reserve((h - 1) * (2 * w) - 2);
    for (i32 i = 0; i < h - 1; i++)
    {
        for (i32 j = 0; j < w; j++)
        {
            indices.push_back(j + w * i);
            indices.push_back(j + w * (i + 1));
        }

        if (i < h - 2)
        {
            indices.push_back((w - 1) + w * (i + 1));
            indices.push_back(w * (i + 1));
        }
    }

    bufferData.dataSize = (u32)(indices.size() * sizeof(u32));
    bufferData.pData = indices.data();

    if (cell.indexBuffer == INVALID_GRAPHICS_HANDLE)
    {
        BufferInfo bufferInfo;
        bufferInfo.type = BufferType::INDEX_BUFFER;
        bufferInfo.usage = BufferUsage::DYNAMIC;
        bufferInfo.access = BufferAccess::WRITE;

        cell.indexBuffer = Graphics::Get().CreateBuffer(bufferInfo, bufferData);
    }
    else
    {
        Graphics::Get().UpdateBuffer(cell.indexBuffer, bufferData);
    }
    cell.indexCount = indices.size();
}

void Terrain::Update(const Vec3& position)
{
    if (!m_fileStream.is_open())
        return;
}

#include <engine/window.hpp>

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

    if (true || Window::Get().GetKey(Key::Z))
        m_prevFrustrum = camera.GetFrustrum();

    for (const auto& cell : m_cells)
    {
        DebugDraw::Get().Box(cell.aabb, 0xFFFFFFFF);

        if (cell.vertexBuffer == INVALID_GRAPHICS_HANDLE ||
            cell.indexBuffer == INVALID_GRAPHICS_HANDLE)
            continue;

        if (!Shape::Overlaps(m_prevFrustrum, cell.aabb))
            continue;

        const u64 offset = 0;
        GraphicsHandle pBuffers[] = { cell.vertexBuffer };

        Graphics::Get().SetVertexBuffers(0, 1, pBuffers, &offset);
        Graphics::Get().SetIndexBuffer(cell.indexBuffer, 0);

        DrawIndexedAttribs attribs;
        attribs.indexType = GraphicsValueType::UINT32;
        attribs.numIndices = cell.indexCount;
        Graphics::Get().DrawIndexed(attribs);
    }
}