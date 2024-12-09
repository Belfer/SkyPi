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
    }
    else if (stbi_is_hdr(filepath))
    {
        FAIL("What to do with HDR?");
    }
    else
    {
        image.data = stbi_load(filepath, &image.width, &image.height, &image.channels, 0);
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
    /*
    // Create terrain buffers
    {
        BufferInfo bufferInfo;
        BufferData bufferData;

        // Vertex buffer
        bufferInfo.type = BufferType::VERTEX_BUFFER;
        bufferInfo.usage = BufferUsage::DYNAMIC;
        bufferInfo.access = BufferAccess::WRITE;
        bufferInfo.strideBytes = sizeof(VertexData);

        static VertexData verts[] =
        {
            VertexData{ Vec3{-128, 0, -128}, Vec3{0, 1, 0}, Vec3{1, 0, 0} },
            VertexData{ Vec3{ 128, 0, -128}, Vec3{0, 1, 0}, Vec3{1, 0, 0} },
            VertexData{ Vec3{-128, 0,  128}, Vec3{0, 1, 0}, Vec3{1, 0, 0} },
            VertexData{ Vec3{ 128, 0,  128}, Vec3{0, 1, 0}, Vec3{1, 0, 0} }
        };
        bufferData.dataSize = sizeof(verts);
        bufferData.pData = verts;

        m_vertexBuffer = Graphics::Get().CreateBuffer(bufferInfo, bufferData);

        // Index buffer
        bufferInfo.type = BufferType::INDEX_BUFFER;
        bufferInfo.usage = BufferUsage::DYNAMIC;
        bufferInfo.access = BufferAccess::WRITE;
        bufferInfo.strideBytes = 0;

        static u32 indices[] = { 0, 1, 2, 3 };
        bufferData.dataSize = sizeof(indices);
        bufferData.pData = indices;

        m_indexBuffer = Graphics::Get().CreateBuffer(bufferInfo, bufferData);
        m_indexCount = ARRAYSIZE(indices);
    }*/
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
    UnloadImage(m_heightmap);
}

void Terrain::Import(StringView srcPath, StringView dstPath)
{
    Image heightmap = LoadImage(srcPath);
    ENSURE(heightmap.is_16bit);

    OutputFileStream outFile(File::GetPath(dstPath), std::ios::binary);

    const i32 width = heightmap.width;
    const i32 height = heightmap.height;
    const u8* data = heightmap.data;

    const u32 cellSize = Terrain::Cell::Length;
    i32 cellsX = (width + cellSize - 1) / cellSize;
    i32 cellsY = (height + cellSize - 1) / cellSize;

    // Write header
    outFile.write((char*)&width, sizeof(i32));
    outFile.write((char*)&height, sizeof(i32));

    // Write cells
    static Terrain::Cell::Buffer g_cellBuffer;
    for (i32 y = 0; y < cellsY; ++y)
    {
        for (i32 x = 0; x < cellsX; ++x)
        {
            for (i32 cy = 0; cy < cellSize; ++cy)
            {
                for (i32 cx = 0; cx < cellSize; ++cx)
                {
                    i32 globalX = x * cellSize + cx;
                    i32 globalY = y * cellSize + cy;
                    if (globalX < width && globalY < height)
                    {
                        g_cellBuffer[cy * cellSize + cx] = data[globalY * width + globalX];
                    }
                    else
                    {
                        g_cellBuffer[cy * cellSize + cx] = 0; // Fill with zeros if out of bounds
                    }
                }
            }

            // Write size and data
            const u32 cellByteSize = Terrain::Cell::ByteSize;
            outFile.write((char*)&cellByteSize, sizeof(u32));
            outFile.write((char*)g_cellBuffer.data(), cellByteSize);
        }
    }

    outFile.close();
    UnloadImage(heightmap);
}

