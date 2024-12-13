#pragma once

#include <engine/log.hpp>
#include <engine/math.hpp>
#include <engine/byte_types.hpp>
#include <engine/macros.hpp>
#include <engine/list.hpp>

// TODO: Replace const char* with StringView
#include <engine/string.hpp>

//#include <rttr/rttr_enable.h>

using GraphicsHandle = u64;
constexpr GraphicsHandle INVALID_GRAPHICS_HANDLE = -1;

enum struct GraphicsClearFlags { NONE, DEPTH, STENCIL };
enum struct GraphicsValueType { UNDEFINED, INT8, INT16, INT32, UINT8, UINT16, UINT32, FLOAT16, FLOAT32, MAT4 };

enum struct ShaderType { UNKNOWN, VERTEX, PIXEL, GEOMETRY, COMPUTE };

enum struct BufferType { VERTEX_BUFFER, INDEX_BUFFER, UNIFORM_BUFFER, STORAGE_BUFFER };
enum struct BufferUsage { IMMUTABLE, DEFAULT, DYNAMIC };
enum struct BufferAccess { NONE, READ, WRITE };

enum struct TextureFormat { UNKNOWN, RGB8_UNORM, RGBA8_UNORM, RG32_UINT, D24_UNORM_S8_UINT }; // GL_R16
enum struct TextureFlags { NONE = BIT(0), SHADER_RESOURCE = BIT(1), RENDER_TARGET = BIT(2), DEPTH_STENCIL = BIT(3) };

enum struct ResourceBindingType { UNKNOWN, TEXTURE, UNIFORM_BUFFER, STORAGE_BUFFER };
enum struct ResourceBindingAccess { STATIC, MUTABLE, DYNAMIC };

enum struct PipelineTopology { UNDEFINED, POINTS, LINES, TRIANGLES, TRIANGLE_STRIP, TRIANGLE_FAN };
enum struct PipelineFaceCull { NONE, CW, CCW };

struct ShaderInfo
{
	ShaderType shaderType = ShaderType::UNKNOWN;
	const char* source = nullptr;
};

struct BufferInfo
{
	u32 strideBytes = 0;
	BufferType type = BufferType::UNIFORM_BUFFER;
	BufferUsage usage = BufferUsage::DYNAMIC;
	BufferAccess access = BufferAccess::WRITE;
};

struct TextureInfo
{
	TextureFormat format = TextureFormat::UNKNOWN;
	u32 width = 0;
	u32 height = 0;
	TextureFlags flags = TextureFlags::SHADER_RESOURCE;
};

struct BufferData
{
	BufferData() {}
	BufferData(const void* pData, u32 dataSize)
		: pData(pData)
		, dataSize(dataSize) {}

	const void* pData = nullptr;
	u32 dataSize = 0;
};

struct ResourceBindingElement
{
	ResourceBindingElement() {}
	ResourceBindingElement(ShaderType shaderType, const char* name, u32 count, ResourceBindingType type, ResourceBindingAccess access)
		: shaderType(shaderType)
		, name(name)
		, count(count)
		, type(type)
		, access(access) {}

	ShaderType shaderType = ShaderType::UNKNOWN;
	const char* name = nullptr;
	u32 count = 0;
	ResourceBindingType type = ResourceBindingType::UNKNOWN;
	ResourceBindingAccess access = ResourceBindingAccess::STATIC;
};

struct ResourceBindingInfo
{
	const ResourceBindingElement* resources = nullptr;
	u32 numResources = 0;
};

struct LayoutElement
{
	LayoutElement() {}
	LayoutElement(u32 inputIndex, u32 bufferSlot, u32 numComponents, GraphicsValueType valueType, bool isNormalized, u32 relativeOffset, u32 instanceDataStepRate)
		: inputIndex(inputIndex)
		, bufferSlot(bufferSlot)
		, numComponents(numComponents)
		, valueType(valueType)
		, isNormalized(isNormalized)
		, relativeOffset(relativeOffset)
		, instanceDataStepRate(instanceDataStepRate) {}

	u32 inputIndex = 0;
	u32 bufferSlot = 0;
	u32 numComponents = 0;
	GraphicsValueType valueType = GraphicsValueType::FLOAT32;
	bool isNormalized = false;
	u32 relativeOffset = 0;
	u32 instanceDataStepRate = 0;
};

struct PipelineInfo
{
	u32 numRenderTargets = 0;
	TextureFormat renderTargetFormats[8] = { TextureFormat::UNKNOWN };
	TextureFormat depthStencilFormat = TextureFormat::UNKNOWN;

	PipelineTopology topology = PipelineTopology::UNDEFINED;
	PipelineFaceCull faceCull = PipelineFaceCull::NONE;

	bool depthEnable = true;
	bool blendEnable = false;

	GraphicsHandle vertShader = INVALID_GRAPHICS_HANDLE;
	GraphicsHandle pixelShader = INVALID_GRAPHICS_HANDLE;

