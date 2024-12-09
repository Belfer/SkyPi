#pragma once

#include <engine/graphics.hpp>
#include <glad/glad.h>

static GLenum GetValueType(GraphicsValueType vt)
{
    switch (vt)
    {
    case GraphicsValueType::INT8: return GL_BYTE;
        break;
    case GraphicsValueType::INT16: return GL_SHORT;
        break;
    case GraphicsValueType::INT32: return GL_INT;
        break;

    case GraphicsValueType::UINT8: return GL_UNSIGNED_BYTE;
        break;
    case GraphicsValueType::UINT16: return GL_UNSIGNED_SHORT;
        break;
    case GraphicsValueType::UINT32: return GL_UNSIGNED_INT;
        break;

    case GraphicsValueType::FLOAT32: return GL_FLOAT;
        break;

    default:
        FAIL("Value type not supported!");
        return 0;
    }
}

static u32 GetValueSize(GraphicsValueType vt)
{
    switch (vt)
    {
    case GraphicsValueType::INT8: return sizeof(i8);
        break;
    case GraphicsValueType::INT16: return sizeof(i16);
        break;
    case GraphicsValueType::INT32: return sizeof(i32);
        break;

    case GraphicsValueType::UINT8: return sizeof(u8);
        break;
    case GraphicsValueType::UINT16: return sizeof(u16);
        break;
    case GraphicsValueType::UINT32: return sizeof(u32);
        break;

    case GraphicsValueType::FLOAT32: return sizeof(f32);
        break;

    default:
        FAIL("Value type not supported!");
        return 0;
    }
}