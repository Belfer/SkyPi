#include <engine/graphics.hpp>

#if defined(EDITOR_BUILD) || defined(DEBUG_BUILD)
static const char* g_debugShaderSrc = R"(
#ifdef GL_ES
precision mediump float;
#endif

#ifdef VERTEX
layout (location = 0) in vec3 v_position;
layout (location = 1) in vec4 v_color;
uniform mat4 ViewProjMtx;
out vec4 io_color;
void main()
{
    gl_Position = ViewProjMtx * vec4(v_position, 1.0);    
    io_color = v_color;
}
#endif

#ifdef PIXEL
layout (location = 0) out vec4 p_color;
in vec4 io_color;
void main()
{
    p_color = io_color;
}
#endif
)";

DebugDraw& DebugDraw::Get()
{
	static DebugDraw instance;
	return instance;
}

bool DebugDraw::Initialize()
{
    // Create shaders
    {
        ShaderInfo shaderInfo;

        shaderInfo.shaderType = ShaderType::VERTEX;
        shaderInfo.source = g_debugShaderSrc;
        m_vertexShader = Graphics::Get().CreateShader(shaderInfo);

        shaderInfo.shaderType = ShaderType::PIXEL;
        shaderInfo.source = g_debugShaderSrc;
        m_pixelShader = Graphics::Get().CreateShader(shaderInfo);
    }

    // Create pipeline
    {
        PipelineInfo pipeInfo;
        pipeInfo.numRenderTargets = 1;
        pipeInfo.renderTargetFormats[0] = Graphics::Get().GetColorBufferFormat();
        pipeInfo.depthStencilFormat = Graphics::Get().GetDepthBufferFormat();

        pipeInfo.topology = PipelineTopology::LINES;
        pipeInfo.faceCull = PipelineFaceCull::CW;
        pipeInfo.depthEnable = true;

        LayoutElement layoutElems[] =
        {
            LayoutElement { 0, 0, 3, GraphicsValueType::FLOAT32, false, 0, 0 },
            LayoutElement { 1, 0, 4, GraphicsValueType::UINT8, true, 0, 0 },
        };

        pipeInfo.layoutElements = layoutElems;
        pipeInfo.numElements = ARRAYSIZE(layoutElems);

        pipeInfo.vertShader = m_vertexShader;
        pipeInfo.pixelShader = m_pixelShader;

        m_pipeline = Graphics::Get().CreatePipeline(pipeInfo);
    }

    // Create buffers
    {
        BufferInfo bufferInfo;
        BufferData bufferData;

        // Vertex buffer
        bufferInfo.type = BufferType::VERTEX_BUFFER;
        bufferInfo.usage = BufferUsage::DYNAMIC;
        bufferInfo.access = BufferAccess::WRITE;
        bufferInfo.strideBytes = sizeof(Vertex);

        bufferData.dataSize = 0;
        bufferData.pData = nullptr;

        m_vertexBuffer = Graphics::Get().CreateBuffer(bufferInfo, bufferData);
    }

    return true;
}

void DebugDraw::Shutdown()
{
    if (m_vertexShader != INVALID_GRAPHICS_HANDLE)
        Graphics::Get().DestroyShader(m_vertexShader);
    if (m_pixelShader != INVALID_GRAPHICS_HANDLE)
        Graphics::Get().DestroyShader(m_pixelShader);
    if (m_pipeline != INVALID_GRAPHICS_HANDLE)
        Graphics::Get().DestroyPipeline(m_pipeline);

    if (m_vertexBuffer != INVALID_GRAPHICS_HANDLE)
        Graphics::Get().DestroyBuffer(m_vertexBuffer);
}

void DebugDraw::Line(const Vec3& a, const Vec3& b, u32 color)
{
    m_vertices.emplace_back(a, color);
    m_vertices.emplace_back(b, color);
}

void DebugDraw::Box(const Box3& box, u32 color)
{
    // Define the 8 corners of the AABB
    const Vec3 corners[8] =
    {
        Vec3(box.min.x, box.min.y, box.min.z),
        Vec3(box.min.x, box.min.y, box.max.z),
        Vec3(box.min.x, box.max.y, box.min.z),
        Vec3(box.min.x, box.max.y, box.max.z),
        Vec3(box.max.x, box.min.y, box.min.z),
        Vec3(box.max.x, box.min.y, box.max.z),
        Vec3(box.max.x, box.max.y, box.min.z),
        Vec3(box.max.x, box.max.y, box.max.z)
    };

    // Draw edges of the AABB by connecting the corners
    // Bottom face (z = min.z)
    Line(corners[0], corners[1], color);
    Line(corners[0], corners[2], color);
    Line(corners[1], corners[3], color);
    Line(corners[2], corners[3], color);

    // Top face (z = max.z)
    Line(corners[4], corners[5], color);
    Line(corners[4], corners[6], color);
    Line(corners[5], corners[7], color);
    Line(corners[6], corners[7], color);

    // Vertical edges connecting top and bottom faces
    Line(corners[0], corners[4], color);
    Line(corners[1], corners[5], color);
    Line(corners[2], corners[6], color);
    Line(corners[3], corners[7], color);
}

void DebugDraw::Render(const Mat4& viewProj)
{
    BufferData bufferData;
    bufferData.dataSize = (u32)(sizeof(Vertex) * m_vertices.size());
    bufferData.pData = m_vertices.data();
    Graphics::Get().UpdateBuffer(m_vertexBuffer, bufferData);

    Graphics::Get().SetPipeline(m_pipeline);
    Graphics::Get().SetUniform(m_pipeline, "ViewProjMtx", GraphicsValueType::MAT4, 1, (u8*)viewProj.data);

    const u64 offset = 0;
    GraphicsHandle pBuffers[] = { m_vertexBuffer };
    Graphics::Get().SetVertexBuffers(0, 1, pBuffers, &offset);

    DrawAttribs attribs;
    attribs.numVertices = (u32)m_vertices.size();
    Graphics::Get().Draw(attribs);
}

void DebugDraw::Clear()
{
    m_vertices.clear();
}
#endif