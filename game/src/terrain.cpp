#include <terrain.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <fstream>
#include <sstream>
#include <stdexcept>

struct VertexData
{
    Vec3 position;
    Vec3 normal;
    Vec3 tangent;
};

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

static Image LoadImage(StringView filename, bool is16bit)
{
    CString<512> filepath = GAME_PATH;
    filepath.append(filename.data());

    Image image{};
    if (is16bit)
    {
        image.data = (u8*)stbi_load_16(filepath, &image.width, &image.height, &image.channels, 0);
        image.channels = 2;
    }
    else
    {
        image.data = stbi_load(filepath, &image.width, &image.height, &image.channels, 0);
    }
    return image;
}

static void UnloadImage(const Image& image)
{
    stbi_image_free(image.data);
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
        Image image = LoadImage("/assets/midgard-textures/21c.png", false);

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
            VertexData{ Vec3{-128, 0, -128}, Vec3{}, Vec3{} },
            VertexData{ Vec3{ 128, 0, -128}, Vec3{}, Vec3{} },
            VertexData{ Vec3{ 128, 0,  128}, Vec3{}, Vec3{} },
            VertexData{ Vec3{-128, 0,  128}, Vec3{}, Vec3{} }
        };
        bufferData.dataSize = sizeof(verts);
        bufferData.pData = verts;

        m_vertexBuffer = Graphics::Get().CreateBuffer(bufferInfo, bufferData);

        // Index buffer
        bufferInfo.type = BufferType::INDEX_BUFFER;
        bufferInfo.usage = BufferUsage::DYNAMIC;
        bufferInfo.access = BufferAccess::WRITE;
        bufferInfo.strideBytes = 0;

        static u32 indices[] = { 0, 1, 2, 2, 1, 3 };
        bufferData.dataSize = sizeof(indices);
        bufferData.pData = indices;

        m_indexBuffer = Graphics::Get().CreateBuffer(bufferInfo, bufferData);
        m_indexCount = ARRAYSIZE(indices);
    }
}

void Terrain::Shutdown()
{
    Graphics::Get().DestroyShader(m_vertexShader);
    Graphics::Get().DestroyShader(m_pixelShader);
    Graphics::Get().DestroyPipeline(m_pipeline);
    Graphics::Get().DestroyResourceBinding(m_resources);

    Graphics::Get().DestroyBuffer(m_vertexBuffer);
    Graphics::Get().DestroyBuffer(m_indexBuffer);

    Graphics::Get().DestroyTexture(m_texture);
    UnloadImage(m_heightmap);
}

void Terrain::Generate(StringView heightmap)
{
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
    }
}

void Terrain::Render()
{
    BufferData bufferData;
    bufferData.dataSize = sizeof(TerrainDrawData);
    bufferData.pData = &m_drawData;
    Graphics::Get().UpdateBuffer(m_drawBuffer, bufferData);

    Graphics::Get().SetPipeline(m_pipeline);
    Graphics::Get().CommitResources(m_pipeline, m_resources);

    const u64 offset = 0;
    GraphicsHandle pBuffers[] = { m_vertexBuffer };

    Graphics::Get().SetVertexBuffers(0, 1, pBuffers, &offset);
    Graphics::Get().SetIndexBuffer(m_indexBuffer, 0);

    DrawIndexedAttribs attribs;
    attribs.indexType = GraphicsValueType::UINT32;
    attribs.numIndices = m_indexCount;
    Graphics::Get().DrawIndexed(attribs);
}