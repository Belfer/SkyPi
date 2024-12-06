#include <engine/application.hpp>
#include <engine/window.hpp>
#include <engine/graphics.hpp>
#include <engine/math.hpp>
#include <engine/time.hpp>
#include <framework/image.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

struct ConstantData
{
    Mat4 viewProjMtx{};
};

struct VertexData
{
    Vec3 position;
    Vec4 color;
    Vec3 normal;
    Vec3 tangent;
    Vec2 texcoord;
};

class SkyPiGame final : public Game
{
public:
    void Configure() override;
    bool Initialize() override;
    void Update() override;
    void Render() override;
    void Shutdown() override;

private:
    // Camera data
    Vec2 m_mousePos{ 0, 0 };
    Vec3 m_cameraPos{ 0, 500, 0 };
    Vec3 m_cameraRot{ 0, 0, 0};
    f32 m_lookSpeed = 0.1f;
    f32 m_moveSpeed = 200.0f;

    // Global data
    ConstantData m_constantData{};
    GraphicsHandle m_constantBuffer = INVALID_GRAPHICS_HANDLE;

    // Terrain data
    GraphicsHandle m_terrainVertexShader = INVALID_GRAPHICS_HANDLE;
    GraphicsHandle m_terrainPixelShader = INVALID_GRAPHICS_HANDLE;
    GraphicsHandle m_terrainPipeline = INVALID_GRAPHICS_HANDLE;

    GraphicsHandle m_terrainResources = INVALID_GRAPHICS_HANDLE;

    Image m_terrainImage{};
    GraphicsHandle m_terrainTexture = INVALID_GRAPHICS_HANDLE;

    GraphicsHandle m_terrainVertexBuffer = INVALID_GRAPHICS_HANDLE;
    GraphicsHandle m_terrainIndexBuffer = INVALID_GRAPHICS_HANDLE;
    u32 m_terrainIndexCount = 0;
};

int main(int argc, char** args)
{
    SkyPiGame game;
	return Application::Get().Run(argc, args, game);
}

static const char* g_terrainShaderSrc = R"(
#ifdef VERTEX
layout (location = 0) in vec3 Position;
layout (location = 1) in vec4 Color;
layout (location = 2) in vec3 Normal;
layout (location = 3) in vec3 Tangent;
layout (location = 4) in vec2 UV;

layout (std140) uniform ConstantBuffer
{
    mat4 ViewProjMtx;
};

out vec3 Frag_Position;
out vec4 Frag_Color;
out vec3 Frag_Normal;
out vec3 Frag_Tangent;
out vec2 Frag_UV;

void main()
{
    vec4 WorldPosition = vec4(Position, 1.0);

    Frag_Position = WorldPosition.xyz;
    Frag_Color = Color;
    Frag_Normal = Normal;
    Frag_Tangent = Tangent;
    Frag_UV = UV;

    gl_Position = ViewProjMtx * WorldPosition;
}
#endif

#ifdef PIXEL
layout (location = 0) out vec4 Out_Color;

in vec3 Frag_Position;
in vec4 Frag_Color;
in vec3 Frag_Normal;
in vec3 Frag_Tangent;
in vec2 Frag_UV;

uniform sampler2D Albedo;

void main()
{
    Out_Color = vec4(vec3(max(0, 0.8 * dot(Frag_Normal, vec3(-0.707, -0.707, 0)))), 1);

    //Out_Color = texture(Albedo, Frag_UV);
    //Out_Color = vec4(vec3(Frag_Position.y * 0.002), 1.0);
    //Out_Color = vec4(Frag_Normal * 0.5 + 0.5, 1);
}
#endif
)";

static Image LoadImage(const char* filename)
{
    Image image{};
    //image.data = stbi_load(filename, &image.width, &image.height, &image.channels, 0);
    image.data = (u8*)stbi_load_16(filename, &image.width, &image.height, &image.channels, 0);
    image.channels = 2;
    return image;
}

static void UnloadImage(const Image& image)
{
    stbi_image_free(image.data);
}

void SkyPiGame::Configure()
{
    // Nothing
}

