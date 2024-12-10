#include <engine/graphics.hpp>

#include <engine/macros.hpp>
#include <engine/enum.hpp>
#include <engine/string.hpp>
#include <engine/hash_map.hpp>
#include <engine/window.hpp>
#include <engine/math.hpp>

#include "gl_common.hpp"

//#include <rttr/registration.h>
//RTTR_PLUGIN_REGISTRATION
//{
//    rttr::registration::class_<GraphicsOpenGLES>("GraphicsOpenGLES")
//        .constructor();
//}

#ifdef EDITOR_BUILD
#include <imgui.h>
#include <backends/imgui_impl_opengl3.h>
#endif

#define GLSL_VERSION "#version 310 es\n"

#define GLSL_VERT_SHADER GLSL_VERSION "#define VERTEX\n"
#define GLSL_FRAG_SHADER GLSL_VERSION "#define PIXEL\n"

#define GPU_MEMORY_INFO_DEDICATED_VIDMEM_NVX            0x9047
#define GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX      0x9048
#define GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX    0x9049
#define GPU_MEMORY_INFO_EVICTION_COUNT_NVX              0x904A
#define GPU_MEMORY_INFO_EVICTED_MEMORY_NVX              0x904B

#define VBO_FREE_MEMORY_ATI                             0x87FB
#define TEXTURE_FREE_MEMORY_ATI                         0x87FC
#define RENDERBUFFER_FREE_MEMORY_ATI                    0x87FD

#define MAX_LAYOUT_ELEMS                                16
#define MAX_BOUND_VERTEX_BUFFERS                        16

class GraphicsOpenGLES final : public Graphics
{
    //RTTR_ENABLE(Graphics)

public:
    static GraphicsOpenGLES& Get();

private:
    GraphicsOpenGLES() = default;
    ~GraphicsOpenGLES() = default;

    GraphicsOpenGLES(const GraphicsOpenGLES&) = delete;
    GraphicsOpenGLES& operator=(const GraphicsOpenGLES&) = delete;

    GraphicsOpenGLES(GraphicsOpenGLES&&) = delete;
    GraphicsOpenGLES& operator=(GraphicsOpenGLES&&) = delete;

public:
    bool Initialize() override;
    void Shutdown() override;

    void NewFrame() override;
    void EndFrame() override;

    TextureFormat GetColorBufferFormat() override;
    TextureFormat GetDepthBufferFormat() override;

    GraphicsHandle GetCurrentBackBufferRT() override;
    GraphicsHandle GetDepthBuffer() override;

    void SetRenderTarget(const GraphicsHandle renderTarget, const GraphicsHandle depthStencil) override;
    void ReadPixels(u32 x, u32 y, u32 w, u32 h, void* pixelData, const GraphicsHandle renderTarget) override;

    void SetViewport(const f32 viewport[4]) override;

    void ClearRenderTarget(const GraphicsHandle renderTarget, const f32 clearColor[4]) override;
    void ClearDepthStencil(const GraphicsHandle depthStencil, GraphicsClearFlags flags, f32 depth, i32 stencil) override;

    GraphicsHandle CreateShader(const ShaderInfo& info) override;
    void DestroyShader(const GraphicsHandle shader) override;

    GraphicsHandle CreateTexture(const TextureInfo& info, const BufferData& data) override;
    void DestroyTexture(const GraphicsHandle texture) override;

    GraphicsHandle CreateResourceBinding(const ResourceBindingInfo& info) override;
    void DestroyResourceBinding(const GraphicsHandle resources) override;
    void BindResource(const GraphicsHandle resources, const char* name, GraphicsHandle resource) override;

    GraphicsHandle CreatePipeline(const PipelineInfo& info) override;
    void DestroyPipeline(const GraphicsHandle pipeline) override;
    void SetPipeline(const GraphicsHandle pipeline) override;
    void SetUniform(const GraphicsHandle pipeline, StringView name, GraphicsValueType valueType, u32 count, u8* data) override;
    void CommitResources(const GraphicsHandle pipeline, const GraphicsHandle resources) override;

    GraphicsHandle CreateBuffer(const BufferInfo& info, const BufferData& data) override;
    void DestroyBuffer(const GraphicsHandle buffer) override;
    void UpdateBuffer(const GraphicsHandle buffer, const BufferData& data) override;

