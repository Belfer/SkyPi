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
//    rttr::registration::class_<GraphicsOpenGL>("GraphicsOpenGL")
//        .constructor();
//}

#ifdef EDITOR_BUILD
#include <imgui.h>
#include <backends/imgui_impl_opengl3.h>
#endif

#define GLSL_VERSION "#version 460 core\n"

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

#define MAX_BOUND_VERTEX_BUFFERS                        16

//#define GRAPHICS_OPENGL_BINDLESS

class GraphicsOpenGL final : public Graphics
{
    //RTTR_ENABLE(Graphics)

public:
    static GraphicsOpenGL& Get();

private:
    GraphicsOpenGL() = default;
    ~GraphicsOpenGL() = default;

    GraphicsOpenGL(const GraphicsOpenGL&) = delete;
    GraphicsOpenGL& operator=(const GraphicsOpenGL&) = delete;

    GraphicsOpenGL(GraphicsOpenGL&&) = delete;
    GraphicsOpenGL& operator=(GraphicsOpenGL&&) = delete;

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

    void SetWireframe(bool enabled) override;

#ifdef EDITOR_BUILD
    bool InitializeImGui() override
    {
        ImGui_ImplOpenGL3_Init("#version 430 core");
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
    return GraphicsOpenGL::Get();
}

GraphicsOpenGL& GraphicsOpenGL::Get()
{
    static GraphicsOpenGL instance;
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

GLuint GraphicsOpenGL::GetTextureHandle(GraphicsHandle texture)
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

    i32 flags;
    glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
    if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
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

static bool CompileShader(GLuint& shader, GLenum type, const GLchar* source)
{
    GLint status;

    if (!source)
    {
        LOGE(Graphics, "Failed to compile empty shader");
        return false;
    }

    shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);

    glCompileShader(shader);

    //#if defined(DEBUG)
    GLint log_length = 0;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_length);
    if (log_length > 1)
    {
        GLchar* log = static_cast<GLchar*>(malloc(log_length));
        glGetShaderInfoLog(shader, log_length, &log_length, log);
        if (log) LOGW(Graphics, "Program compile log: {}", log);
        free(log);
    }
    //#endif

    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (status == 0)
    {
        glDeleteShader(shader);
        return false;
    }

    return true;
}

static bool LinkProgram(GLuint program)
{
    GLint status;

    glLinkProgram(program);

    //#if defined(DEBUG)
    GLint logLength = 0;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
    if (logLength > 1)
    {
        GLchar* log = static_cast<GLchar*>(malloc(logLength));
        glGetProgramInfoLog(program, logLength, &logLength, log);
        if (log) LOGW(Graphics, "Program link log: {}", log);
        free(log);
    }
    //#endif

    glGetProgramiv(program, GL_LINK_STATUS, &status);
    return status != 0;
}

//static bool InitializeDebugDraw()
//{
//    glCreateVertexArrays(1, &g_debugVao);
//    glCreateBuffers(1, &g_debugVbo);
//
//    glVertexArrayAttribFormat(g_debugVao, 0, 3, GL_FLOAT, GL_FALSE, 0);
//    glEnableVertexArrayAttrib(g_debugVao, 0);
//    glVertexArrayAttribBinding(g_debugVao, 0, 0);
//
//    glVertexArrayAttribFormat(g_debugVao, 1, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vec3));
//    glEnableVertexArrayAttrib(g_debugVao, 1);
//    glVertexArrayAttribBinding(g_debugVao, 1, 0);
//
//    static const char* vsrc = GLSL_VERSION
//        "layout (location = 0) in vec3 Position;\n"
//        "layout (location = 1) in vec4 Color;\n"
//        "uniform mat4 ViewProjMtx;\n"
//        "out vec4 Frag_Color;\n"
//        "void main() { gl_Position = ViewProjMtx * vec4(Position, 1.0); Frag_Color = Color; }\n";
//
//    static const char* psrc = GLSL_VERSION
//        "layout (location = 0) out vec4 Out_Color;\n"
//        "in vec4 Frag_Color;\n"
//        "void main() { Out_Color = Frag_Color; }\n";
//
//    GLuint vshader;
//    if (!CompileShader(vshader, GL_VERTEX_SHADER, vsrc))
//    {
//        LOGE(Graphics, "Renderer failed to compile shader!");
//        return false;
//    }
//
//    GLuint pshader;
//    if (!CompileShader(pshader, GL_FRAGMENT_SHADER, psrc))
//    {
//        LOGE(Graphics, "Renderer failed to compile shader!");
//        return false;
//    }
//
//    g_debugShader = glCreateProgram();
//    glAttachShader(g_debugShader, vshader);
//    glAttachShader(g_debugShader, pshader);
//
//    if (!LinkProgram(g_debugShader))
//    {
//        glDeleteProgram(g_debugShader);
//        LOGE(Graphics, "Renderer failed to link shader program!");
//        return false;
//    }
//
//    glDeleteShader(vshader);
//    glDeleteShader(pshader);
//
//    return true;
//}