bool SkyPiGame::Initialize()
{
    // Create global buffer
    {
        BufferInfo bufferInfo;
        bufferInfo.type = BufferType::UNIFORM_BUFFER;
        bufferInfo.usage = BufferUsage::DYNAMIC;
        bufferInfo.access = BufferAccess::WRITE;

        BufferData bufferData;
        bufferData.dataSize = 0;
        bufferData.pData = nullptr;

        m_constantBuffer = Graphics::Get().CreateBuffer(bufferInfo, bufferData);
    }

    // Create terrain vertex shader
    {
        ShaderInfo shaderInfo;
        shaderInfo.shaderType = ShaderType::VERTEX;
        shaderInfo.source = g_terrainShaderSrc;
        m_terrainVertexShader = Graphics::Get().CreateShader(shaderInfo);
    }

    // Create terrain pixel shader
    {
        ShaderInfo shaderInfo;
        shaderInfo.shaderType = ShaderType::PIXEL;
        shaderInfo.source = g_terrainShaderSrc;
        m_terrainPixelShader = Graphics::Get().CreateShader(shaderInfo);
    }

    // Create terrain resource bindings & pipeline
    {
        ResourceBindingElement resourceElems[] =
        {
            ResourceBindingElement { ShaderType::VERTEX, "ConstantBuffer", 1, ResourceBindingType::UNIFORM_BUFFER, ResourceBindingAccess::STATIC },
            ResourceBindingElement { ShaderType::PIXEL, "Albedo", 1, ResourceBindingType::TEXTURE, ResourceBindingAccess::DYNAMIC }
        };

        ResourceBindingInfo resourceBindingInfo;
        resourceBindingInfo.resources = resourceElems;
        resourceBindingInfo.numResources = 2;

        m_terrainResources = Graphics::Get().CreateResourceBinding(resourceBindingInfo);
        Graphics::Get().BindResource(m_terrainResources, "ConstantBuffer", m_constantBuffer);

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
            LayoutElement { 1, 0, 4, GraphicsValueType::FLOAT32, false, 0, 0 },
            LayoutElement { 2, 0, 3, GraphicsValueType::FLOAT32, false, 0, 0 },
            LayoutElement { 3, 0, 3, GraphicsValueType::FLOAT32, false, 0, 0 },
            LayoutElement { 4, 0, 2, GraphicsValueType::FLOAT32, false, 0, 0 }
        };

        pipeInfo.layoutElements = layoutElems;
        pipeInfo.numElements = 5;

        pipeInfo.vertShader = m_terrainVertexShader;
        pipeInfo.pixelShader = m_terrainPixelShader;

        m_terrainPipeline = Graphics::Get().CreatePipeline(pipeInfo);
    }

    // Create terrain texture
    {
        // Full res is 160x128 cells each cell is 128 pixels
        // The full res also adds an extra padding row/col at the end so the total res is: 20480+1 x 16384+1
        // The low res is just a subsection of the whole thing, and is 32x32 cells and it also includes the padding.
        m_terrainImage = LoadImage(GAME_PATH"/assets/tamriel_lowres.png");
    
        //TextureInfo textureInfo;
        //textureInfo.width = m_terrainImage.width;
        //textureInfo.height = m_terrainImage.height;
        //textureInfo.format = TextureFormat::RGBA8_UNORM;
        //textureInfo.flags = TextureFlags::SHADER_RESOURCE;
        //
        //BufferData textureData;
        //textureData.dataSize = m_terrainImage.width * m_terrainImage.height * m_terrainImage.channels;
        //textureData.pData = m_terrainImage.data;
        //
        //m_terrainTexture = Graphics::Get().CreateTexture(textureInfo, textureData);
        //Graphics::Get().BindResource(m_terrainResources, "Albedo", m_terrainTexture);
    }

    // Create terrain buffers
    {
        BufferInfo bufferInfo;
        BufferData bufferData;

        // Vertex buffer
        i32 w = m_terrainImage.width;
        i32 h = m_terrainImage.height;
        i32 c = m_terrainImage.channels;
        auto d = m_terrainImage.data;

        f32 yScaleNorm = 1.f / 0xFFFF;
        f32 yScale = 500.f;
        f32 yShift = 0;
        List<VertexData> vertices;

        // Calculate position and uv
        for (i32 i = 0; i < h; i++)
        {
            for (i32 j = 0; j < w; j++)
            {
                u16* pixelOffset = reinterpret_cast<u16*>(d + (j + w * i) * c);
                u16 y = *pixelOffset;

                // Vertex position
                f32 vx = -h / 2.0f + h * i / (f32)h;
                f32 vy = y * yScaleNorm * yScale - yShift;
                f32 vz = -w / 2.0f + w * j / (f32)w;

                f32 u = j / (f32)(w - 1);
                f32 v = i / (f32)(h - 1);

                vertices.emplace_back(VertexData{ Vec3{vx, vy, vz}, Vec4{ 1.0f, 1.0f, 1.0f, 1.0f }, Vec3{}, Vec3{}, Vec2{u, v} });
            }
        }

        // Calculate normals and tangents
        for (i32 i = 1; i < h - 1; i++)
        {
            for (i32 j = 1; j < w - 1; j++)
            {
                Vec3 center = vertices[i * w + j].position;
                Vec3 left = vertices[i * w + (j - 1)].position;
                Vec3 right = vertices[i * w + (j + 1)].position;
                Vec3 down = vertices[(i - 1) * w + j].position;
                Vec3 up = vertices[(i + 1) * w + j].position;

                vertices[i * w + j].normal = Vec3::Cross(up - center, right - center).Normalized();
                vertices[i * w + j].tangent = (right - center).Normalized();
            }
        }

        // Index buffer
        bufferInfo.type = BufferType::VERTEX_BUFFER;
        bufferInfo.usage = BufferUsage::DYNAMIC;
        bufferInfo.access = BufferAccess::WRITE;
        bufferInfo.strideBytes = sizeof(VertexData);

        bufferData.dataSize = (u32)(vertices.size() * sizeof(VertexData));
        bufferData.pData = vertices.data();

        m_terrainVertexBuffer = Graphics::Get().CreateBuffer(bufferInfo, bufferData);

        std::vector<u32> indices;
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

        bufferInfo.type = BufferType::INDEX_BUFFER;
        bufferInfo.usage = BufferUsage::DYNAMIC;
        bufferInfo.access = BufferAccess::WRITE;
        bufferInfo.strideBytes = 0;

        bufferData.dataSize = (u32)(indices.size() * sizeof(u32));
        bufferData.pData = indices.data();

        m_terrainIndexBuffer = Graphics::Get().CreateBuffer(bufferInfo, bufferData);
        m_terrainIndexCount = indices.size();// (u32)((2 * w * (h - 1)) + (2 * (h - 2)));
    }

    return true;
}