    void SetVertexBuffers(i32 i, i32 count, const GraphicsHandle* pBuffers, const u64* offset) override;
    void SetIndexBuffer(const GraphicsHandle buffer, i32 i) override;

    void Draw(const DrawAttribs& attribs) override;
    void DrawIndexed(const DrawIndexedAttribs& attribs) override;

#ifdef EDITOR_BUILD
    bool InitializeImGui() override
    {
        ImGui_ImplOpenGL3_Init("#version 300 es");
        return true;
    }

    void ShutdownImGui() override
    {
        ImGui_ImplOpenGL3_Shutdown();
    }

    void NewFrameImGui() override
    {
        ImGui_ImplOpenGL3_NewFrame();
    }

    void EndFrameImGui() override
    {
        ImGui::Render();

        //GraphicsHandle renderTarget = GetCurrentBackBufferRT();
        //GraphicsHandle depthStencil = GetDepthBuffer();
        //SetRenderTarget(renderTarget, depthStencil);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }
#endif

public:
    u32 GetTextureHandle(GraphicsHandle texture);

private:
    GraphicsContext m_ctx{};
};

Graphics& Graphics::Get()
{
    return GraphicsOpenGLES::Get();
}

GraphicsOpenGLES& GraphicsOpenGLES::Get()
{
    static GraphicsOpenGLES instance;
    return instance;
}

struct ShaderImpl
{
    GLuint handle = 0;
};

struct BufferImpl
{
    GLuint handle = 0;
    GLenum target = 0;
    GLenum usage = 0;
    GLsizei stride = 0;
};

struct TextureImpl
{
    GLuint texture = 0;
    GLuint sampler = 0;
    GLuint fbo = 0;
    GLuint rbo = 0;

    GLuint64 handle = 0;
};

struct ResourceBindingImpl
{
    struct Data
    {
        ShaderType shaderType = ShaderType::UNKNOWN;
        u32 count = 0;
        ResourceBindingType type = ResourceBindingType::UNKNOWN;
        ResourceBindingAccess access = ResourceBindingAccess::STATIC;

        GraphicsHandle handle = INVALID_GRAPHICS_HANDLE;
    };
    HashMap<String, Data> resources;
};

struct PipelineImpl
{
    GLuint program = 0;
    GLuint vao = 0;

    GraphicsHandle prevVertBuffer = INVALID_GRAPHICS_HANDLE;
    LayoutElement layoutElements[MAX_LAYOUT_ELEMS];
    u32 numElements = 0;

    PipelineTopology topology = PipelineTopology::TRIANGLES;
    PipelineFaceCull faceCull = PipelineFaceCull::CCW;

    bool depthEnable = true;
    bool blendEnable = true;

