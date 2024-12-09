#pragma once

#include <memory>

template <typename T>
struct NoDelete
{
	void operator()(T* ptr) const noexcept {}
};

template <typename T>
using DefaultDelete = std::default_delete<T>;

template <typename T>
using SharedPtr = std::shared_ptr<T>;

template <typename T, typename Deleter = DefaultDelete<T>>
using UniquePtr = std::unique_ptr<T, Deleter>;

namespace meta
{
	template <typename T, typename... Args>
	UniquePtr<T> make_unique(Args&&... args)
	{
		return UniquePtr<T>(new T(std::forward<Args>(args)...));
	}
}