void Terrain::OpenStream(StringView heightmapPath)
{
    CString<512> filepath = File::GetPath(heightmapPath);
    m_fileStream.open(filepath, std::ios::binary);
}

void Terrain::CloseStream()
{
    m_fileStream.close();
    m_cells.clear();
}

Terrain::Cell Terrain::ReadCell(u32 x, u32 y)
{
    ENSURE(m_fileStream.is_open());
    static Terrain::Cell::Buffer g_cellBuffer;

    m_fileStream.seekg(0);

    /*
    // Load terrain heightmap
    {
        // Full res is 160x128 cells each cell is 128 pixels
        // The full res also adds an extra padding row/col at the end so the total res is: 20480+1 x 16384+1
        // The low res is just a subsection of the whole thing, and is 32x32 cells and it also includes the padding.
        m_heightmap = LoadImage(heightmap, true);
    }

    // Update terrain buffers
    {
        BufferData bufferData;

        i32 w = m_heightmap.width;
        i32 h = m_heightmap.height;
        i32 c = m_heightmap.channels;
        auto d = m_heightmap.data;

        // Vertex buffer
        f32 yScaleNorm = 1.f / 0xFFFF;
        f32 yScale = 500.f;
        f32 yShift = 0;
        List<VertexData> vertices;
        vertices.reserve(w * h);
        for (i32 i = 0; i < h; i++)
        {
            for (i32 j = 0; j < w; j++)
            {
                u16* pixelOffset = reinterpret_cast<u16*>(d + (j + w * i) * c);
                u16 y = *pixelOffset;

                // Calculate position
                f32 vx = -h / 2.0f + h * i / (f32)h;
                f32 vy = y * yScaleNorm * yScale - yShift;
                f32 vz = -w / 2.0f + w * j / (f32)w;

                vertices.emplace_back(VertexData{ Vec3{vx, vy, vz}, Vec3{}, Vec3{} });
            }
        }

        for (i32 i = 1; i < h - 1; i++)
        {
            for (i32 j = 1; j < w - 1; j++)
            {
                // Calculate normals and tangents
                Vec3 center = vertices[i * w + j].position;
                Vec3 left = vertices[i * w + (j - 1)].position;
                Vec3 right = vertices[i * w + (j + 1)].position;
                Vec3 down = vertices[(i - 1) * w + j].position;
                Vec3 up = vertices[(i + 1) * w + j].position;

                vertices[i * w + j].normal = Vec3::Cross(up - center, right - center).Normalized();
                vertices[i * w + j].tangent = (right - center).Normalized();
            }
        }

        bufferData.dataSize = (u32)(vertices.size() * sizeof(VertexData));
        bufferData.pData = vertices.data();

        Graphics::Get().UpdateBuffer(m_vertexBuffer, bufferData);

        // Index buffer
        std::vector<u32> indices;
        indices.reserve((h - 1) * (2 * w) + 2 * (h - 2));
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

        Graphics::Get().UpdateBuffer(m_indexBuffer, bufferData);
        m_indexCount = indices.size();
    }*/

    return Terrain::Cell{};
}

void Terrain::Update(const Vec3& position)
{
    //if (!m_fileStream.is_open())
    //    return;
}

void Terrain::Render(const Camera& camera)
{
    //if (!m_fileStream.is_open())
    //    return;

    BufferData bufferData;
    bufferData.dataSize = sizeof(TerrainDrawData);
    bufferData.pData = &m_drawData;
    Graphics::Get().UpdateBuffer(m_drawBuffer, bufferData);

    Graphics::Get().SetPipeline(m_pipeline);
    Graphics::Get().CommitResources(m_pipeline, m_resources);

    for (const auto& cell : m_cells)
    {
        if (cell.vertexBuffer == INVALID_GRAPHICS_HANDLE ||
            cell.indexBuffer == INVALID_GRAPHICS_HANDLE)
            continue;

        if (!Shape::Overlaps(camera.GetFrustrum(), cell.aabb))
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