    GLuint bufferCount = 0;
};

constexpr f32 MAX_LOAD_FACTOR = 0.75f;

static HashMap<GraphicsHandle, ShaderImpl> s_shaders;
static HashMap<GraphicsHandle, BufferImpl> s_buffers;
static HashMap<GraphicsHandle, TextureImpl> s_textures;
static HashMap<GraphicsHandle, ResourceBindingImpl> s_resources;
static HashMap<GraphicsHandle, PipelineImpl> s_pipelines;

static GLuint g_debugVao = 0;
static GLuint g_debugVbo = 0;
static GLuint g_debugShader = 0;

template <typename T>
static T& GetImpl(GraphicsHandle handle, HashMap<GraphicsHandle, T>& map)
{
    auto it = map.find(handle);
    ENSURE(it != map.end());
    return it->second;
}

GLuint GraphicsOpenGLES::GetTextureHandle(GraphicsHandle texture)
{
    const auto& texture_impl = GetImpl(texture, s_textures);
    return texture_impl.texture;
}

template <typename T>
void RebalanceMap(std::unordered_map<GraphicsHandle, T>& map)
{
    if (map.load_factor() > MAX_LOAD_FACTOR)
    {
        size_t newCount = map.bucket_count() * 2;
        map.rehash(newCount);
    }
}

static const char* GetGlSource(GLenum source)
{
    switch (source)
    {
    case GL_DEBUG_SOURCE_API:               return "API";
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:     return "Window System";
    case GL_DEBUG_SOURCE_SHADER_COMPILER:   return "Shader Compiler";
    case GL_DEBUG_SOURCE_THIRD_PARTY:       return "Third Party";
    case GL_DEBUG_SOURCE_APPLICATION:       return "Application";
    case GL_DEBUG_SOURCE_OTHER:             return "Other";
    }

    return "Unknown";
}

static const char* GetGlType(GLenum type)
{
    switch (type)
    {
    case GL_DEBUG_TYPE_ERROR:               return "Error";
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: return "Deprecated Behaviour";
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  return "Undefined Behaviour";
    case GL_DEBUG_TYPE_PORTABILITY:         return "Portability";
    case GL_DEBUG_TYPE_PERFORMANCE:         return "Performance";
    case GL_DEBUG_TYPE_MARKER:              return "Marker";
    case GL_DEBUG_TYPE_PUSH_GROUP:          return "Push Group";
    case GL_DEBUG_TYPE_POP_GROUP:           return "Pop Group";
    case GL_DEBUG_TYPE_OTHER:               return "Other";
    }

    return "Unknown";
}

static const char* GetGlSeverity(GLenum severity)
{
    switch (severity)
    {
    case GL_DEBUG_SEVERITY_HIGH:            return "High";
    case GL_DEBUG_SEVERITY_MEDIUM:          return "Medium";
    case GL_DEBUG_SEVERITY_LOW:             return "Low";
    case GL_DEBUG_SEVERITY_NOTIFICATION:    return "Notification";
    }

    return "Unknown";
}

static void APIENTRY DebugCallback(GLenum source, GLenum type, u32 id, GLenum severity, GLsizei length, const char* message, const void* userParam)
{
    // Ignore non-significant error/warning codes
    if (id == 131169 || id == 131185 || id == 131218 || id == 131204)
        return;

    switch (severity)
    {
    case GL_DEBUG_SEVERITY_HIGH:
        LOGE(Graphics, "GL message ID:({}) - Source:({}) - Type:({}) - Severity:({})\n{}", id, GetGlSource(source), GetGlType(type), GetGlSeverity(severity), message);
        break;
    case GL_DEBUG_SEVERITY_MEDIUM:
        LOGW(Graphics, "GL message ID:({}) - Source:({}) - Type:({}) - Severity:({})\n{}", id, GetGlSource(source), GetGlType(type), GetGlSeverity(severity), message);
        break;
    case GL_DEBUG_SEVERITY_LOW:
        LOGI(Graphics, "GL message ID:({}) - Source:({}) - Type:({}) - Severity:({})\n{}", id, GetGlSource(source), GetGlType(type), GetGlSeverity(severity), message);
        break;

    case GL_DEBUG_SEVERITY_NOTIFICATION:
    default:
        LOGD(Graphics, "GL message ID:({}) - Source:({}) - Type:({}) - Severity:({})\n{}", id, GetGlSource(source), GetGlType(type), GetGlSeverity(severity), message);
    }
}

static bool InitializeGlDebug()
{
    const GLubyte* renderer = glGetString(GL_RENDERER);
    const GLubyte* version = glGetString(GL_VERSION);
    LOGD(Graphics, "Renderer: {}", (const char*)renderer);
    LOGD(Graphics, "OpenGL version supported: {}", (const char*)version);

    //i32 flags;
    //glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
    //if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
    {
        // Initialize debug output
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(DebugCallback, nullptr);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);

        return true;
    }

    return false;
}

static void PrintGlInfo()
{
    i32 numOfExtensions;
    glGetIntegerv(GL_NUM_EXTENSIONS, &numOfExtensions);
    LOGD(Graphics, "GL Supported extensions ({}):", numOfExtensions);
    for (i32 i = 0; i < numOfExtensions; i++)
    {
        LOGD(Graphics, (const char*)glGetStringi(GL_EXTENSIONS, i));
    }

    GLint maxUniformBufferBindings;
    glGetIntegerv(GL_MAX_UNIFORM_BUFFER_BINDINGS, &maxUniformBufferBindings);
    LOGD(Graphics, "GL Max Uniform Buffer Bindings: {}", maxUniformBufferBindings);
}

static WindowGLProc GetProcAddress(const char* name)
{
    return Window::Get().GetProcAddress(name);
}

