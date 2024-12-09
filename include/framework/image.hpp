#pragma once

#include <engine/byte_types.hpp>

struct Image
{
	bool is_16bit{ false };
	bool is_hdr{ false };

	i32 width{ 0 };
	i32 height{ 0 };
	i32 channels{ 0 };
	u8* data{ nullptr };
};