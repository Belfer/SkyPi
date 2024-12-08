#pragma once

#include <memory>

template <typename T>
using SharedPtr = std::shared_ptr<T>;

template <typename T>
using UniquePtr = std::unique_ptr<T>;

namespace meta
{
	template <typename T, typename... Args>
	UniquePtr<T> make_unique(Args&&... args)
	{
		return UniquePtr<T>(new T(std::forward<Args>(args)...));
	}
}