bool GraphicsOpenGLES::Initialize()
{
    if (!gladLoadGLES2Loader((GLADloadproc)GetProcAddress))
    {
        LOGE(Graphics, "Failed to initialize GLAD GLES!");
        return false;
    }

#if defined(DEBUG_BUILD) || defined(EDITOR_BUILD)
    if (!InitializeGlDebug())
        LOGW(Graphics, "GL debug output not supported.");

    //PrintGlInfo();
#endif

    //InitializeDebugDraw();

    return true;
}

void GraphicsOpenGLES::Shutdown()
{
    for (const auto& it : s_shaders)
    {
        glDeleteShader(it.second.handle);
    }

    for (const auto& it : s_buffers)
    {
        glDeleteBuffers(1, &it.second.handle);
    }

    for (const auto& it : s_pipelines)
    {
        glDeleteProgram(it.second.program);
        glDeleteVertexArrays(1, &it.second.vao);
    }

    s_shaders.clear();
    s_buffers.clear();
    s_pipelines.clear();
}

void GraphicsOpenGLES::NewFrame()
{
    //PROFILE_FUNCTION();

    i32 width, height;
    Window::Get().GetSize(&width, &height);

    glViewport(0, 0, width, height);
    glClearColor(0.f, 0.f, 0.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    // TODO: Find way to track GPU memory on Rpi
    //i32 values[4] = { -1, -1, -1, -1 };
    //glGetIntegerv(GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX, values);
    //if (values[0] > -1) Log::Info("GPU memory: {}", values[0]);
    //glGetIntegerv(TEXTURE_FREE_MEMORY_ATI, values);
    //if (values[0] > -1) Log::Info("GPU memory: {}", values[0]);
}

void GraphicsOpenGLES::EndFrame()
{
    //PROFILE_FUNCTION();

    RebalanceMap(s_shaders);
    RebalanceMap(s_buffers);
    RebalanceMap(s_textures);
    RebalanceMap(s_resources);
    RebalanceMap(s_pipelines);
}

TextureFormat GraphicsOpenGLES::GetColorBufferFormat()
{
    return TextureFormat::UNKNOWN;
}

TextureFormat GraphicsOpenGLES::GetDepthBufferFormat()
{
    return TextureFormat::UNKNOWN;
}

GraphicsHandle GraphicsOpenGLES::GetCurrentBackBufferRT()
{
    return INVALID_GRAPHICS_HANDLE;
}

GraphicsHandle GraphicsOpenGLES::GetDepthBuffer()
{
    return INVALID_GRAPHICS_HANDLE;
}

void GraphicsOpenGLES::SetRenderTarget(const GraphicsHandle renderTarget, const GraphicsHandle depthStencil)
{
    if (renderTarget == INVALID_GRAPHICS_HANDLE)
    {
        // Bind the default framebuffer
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        return;
    }

    const auto& renderTarget_impl = GetImpl(renderTarget, s_textures);

    // Bind the framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, renderTarget_impl.fbo);

    // Attach the color texture
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderTarget_impl.texture, 0);

    // Attach the depth-stencil renderbuffer, if provided
    if (depthStencil != INVALID_GRAPHICS_HANDLE)
    {
        const auto& depthStencil_impl = GetImpl(depthStencil, s_textures);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, depthStencil_impl.rbo);
    }

    // Validate the framebuffer
    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE)
    {
        LOGE(GRAPHICS, "Framebuffer is incomplete: 0x%X", status);
        return;
    }
}