void SkyPiGame::Update()
{
    // Update camera rotation
    Vec2 mousePos{ Window::Get().GetMouseX(), Window::Get().GetMouseY() };
    Vec2 viewDelta = mousePos - m_mousePos;
    m_mousePos = mousePos;

    if (Window::Get().GetMouseButton(MouseButton::MOUSE_BUTTON_RIGHT))
    {
        m_cameraRot.x += viewDelta.y * m_lookSpeed;
        m_cameraRot.y -= viewDelta.x * m_lookSpeed;
    }
    
    // Update camera position
    Vec3 moveDelta{};
    if (Window::Get().GetKey(Key::W))
        moveDelta.z += 1;
    if (Window::Get().GetKey(Key::S))
        moveDelta.z -= 1;
    if (Window::Get().GetKey(Key::A))
        moveDelta.x += 1;
    if (Window::Get().GetKey(Key::D))
        moveDelta.x -= 1;
    if (Window::Get().GetKey(Key::E))
        moveDelta.y += 1;
    if (Window::Get().GetKey(Key::Q))
        moveDelta.y -= 1;

    Quat camRot = Quat::Euler(m_cameraRot.x, m_cameraRot.y, m_cameraRot.z);
    m_cameraPos += camRot * moveDelta * m_moveSpeed * Time::Get().DeltaTime();

    // Update view data
    int w, h;
    Window::Get().GetSize(&w, &h);
    Mat4 camProj = Mat4::Perspective(60.f, h > 0 ? (f32)w/h : 1.f, 0.1f, 5000.f);
    Mat4 camView = Mat4::LookAt(m_cameraPos, m_cameraPos + camRot * Vec3::Forward(), Vec3::Up());
    m_constantData.viewProjMtx = camProj * camView;
}

void SkyPiGame::Render()
{
    static f32 clearColor[] = { .4f, .6f, .9f, 1.f };
    Graphics::Get().ClearRenderTarget(0, clearColor);

    BufferData bufferData;
    bufferData.dataSize = sizeof(ConstantData);
    bufferData.pData = &m_constantData;
    Graphics::Get().UpdateBuffer(m_constantBuffer, bufferData);

    Graphics::Get().SetPipeline(m_terrainPipeline);
    Graphics::Get().CommitResources(m_terrainPipeline, m_terrainResources);

    const u64 offset = 0;
    GraphicsHandle pBuffers[] = { m_terrainVertexBuffer };

    Graphics::Get().SetVertexBuffers(0, 1, pBuffers, offset);
    Graphics::Get().SetIndexBuffer(m_terrainIndexBuffer, 0);

    DrawIndexedAttribs attribs;
    attribs.indexType = GraphicsValueType::UINT32;
    attribs.numIndices = m_terrainIndexCount;
    Graphics::Get().DrawIndexed(attribs);
}

void SkyPiGame::Shutdown()
{
    Graphics::Get().DestroyBuffer(m_constantBuffer);

    Graphics::Get().DestroyShader(m_terrainVertexShader);
    Graphics::Get().DestroyShader(m_terrainPixelShader);
    Graphics::Get().DestroyPipeline(m_terrainPipeline);
    Graphics::Get().DestroyResourceBinding(m_terrainResources);

    Graphics::Get().DestroyBuffer(m_terrainVertexBuffer);
    Graphics::Get().DestroyBuffer(m_terrainIndexBuffer);

    Graphics::Get().DestroyTexture(m_terrainTexture);
    UnloadImage(m_terrainImage);
}