	const LayoutElement* layoutElements = nullptr;
	u32 numElements = 0;
};

struct DrawAttribs
{
	DrawAttribs() {}
	DrawAttribs(u32 numVertices)
		: numVertices(numVertices) {}

	u32 numVertices = 0;
};

struct DrawIndexedAttribs
{
	DrawIndexedAttribs() {}
	DrawIndexedAttribs(GraphicsValueType indexType, u32 numIndices)
		: indexType(indexType), numIndices(numIndices) {}

	GraphicsValueType indexType = GraphicsValueType::UINT32;
	u32 numIndices = 0;
	u32 offset = 0;
};

class Graphics
{
	//RTTR_ENABLE()

public:
	static Graphics& Get();

public:
	Graphics() = default;
	virtual ~Graphics() = default;

	virtual bool Initialize() = 0;
	virtual void Shutdown() = 0;

	virtual void NewFrame() = 0;
	virtual void EndFrame() = 0;

	virtual TextureFormat GetColorBufferFormat() = 0;
	virtual TextureFormat GetDepthBufferFormat() = 0;

	virtual GraphicsHandle GetCurrentBackBufferRT() = 0;
	virtual GraphicsHandle GetDepthBuffer() = 0;

	virtual void SetRenderTarget(const GraphicsHandle renderTarget, const GraphicsHandle depthStencil) = 0;
	virtual void ReadPixels(u32 x, u32 y, u32 w, u32 h, void* pixelData, const GraphicsHandle renderTarget) = 0;

	virtual void SetViewport(const f32 viewport[4]) = 0;

	virtual void ClearRenderTarget(const GraphicsHandle renderTarget, const f32 clearColor[4]) = 0;
	virtual void ClearDepthStencil(const GraphicsHandle depthStencil, GraphicsClearFlags flags, f32 depth, i32 stencil) = 0;

	virtual GraphicsHandle CreateShader(const ShaderInfo& info) = 0;
	virtual void DestroyShader(const GraphicsHandle shader) = 0;

	virtual GraphicsHandle CreateBuffer(const BufferInfo& info, const BufferData& data) = 0;
	virtual void DestroyBuffer(const GraphicsHandle buffer) = 0;
	virtual void UpdateBuffer(const GraphicsHandle buffer, const BufferData& data) = 0;

	virtual GraphicsHandle CreateTexture(const TextureInfo& info, const BufferData& data) = 0;
	virtual void DestroyTexture(const GraphicsHandle texture) = 0;

	virtual GraphicsHandle CreateResourceBinding(const ResourceBindingInfo& info) = 0;
	virtual void DestroyResourceBinding(const GraphicsHandle resources) = 0;
	virtual void BindResource(const GraphicsHandle resources, const char* name, GraphicsHandle resource) = 0;

	virtual GraphicsHandle CreatePipeline(const PipelineInfo& info) = 0;
	virtual void DestroyPipeline(const GraphicsHandle pipeline) = 0;
	virtual void SetPipeline(const GraphicsHandle pipeline) = 0;
	virtual void SetUniform(const GraphicsHandle pipeline, StringView name, GraphicsValueType valueType, u32 count, u8* data) = 0;
	virtual void CommitResources(const GraphicsHandle pipeline, const GraphicsHandle resources) = 0;

	virtual void SetVertexBuffers(i32 i, i32 count, const GraphicsHandle* pBuffers, const u64* offset) = 0;
	virtual void SetIndexBuffer(const GraphicsHandle buffer, i32 i) = 0;

	virtual void Draw(const DrawAttribs& attribs) = 0;
	virtual void DrawIndexed(const DrawIndexedAttribs& attribs) = 0;

	virtual void SetWireframe(bool enabled) = 0;

#ifdef EDITOR_BUILD
	// ImGui calls (tmp solution until imgui is fully done via interface)
	virtual bool InitializeImGui() = 0;
	virtual void ShutdownImGui() = 0;
	virtual void NewFrameImGui() = 0;
	virtual void EndFrameImGui() = 0;
#endif
};

#if defined(EDITOR_BUILD) || defined(DEBUG_BUILD)
class DebugDraw
{
public:
	static DebugDraw& Get();

	bool Initialize();
	void Shutdown();

	void Line(const Vec3& a, const Vec3& b, u32 color);
	void Box(const Box3& box, u32 color);

	void Render(const Mat4& viewProj);
	void Clear();

private:
	GraphicsHandle m_vertexShader{ INVALID_GRAPHICS_HANDLE };
	GraphicsHandle m_pixelShader{ INVALID_GRAPHICS_HANDLE };
	GraphicsHandle m_pipeline{ INVALID_GRAPHICS_HANDLE };

	GraphicsHandle m_vertexBuffer{ INVALID_GRAPHICS_HANDLE };

	struct Vertex
	{
		Vec3 position{};
		u32 color{};

		Vertex() {}
		Vertex(const Vec3& p, u32 c)
			: position(p)
			, color(c)
		{}
	};

	List<Vertex> m_vertices{};
};
#endif