void GraphicsOpenGLES::ReadPixels(u32 x, u32 y, u32 w, u32 h, void* pixelData, const GraphicsHandle renderTarget)
{
    const auto& renderTarget_impl = GetImpl(renderTarget, s_textures);

    // Bind the framebuffer for reading
    glBindFramebuffer(GL_FRAMEBUFFER, renderTarget_impl.fbo);

    // Read pixels from the framebuffer
    glReadPixels(x, y, w, h, GL_RG_INTEGER, GL_UNSIGNED_INT, pixelData);

    // Unbind the framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void GraphicsOpenGLES::SetViewport(const f32 viewport[4])
{
    // Set the viewport
    GLint x = static_cast<GLint>(viewport[0]);
    GLint y = static_cast<GLint>(viewport[1]);
    GLsizei w = static_cast<GLsizei>(viewport[2]);
    GLsizei h = static_cast<GLsizei>(viewport[3]);
    glViewport(x, y, w, h);
}

void GraphicsOpenGLES::ClearRenderTarget(const GraphicsHandle rt, const f32 clearColor[4])
{
    if (rt != INVALID_GRAPHICS_HANDLE)
    {
        const auto& renderTarget_impl = GetImpl(rt, s_textures);
        glBindFramebuffer(GL_FRAMEBUFFER, renderTarget_impl.fbo);
    }
    else
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    // Set the clear color
    glClearColor(clearColor[0], clearColor[1], clearColor[2], clearColor[3]);

    // Clear the color buffer
    glClear(GL_COLOR_BUFFER_BIT);

    // Unbind the framebuffer
    if (rt != INVALID_GRAPHICS_HANDLE)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
}

void GraphicsOpenGLES::ClearDepthStencil(const GraphicsHandle dt, GraphicsClearFlags flags, f32 depth, i32 stencil)
{
    if (dt != INVALID_GRAPHICS_HANDLE)
    {
        const auto& depthStencil_impl = GetImpl(dt, s_textures);
        glBindFramebuffer(GL_FRAMEBUFFER, depthStencil_impl.fbo);
    }
    else
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    // Set the depth and stencil clear values
    glClearDepthf(depth);
    glClearStencil(stencil);

    // Clear the depth and stencil buffers based on flags
    GLbitfield mask = 0;
    if (flags & GraphicsClearFlags::DEPTH)
        mask |= GL_DEPTH_BUFFER_BIT;
    if (flags & GraphicsClearFlags::STENCIL)
        mask |= GL_STENCIL_BUFFER_BIT;

    glClear(mask);

    // Unbind the framebuffer
    if (dt != INVALID_GRAPHICS_HANDLE)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
}

GraphicsHandle GraphicsOpenGLES::CreateShader(const ShaderInfo& info)
{
    String header;
    GLenum shader_type = 0;

    switch (info.shaderType)
    {
    case ShaderType::VERTEX:
        header = GLSL_VERT_SHADER;
        shader_type = GL_VERTEX_SHADER;
        break;
    case ShaderType::PIXEL:
        header = GLSL_FRAG_SHADER;
        shader_type = GL_FRAGMENT_SHADER;
        break;
    default:
        LOGE(Graphics, "Shader type not supported!");
        return INVALID_GRAPHICS_HANDLE;
    }

    // Combine header with source.
    String source = header + info.source;
    const char* const pSource = source.c_str();

    // Create and compile the shader.
    GLuint shader_handle = glCreateShader(shader_type);
    if (!shader_handle)
    {
        LOGE(Graphics, "Failed to create shader of type {}", shader_type);
        return INVALID_GRAPHICS_HANDLE;
    }

    glShaderSource(shader_handle, 1, &pSource, nullptr);
    glCompileShader(shader_handle);

    // Check for compilation errors.
    GLint compile_status = GL_FALSE;
    glGetShaderiv(shader_handle, GL_COMPILE_STATUS, &compile_status);

    if (compile_status != GL_TRUE)
    {
        GLint log_length = 0;
        glGetShaderiv(shader_handle, GL_INFO_LOG_LENGTH, &log_length);

        if (log_length > 0)
        {
            std::vector<GLchar> log(log_length);
            glGetShaderInfoLog(shader_handle, log_length, nullptr, log.data());
            LOGW(Graphics, "Shader compile log: {}", log.data());
        }

        glDeleteShader(shader_handle);
        LOGE(Graphics, "Failed to compile shader of type {}", shader_type);
        return INVALID_GRAPHICS_HANDLE;
    }

    // Store shader metadata and return the handle.
    ShaderImpl shader_impl;
    shader_impl.handle = shader_handle;
    s_shaders.emplace(shader_handle, shader_impl);

    return shader_handle;
}

void GraphicsOpenGLES::DestroyShader(const GraphicsHandle shader)
{
    auto it = s_shaders.find(shader);
    if (it == s_shaders.end())
        return;

    glDeleteShader(it->second.handle);
    s_shaders.erase(it);
}

GraphicsHandle GraphicsOpenGLES::CreateTexture(const TextureInfo& info, const BufferData& data)
{
    TextureImpl texture_impl;

    GLenum internalFormat = GetTextureFormat(info.format);
    GLenum format = GetTextureBaseFormat(info.format);
    GLenum type = GetTextureType(info.format);

    // Create and bind the texture
    glGenTextures(1, &texture_impl.texture);
    glBindTexture(GL_TEXTURE_2D, texture_impl.texture);

    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Allocate and upload texture data
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, info.width, info.height, 0, format, type, data.pData);

    // Generate mipmaps
    glGenerateMipmap(GL_TEXTURE_2D);

    // Unbind the texture
    glBindTexture(GL_TEXTURE_2D, 0);

    // Handle additional flags for render targets or depth-stencil buffers
    if (info.flags & TextureFlags::RENDER_TARGET)
    {
        glGenFramebuffers(1, &texture_impl.fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, texture_impl.fbo);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture_impl.texture, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    if (info.flags & TextureFlags::DEPTH_STENCIL)
    {
        glGenRenderbuffers(1, &texture_impl.rbo);
        glBindRenderbuffer(GL_RENDERBUFFER, texture_impl.rbo);
        glRenderbufferStorage(GL_RENDERBUFFER, internalFormat, info.width, info.height);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
    }

    // Store the texture metadata and return the handle
    s_textures.insert(std::make_pair(texture_impl.texture, texture_impl));
    return texture_impl.texture;
}

void GraphicsOpenGLES::DestroyTexture(const GraphicsHandle texture)
{
    auto it = s_textures.find(texture);
    if (it == s_textures.end())
        return;

    auto& texture_impl = it->second;

    // Delete the texture
    if (texture_impl.texture != 0)
        glDeleteTextures(1, &texture_impl.texture);

    // Delete the framebuffer (if it's a render target)
    if (texture_impl.fbo != 0)
        glDeleteFramebuffers(1, &texture_impl.fbo);

    // Delete the renderbuffer (if it's a depth-stencil buffer)
    if (texture_impl.rbo != 0)
        glDeleteRenderbuffers(1, &texture_impl.rbo);

    // Remove the texture from the texture map
    s_textures.erase(it);
}

GraphicsHandle GraphicsOpenGLES::CreateResourceBinding(const ResourceBindingInfo& info)
{
    ResourceBindingImpl resource_impl;
    for (u32 i = 0; i < info.numResources; ++i)
    {
        const auto& elem = info.resources[i];

        ResourceBindingImpl::Data data;
        data.shaderType = elem.shaderType;
        data.count = elem.count;
        data.type = elem.type;
        data.access = elem.access;

        resource_impl.resources.insert(std::make_pair(elem.name, data));
    }

    static GraphicsHandle counter = 0;
    GraphicsHandle handle = counter++;
    s_resources.insert(std::make_pair(handle, resource_impl));
    return handle;
}

void GraphicsOpenGLES::DestroyResourceBinding(const GraphicsHandle resources)
{
    auto it = s_resources.find(resources);
    if (it != s_resources.end())
    {
        s_resources.erase(it);
    }
}

void GraphicsOpenGLES::BindResource(const GraphicsHandle resources, const char* name, GraphicsHandle resource)
{
    auto& resource_impl = GetImpl(resources, s_resources);

    auto it = resource_impl.resources.find(name);
    if (it == resource_impl.resources.end())
    {
        ResourceBindingImpl::Data data;
        data.handle = resource;
        resource_impl.resources.insert(std::make_pair(name, data));
        return;
    }

    it->second.handle = resource;
}

GraphicsHandle GraphicsOpenGLES::CreatePipeline(const PipelineInfo& info)
{
    // Retrieve shaders from storage
    const auto& vert_shader = GetImpl(info.vertShader, s_shaders);
    const auto& pixel_shader = GetImpl(info.pixelShader, s_shaders);

    // Create and link the shader program
    GLuint program_handle = glCreateProgram();

    glAttachShader(program_handle, vert_shader.handle);
    glAttachShader(program_handle, pixel_shader.handle);

    glLinkProgram(program_handle);

    GLint status = 0;
    glGetProgramiv(program_handle, GL_LINK_STATUS, &status);
    if (status == GL_FALSE)
    {
        GLint logLength = 0;
        glGetProgramiv(program_handle, GL_INFO_LOG_LENGTH, &logLength);

        if (logLength > 1)
        {
            GLchar* log = static_cast<GLchar*>(malloc(logLength));
            glGetProgramInfoLog(program_handle, logLength, nullptr, log);
            LOGW(Graphics, "Program link log: {}", log);
            free(log);
        }

        glDeleteProgram(program_handle);
        LOGE(Graphics, "Failed to link shader program!");
        return INVALID_GRAPHICS_HANDLE;
    }

    // Create and bind the Vertex Array Object (VAO)
    GLuint vao_handle;
    glGenVertexArrays(1, &vao_handle);

    // Initialize and store pipeline implementation
    PipelineImpl pipeline_impl;
    pipeline_impl.program = program_handle;
    pipeline_impl.vao = vao_handle;
    pipeline_impl.topology = info.topology;
    pipeline_impl.faceCull = info.faceCull;
    pipeline_impl.depthEnable = info.depthEnable;
    pipeline_impl.blendEnable = info.blendEnable;

    ENSURE(info.numElements <= MAX_LAYOUT_ELEMS);
    pipeline_impl.numElements = info.numElements;
    for (u32 i = 0; i < info.numElements; ++i)
    {
        pipeline_impl.layoutElements[i] = info.layoutElements[i];
    }

    s_pipelines.insert(std::make_pair(program_handle, pipeline_impl));
    return program_handle;
}

void GraphicsOpenGLES::DestroyPipeline(const GraphicsHandle pipeline)
{
    auto it = s_pipelines.find(pipeline);
    if (it == s_pipelines.end())
        return;

    const auto& pipeline_impl = it->second;

    if (pipeline_impl.program != 0)
        glDeleteProgram(pipeline_impl.program);

    if (pipeline_impl.vao != 0)
        glDeleteVertexArrays(1, &pipeline_impl.vao);

    s_pipelines.erase(it);
}

void GraphicsOpenGLES::SetPipeline(const GraphicsHandle pipeline)
{
    auto& pipeline_impl = GetImpl(pipeline, s_pipelines);
    pipeline_impl.bufferCount = 0;

    if (pipeline_impl.faceCull == PipelineFaceCull::NONE)
    {
        glDisable(GL_CULL_FACE);
    }
    else
    {
        glEnable(GL_CULL_FACE);
        glFrontFace(GetCullMode(pipeline_impl.faceCull));
    }
    
    pipeline_impl.depthEnable ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);
    pipeline_impl.blendEnable ? glEnable(GL_BLEND) : glDisable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glUseProgram(pipeline_impl.program);
    glBindVertexArray(pipeline_impl.vao);

    m_ctx.currentPipeline = pipeline;
}

