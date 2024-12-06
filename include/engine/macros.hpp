#pragma once

#include <engine/byte_types.hpp>
#include <engine/log.hpp>

#include <cstdlib>

#define STR(Str) #Str
#define XSTR(Str) STR(Str)

#define ARRAYSIZE(Arr) ((u32)(sizeof(Arr) / sizeof(*(Arr))))

#define BIT(x) (1 << (x))

#define ASSERT(Condition, Message) \
    do { \
        if (!(Condition)) \
        { \
			LOGE(Core, "Assertion failed: {}", Message); \
            std::abort(); \
        } \
    } while (false)

#define ENSURE(Condition) ASSERT(Condition, #Condition)
#define FAIL(Message) ASSERT(false, Message)