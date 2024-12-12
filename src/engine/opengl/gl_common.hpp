#pragma once

#include <engine/graphics.hpp>
#include <glad/glad.h>

struct GraphicsContext
{
    bool m_wireframe{ false };
    GraphicsHandle currentPipeline{ INVALID_GRAPHICS_HANDLE };
};

static GLenum GetTextureFormat(TextureFormat format)
{
    switch (format)
    {
#ifdef GRAPHICS_OPENGLES_BACKED
    case TextureFormat::RGB8_UNORM:         return GL_RGB;
    case TextureFormat::RGBA8_UNORM:        return GL_RGBA;
#else
    case TextureFormat::RGBA8_UNORM:        return GL_RGBA8;
    case TextureFormat::RGB8_UNORM:         return GL_RGB8;
#endif
    case TextureFormat::RG32_UINT:          return GL_RG_INTEGER;
    case TextureFormat::D24_UNORM_S8_UINT:  return GL_DEPTH_STENCIL;

    default:
        FAIL("Texture format not supported!");
        return 0;
    }
}

static GLenum GetTextureBaseFormat(TextureFormat format)
{
    // Base format is the same across OpenGL and OpenGL ES
    switch (format)
    {
    case TextureFormat::RGB8_UNORM:         return GL_RGB;
    case TextureFormat::RGBA8_UNORM:        return GL_RGBA;
    case TextureFormat::RG32_UINT:          return GL_RG_INTEGER;
    case TextureFormat::D24_UNORM_S8_UINT:  return GL_DEPTH_STENCIL;

    default:
        FAIL("Texture base format not supported!");
        return 0;
    }
}

static GLenum GetTextureType(TextureFormat format)
{
    switch (format)
    {
    case TextureFormat::RGB8_UNORM:
    case TextureFormat::RGBA8_UNORM:       return GL_UNSIGNED_BYTE;
    case TextureFormat::RG32_UINT:         return GL_UNSIGNED_INT;
    case TextureFormat::D24_UNORM_S8_UINT: return GL_UNSIGNED_INT_24_8;

    default:
        FAIL("Texture type not supported!");
        return 0;
    }
}

static GLenum GetBufferTarget(BufferType type)
{
    switch (type)
    {
    case BufferType::VERTEX_BUFFER: return GL_ARRAY_BUFFER;
    case BufferType::INDEX_BUFFER: return GL_ELEMENT_ARRAY_BUFFER;
    case BufferType::UNIFORM_BUFFER: return GL_UNIFORM_BUFFER;
    case BufferType::STORAGE_BUFFER: return GL_SHADER_STORAGE_BUFFER;

    default:
        FAIL("Buffer type not supported!");
        return 0;
    }
}

static GLenum GetBufferUsage(BufferUsage usage)
{
    switch (usage)
    {
    case BufferUsage::IMMUTABLE: return GL_STATIC_DRAW;
    case BufferUsage::DEFAULT: return GL_DYNAMIC_DRAW;
    case BufferUsage::DYNAMIC: return GL_STREAM_DRAW;

    default:
        FAIL("Buffer usage not supported!");
        return 0;
    }
}

static GLenum GetValueType(GraphicsValueType vt)
{
    switch (vt)
    {
    case GraphicsValueType::INT8:       return GL_BYTE;
    case GraphicsValueType::INT16:      return GL_SHORT;
    case GraphicsValueType::INT32:      return GL_INT;

    case GraphicsValueType::UINT8:      return GL_UNSIGNED_BYTE;
    case GraphicsValueType::UINT16:     return GL_UNSIGNED_SHORT;
    case GraphicsValueType::UINT32:     return GL_UNSIGNED_INT;

    case GraphicsValueType::FLOAT32:    return GL_FLOAT;

    default:
        FAIL("Value type not supported!");
        return 0;
    }
}

static u32 GetValueSize(GraphicsValueType vt)
{
    switch (vt)
    {
    case GraphicsValueType::INT8:       return sizeof(i8);
    case GraphicsValueType::INT16:      return sizeof(i16);
    case GraphicsValueType::INT32:      return sizeof(i32);

    case GraphicsValueType::UINT8:      return sizeof(u8);
    case GraphicsValueType::UINT16:     return sizeof(u16);
    case GraphicsValueType::UINT32:     return sizeof(u32);

    case GraphicsValueType::FLOAT32:    return sizeof(f32);

    default:
        FAIL("Value type not supported!");
        return 0;
    }
}

static GLenum GetCullMode(PipelineFaceCull faceCull)
{
    switch (faceCull)
    {
    case PipelineFaceCull::CW:      return GL_CW;
    case PipelineFaceCull::CCW:     return GL_CCW;

    default:
        FAIL("Face cull not supported!");
        return 0;
    }
}

static GLenum GetTopologyMode(PipelineTopology topology)
{
    switch (topology)
    {
    case PipelineTopology::POINTS:          return GL_POINTS;
    case PipelineTopology::LINES:           return GL_LINES;
    case PipelineTopology::TRIANGLES:       return GL_TRIANGLES;
    case PipelineTopology::TRIANGLE_STRIP:  return GL_TRIANGLE_STRIP;
    case PipelineTopology::TRIANGLE_FAN:    return GL_TRIANGLE_FAN;

    default:
        FAIL("Topology mode not supported!");
        return 0;
    }
}