void GraphicsOpenGLES::SetUniform(const GraphicsHandle pipeline, StringView name, GraphicsValueType valueType, u32 count, u8* data)
{
    auto& pipeline_impl = GetImpl(pipeline, s_pipelines);

    switch (valueType)
    {
    case GraphicsValueType::MAT4:
    {
        GLint location = glGetUniformLocation(pipeline_impl.program, name.data());
        glUniformMatrix4fv(location, count, GL_FALSE, (GLfloat*)data);
    }
    }
}

void GraphicsOpenGLES::CommitResources(const GraphicsHandle pipeline, const GraphicsHandle resources)
{
    auto& pipeline_impl = GetImpl(pipeline, s_pipelines);
    const auto& resource_impl = GetImpl(resources, s_resources);

    for (const auto& entry : resource_impl.resources)
    {
        switch (entry.second.type)
        {
        case ResourceBindingType::UNIFORM_BUFFER:
        {
            if (entry.second.handle != INVALID_GRAPHICS_HANDLE)
            {
                const auto& buffer_impl = GetImpl(entry.second.handle, s_buffers);

                GLuint location = glGetUniformBlockIndex(pipeline_impl.program, entry.first.c_str());
                if (location != GL_INVALID_INDEX)
                {
                    GLuint binding = pipeline_impl.bufferCount++;
                    glUniformBlockBinding(pipeline_impl.program, location, binding);
                    glBindBufferBase(GL_UNIFORM_BUFFER, binding, buffer_impl.handle);
                }
            }
            break;
        }

        case ResourceBindingType::TEXTURE:
        {
            if (entry.second.handle != INVALID_GRAPHICS_HANDLE)
            {
                const auto& texture_impl = GetImpl(entry.second.handle, s_textures);

                GLint location = glGetUniformLocation(pipeline_impl.program, entry.first.c_str());
                if (location >= 0)
                {
                    glActiveTexture(GL_TEXTURE0);
                    glBindTexture(GL_TEXTURE_2D, texture_impl.texture);
                    glUniform1i(location, 0);
                }
            }
            break;
        }
        }
    }
}