static WindowGLProc GetProcAddress(const char* name)
{
    return Window::Get().GetProcAddress(name);
}

bool GraphicsOpenGL::Initialize()
{
    if (!gladLoadGLLoader((GLADloadproc)GetProcAddress))
    {
        LOGE(Graphics, "Failed to initialize GLAD GL!");
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

void GraphicsOpenGL::Shutdown()
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

void GraphicsOpenGL::NewFrame()
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

void GraphicsOpenGL::EndFrame()
{
    //PROFILE_FUNCTION();

    RebalanceMap(s_shaders);
    RebalanceMap(s_buffers);
    RebalanceMap(s_textures);
    RebalanceMap(s_resources);
    RebalanceMap(s_pipelines);
}

TextureFormat GraphicsOpenGL::GetColorBufferFormat()
{
    return TextureFormat::UNKNOWN;
}

TextureFormat GraphicsOpenGL::GetDepthBufferFormat()
{
    return TextureFormat::UNKNOWN;
}

GraphicsHandle GraphicsOpenGL::GetCurrentBackBufferRT()
{
    return INVALID_GRAPHICS_HANDLE;
}

GraphicsHandle GraphicsOpenGL::GetDepthBuffer()
{
    return INVALID_GRAPHICS_HANDLE;
}

void GraphicsOpenGL::SetRenderTarget(const GraphicsHandle renderTarget, const GraphicsHandle depthStencil)
{
    if (renderTarget == INVALID_GRAPHICS_HANDLE)
    {
        // Bind the default framebuffer
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        return;
    }

    const auto& renderTarget_impl = GetImpl(renderTarget, s_textures);

    // Attach the color texture to the framebuffer
    glNamedFramebufferTexture(renderTarget_impl.fbo, GL_COLOR_ATTACHMENT0, renderTarget_impl.texture, 0);

    // Attach the depth-stencil renderbuffer, if provided
    if (depthStencil != INVALID_GRAPHICS_HANDLE)
    {
        const auto& depthStencil_impl = GetImpl(depthStencil, s_textures);
        glNamedFramebufferRenderbuffer(renderTarget_impl.fbo, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, depthStencil_impl.rbo);
    }

    // Validate the framebuffer
    GLenum status = glCheckNamedFramebufferStatus(renderTarget_impl.fbo, GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE)
    {
        LOGE(GRAPHICS, "Framebuffer is incomplete: 0x%X", status);
        return;
    }

    // Bind the framebuffer for rendering
    glBindFramebuffer(GL_FRAMEBUFFER, renderTarget_impl.fbo);

    // Optionally, set the draw buffer (only one attachment in this case)
    GLenum drawBuffer = GL_COLOR_ATTACHMENT0;
    glNamedFramebufferDrawBuffers(renderTarget_impl.fbo, 1, &drawBuffer);
}

void GraphicsOpenGL::ReadPixels(u32 x, u32 y, u32 w, u32 h, void* pixelData, const GraphicsHandle renderTarget)
{
    const auto& renderTarget_impl = GetImpl(renderTarget, s_textures);

    // Ensure the framebuffer is set before reading
    glBindFramebuffer(GL_READ_FRAMEBUFFER, renderTarget_impl.fbo);

    // Read from the color attachment
    glReadBuffer(GL_COLOR_ATTACHMENT0);
    glReadPixels(x, y, w, h, GL_RGBA, GL_UNSIGNED_BYTE, pixelData); // Assuming 8-bit RGBA
}

void GraphicsOpenGL::SetViewport(const f32 viewport[4])
{
    // Set viewport directly using the provided dimensions
    GLint x = static_cast<GLint>(viewport[0]);
    GLint y = static_cast<GLint>(viewport[1]);
    GLsizei w = static_cast<GLsizei>(viewport[2]);
    GLsizei h = static_cast<GLsizei>(viewport[3]);
    glViewport(x, y, w, h);
}

void GraphicsOpenGL::ClearRenderTarget(const GraphicsHandle rt, const f32 clearColor[4])
{
    if (rt != INVALID_GRAPHICS_HANDLE)
    {
        const auto& renderTarget_impl = GetImpl(rt, s_textures);

        // Bind the framebuffer to clear
        glBindFramebuffer(GL_FRAMEBUFFER, renderTarget_impl.fbo);

        // Clear the color buffer with specified clear color
        glClearBufferfv(GL_COLOR, 0, clearColor);
    }
    else
    {
        // Clear the default framebuffer
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClearColor(clearColor[0], clearColor[1], clearColor[2], clearColor[3]);
        glClear(GL_COLOR_BUFFER_BIT);
    }
}

void GraphicsOpenGL::ClearDepthStencil(const GraphicsHandle dt, GraphicsClearFlags flags, f32 depth, i32 stencil)
{
    if (dt != INVALID_GRAPHICS_HANDLE)
    {
        const auto& depthStencil_impl = GetImpl(dt, s_textures);

        // Bind the framebuffer to clear
        glBindFramebuffer(GL_FRAMEBUFFER, depthStencil_impl.fbo);

        // Clear depth and/or stencil
        if (flags & GraphicsClearFlags::DEPTH)
            glClearBufferfv(GL_DEPTH, 0, &depth);
        
        if (flags & GraphicsClearFlags::STENCIL)
            glClearBufferiv(GL_STENCIL, 0, &stencil);
    }
    else
    {
        // Clear the default framebuffer depth/stencil
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        GLbitfield mask = 0;
        if (flags & GraphicsClearFlags::DEPTH) mask |= GL_DEPTH_BUFFER_BIT;
        if (flags & GraphicsClearFlags::STENCIL) mask |= GL_STENCIL_BUFFER_BIT;

        glClear(mask);
    }
}

GraphicsHandle GraphicsOpenGL::CreateShader(const ShaderInfo& info)
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

    auto src = header + info.source;
    const char* const psrc = src.c_str();

    GLuint shader_handle = 0;
    GLboolean ret = CompileShader(shader_handle, shader_type, psrc);
    if (!ret)
    {
        LOGE(Graphics, "Renderer failed to compile shader!");
        return INVALID_GRAPHICS_HANDLE;
    }

    ShaderImpl shader_impl;
    shader_impl.handle = shader_handle;
    s_shaders.insert(std::make_pair(shader_handle, shader_impl));

    return shader_handle;
}

void GraphicsOpenGL::DestroyShader(const GraphicsHandle shader)
{
    auto it = s_shaders.find(shader);
    if (it == s_shaders.end())
        return;

    auto& shader_impl = it->second;
    glDeleteShader(shader_impl.handle);

    s_shaders.erase(it);
}

GraphicsHandle GraphicsOpenGL::CreateTexture(const TextureInfo& info, const BufferData& data)
{
    TextureImpl texture_impl;

    GLenum internalFormat = GetTextureFormat(info.format);
    GLenum format = GetTextureBaseFormat(info.format);
    GLenum type = GetTextureType(info.format);

#ifdef GRAPHICS_OPENGL_BINDLESS
    glCreateSamplers(1, &texture_impl.sampler);
    glSamplerParameteri(texture_impl.sampler, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glSamplerParameteri(texture_impl.sampler, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glSamplerParameteri(texture_impl.sampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glSamplerParameteri(texture_impl.sampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    glCreateTextures(GL_TEXTURE_2D, 1, &texture_impl.texture);
    glTextureStorage2D(texture_impl.texture, 1, internalFormat, info.width, info.height);

    if (data.pData)
        glTextureSubImage2D(texture_impl.texture, 0, 0, 0, info.width, info.height, format, type, data.pData);

    glGenerateTextureMipmap(texture_impl.texture);

    ENSURE(glGetTextureSamplerHandleARB != nullptr);
    texture_impl.handle = glGetTextureSamplerHandleARB(texture_impl.texture, texture_impl.sampler);
    glMakeTextureHandleResidentARB(texture_impl.handle);
#else
    glGenTextures(1, &texture_impl.texture);
    glBindTexture(GL_TEXTURE_2D, texture_impl.texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, info.width, info.height, 0, format, type, data.pData);
    glGenerateMipmap(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, 0);
#endif

    if ((i32)info.flags & (i32)TextureFlags::RENDER_TARGET)
    {
        glCreateFramebuffers(1, &texture_impl.fbo);
        glNamedFramebufferTexture(texture_impl.fbo, GL_COLOR_ATTACHMENT0, texture_impl.texture, 0);
    }

    if ((i32)info.flags & (i32)TextureFlags::DEPTH_STENCIL)
    {
        glCreateRenderbuffers(1, &texture_impl.rbo);
        glNamedRenderbufferStorage(texture_impl.rbo, internalFormat, info.width, info.height);
    }

    s_textures.insert(std::make_pair(texture_impl.texture, texture_impl));
    return texture_impl.texture;
}

void GraphicsOpenGL::DestroyTexture(const GraphicsHandle texture)
{
    auto it = s_textures.find(texture);
    if (it == s_textures.end())
        return;

    auto& texture_impl = it->second;

#ifdef GRAPHICS_OPENGL_BINDLESS
    // Make texture handle non-resident if it exists
    if (texture_impl.handle != 0)
        glMakeTextureHandleNonResidentARB(texture_impl.handle);

    // Delete the sampler
    if (texture_impl.sampler != 0)
        glDeleteSamplers(1, &texture_impl.sampler);
#endif

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

GraphicsHandle GraphicsOpenGL::CreateResourceBinding(const ResourceBindingInfo& info)
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

void GraphicsOpenGL::DestroyResourceBinding(const GraphicsHandle resources)
{
    auto it = s_resources.find(resources);
    if (it != s_resources.end())
    {
        s_resources.erase(it);
    }
}

void GraphicsOpenGL::BindResource(const GraphicsHandle resources, const char* name, GraphicsHandle resource)
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

GraphicsHandle GraphicsOpenGL::CreatePipeline(const PipelineInfo& info)
{
    const auto& vert_shader = GetImpl(info.vertShader, s_shaders);
    const auto& pixel_shader = GetImpl(info.pixelShader, s_shaders);

    GLuint program_handle = glCreateProgram();

    glAttachShader(program_handle, vert_shader.handle);
    glAttachShader(program_handle, pixel_shader.handle);

    if (!LinkProgram(program_handle))
    {
        glDeleteProgram(program_handle);

        LOGE(Graphics, "Renderer failed to link shader program!");
        return INVALID_GRAPHICS_HANDLE;
    }

    GLuint vao_handle;

    glCreateVertexArrays(1, &vao_handle);

    u32 relativeOffset = 0;
    for (u32 i = 0; i < info.numElements; i++)
    {
        const auto& elem = info.layoutElements[i];

        if (elem.relativeOffset > 0)
        {
            relativeOffset = elem.relativeOffset;
        }

        if (elem.valueType < GraphicsValueType::FLOAT16 && !elem.isNormalized)
            glVertexArrayAttribIFormat(vao_handle, elem.inputIndex, elem.numComponents, GetValueType(elem.valueType), relativeOffset);
        else
            glVertexArrayAttribFormat(vao_handle, elem.inputIndex, elem.numComponents, GetValueType(elem.valueType), elem.isNormalized, relativeOffset);

        glEnableVertexArrayAttrib(vao_handle, elem.inputIndex);
        glVertexArrayAttribBinding(vao_handle, elem.inputIndex, elem.bufferSlot);

        relativeOffset += elem.numComponents * GetValueSize(elem.valueType);
    }

    PipelineImpl pipeline_impl;
    pipeline_impl.program = program_handle;
    pipeline_impl.vao = vao_handle;
    pipeline_impl.topology = info.topology;
    pipeline_impl.faceCull = info.faceCull;
    pipeline_impl.depthEnable = info.depthEnable;
    pipeline_impl.blendEnable = info.blendEnable;
    s_pipelines.insert(std::make_pair(program_handle, pipeline_impl));

    return program_handle;
}

void GraphicsOpenGL::DestroyPipeline(const GraphicsHandle pipeline)
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

void GraphicsOpenGL::SetPipeline(const GraphicsHandle pipeline)
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

    glDisable(GL_DITHER);

    if (m_ctx.m_wireframe)
    {
        glEnable(GL_POLYGON_OFFSET_LINE);
        glPolygonOffset(-1, -1);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    else
    {
        glDisable(GL_POLYGON_OFFSET_LINE);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    glUseProgram(pipeline_impl.program);
    glBindVertexArray(pipeline_impl.vao);

    m_ctx.currentPipeline = pipeline;
}

void GraphicsOpenGL::SetUniform(const GraphicsHandle pipeline, StringView name, GraphicsValueType valueType, u32 count, u8* data)
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

void GraphicsOpenGL::CommitResources(const GraphicsHandle pipeline, const GraphicsHandle resources)
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
                GLuint binding = pipeline_impl.bufferCount++;

                glUniformBlockBinding(pipeline_impl.program, location, binding);
                glBindBufferBase(GL_UNIFORM_BUFFER, binding, buffer_impl.handle);
            }
            break;
        }

        case ResourceBindingType::TEXTURE:
        {
            if (entry.second.handle != INVALID_GRAPHICS_HANDLE)
            {
                const auto& texture_impl = GetImpl(entry.second.handle, s_textures);

                GLint location = glGetUniformLocation(pipeline_impl.program, entry.first.c_str());

#ifdef GRAPHICS_OPENGL_BINDLESS
                glUniformHandleui64ARB(location, texture_impl.handle);
#else
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, texture_impl.texture);
                glUniform1i(location, 0);
#endif
            }
            break;
        }
        }
    }
}

GraphicsHandle GraphicsOpenGL::CreateBuffer(const BufferInfo& info, const BufferData& data)
{
    GLenum target = GetBufferTarget(info.type);
    GLenum usage = GetBufferUsage(info.usage);

    GLuint buffer_handle;
    glCreateBuffers(1, &buffer_handle);
    glNamedBufferData(buffer_handle, data.dataSize, data.pData, usage);

    BufferImpl buffer_impl;
    buffer_impl.handle = buffer_handle;
    buffer_impl.target = target;
    buffer_impl.usage = usage;
    buffer_impl.stride = info.strideBytes;

    s_buffers.insert(std::make_pair(buffer_handle, buffer_impl));

    return buffer_handle;
}

void GraphicsOpenGL::DestroyBuffer(const GraphicsHandle buffer)
{
    auto it = s_buffers.find(buffer);
    if (it != s_buffers.end())
    {
        glDeleteBuffers(1, &it->second.handle);
        s_buffers.erase(it);
    }
}

void GraphicsOpenGL::UpdateBuffer(const GraphicsHandle buffer, const BufferData& data)
{
    const auto& buffer_impl = GetImpl(buffer, s_buffers);
    glNamedBufferData(buffer_impl.handle, data.dataSize, data.pData, buffer_impl.usage);
}

void GraphicsOpenGL::SetVertexBuffers(i32 i, i32 count, const GraphicsHandle* pBuffers, const u64* offset)
{
    static GLuint s_tmp_buffers[MAX_BOUND_VERTEX_BUFFERS];
    static GLintptr s_tmp_offset[MAX_BOUND_VERTEX_BUFFERS];
    static GLsizei s_tmp_strides[MAX_BOUND_VERTEX_BUFFERS];
    for (i32 i = 0; i < count; i++)
    {
        const auto& buffer_impl = GetImpl(pBuffers[i], s_buffers);
        s_tmp_buffers[i] = buffer_impl.handle;
        s_tmp_offset[i] = 0;
        s_tmp_strides[i] = buffer_impl.stride;
    }
    glBindVertexBuffers(i, count, s_tmp_buffers, s_tmp_offset, s_tmp_strides);
}

void GraphicsOpenGL::SetIndexBuffer(const GraphicsHandle buffer, i32 i)
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GetImpl(buffer, s_buffers).handle);
}

void GraphicsOpenGL::Draw(const DrawAttribs& attribs)
{
    auto& pipeline_impl = GetImpl(m_ctx.currentPipeline, s_pipelines);
    glDrawArrays(GetTopologyMode(pipeline_impl.topology), 0, attribs.numVertices);
}

void GraphicsOpenGL::DrawIndexed(const DrawIndexedAttribs& attribs)
{
    auto& pipeline_impl = GetImpl(m_ctx.currentPipeline, s_pipelines);
    glDrawElements(GetTopologyMode(pipeline_impl.topology), attribs.numIndices, GetValueType(attribs.indexType), (void*)attribs.offset);
    //glDrawElements(GL_LINE_STRIP, attribs.numIndices, GetValueType(attribs.indexType), 0);
}

void GraphicsOpenGL::SetWireframe(bool enabled)
{
    m_ctx.m_wireframe = enabled;
}