GraphicsHandle GraphicsOpenGLES::CreateBuffer(const BufferInfo& info, const BufferData& data)
{
    GLenum target = GetBufferTarget(info.type);
    GLenum usage = GetBufferUsage(info.usage);

    GLuint buffer_handle;
    glGenBuffers(1, &buffer_handle);
    glBindBuffer(target, buffer_handle);
    glBufferData(target, data.dataSize, data.pData, usage);
    glBindBuffer(target, 0);

    BufferImpl buffer_impl;
    buffer_impl.handle = buffer_handle;
    buffer_impl.target = target;
    buffer_impl.usage = usage;
    buffer_impl.stride = info.strideBytes;

    s_buffers.insert(std::make_pair(buffer_handle, buffer_impl));

    return buffer_handle;
}

void GraphicsOpenGLES::DestroyBuffer(const GraphicsHandle buffer)
{
    auto it = s_buffers.find(buffer);
    if (it != s_buffers.end())
    {
        glDeleteBuffers(1, &it->second.handle);
        s_buffers.erase(it);
    }
}

void GraphicsOpenGLES::UpdateBuffer(const GraphicsHandle buffer, const BufferData& data)
{
    const auto& buffer_impl = GetImpl(buffer, s_buffers);
    glBindBuffer(buffer_impl.target, buffer_impl.handle);
    glBufferData(buffer_impl.target, data.dataSize, data.pData, buffer_impl.usage);
    glBindBuffer(buffer_impl.target, 0);
}

void GraphicsOpenGLES::SetVertexBuffers(i32 startSlot, i32 count, const GraphicsHandle* pBuffers, const u64* offsets)
{
    auto& pipeline_impl = GetImpl(m_ctx.currentPipeline, s_pipelines);

    for (i32 i = 0; i < count; ++i)
    {
        const i32 slot = startSlot + i;
        const auto& buffer_impl = GetImpl(pBuffers[i], s_buffers);

        // Bind the buffer
        glBindBuffer(GL_ARRAY_BUFFER, buffer_impl.handle);

        u32 relativeOffset = static_cast<u32>(offsets ? offsets[i] : 0);

        for (u32 j = 0; j < pipeline_impl.numElements; ++j)
        {
            const auto& elem = pipeline_impl.layoutElements[j];

            if (elem.bufferSlot != slot)
                continue; // Skip attributes not associated with this buffer slot

            if (elem.valueType < GraphicsValueType::FLOAT16 && !elem.isNormalized)
                glVertexAttribIPointer(elem.inputIndex, elem.numComponents, GetValueType(elem.valueType), buffer_impl.stride, (void*)relativeOffset);
            else
                glVertexAttribPointer(elem.inputIndex, elem.numComponents, GetValueType(elem.valueType), elem.isNormalized, buffer_impl.stride, (void*)relativeOffset);

            glEnableVertexAttribArray(elem.inputIndex);

            relativeOffset += elem.numComponents * GetValueSize(elem.valueType);
        }
    }
}

void GraphicsOpenGLES::SetIndexBuffer(const GraphicsHandle buffer, i32 i)
{
    const auto& buffer_impl = GetImpl(buffer, s_buffers);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer_impl.handle);
}

void GraphicsOpenGLES::Draw(const DrawAttribs& attribs)
{
    auto& pipeline_impl = GetImpl(m_ctx.currentPipeline, s_pipelines);
    glDrawArrays(GetTopologyMode(pipeline_impl.topology), 0, attribs.numVertices);
}

void GraphicsOpenGLES::DrawIndexed(const DrawIndexedAttribs& attribs)
{
    auto& pipeline_impl = GetImpl(m_ctx.currentPipeline, s_pipelines);
    glDrawElements(GetTopologyMode(pipeline_impl.topology), attribs.numIndices, GetValueType(attribs.indexType), 0);
}