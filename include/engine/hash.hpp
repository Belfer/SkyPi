#pragma once

#include <engine/type_traits.hpp>
#include <engine/byte_types.hpp>
#include <engine/string.hpp>

template <typename T>
struct Hash
{
	inline SizeType operator()(const T& v) const
	{
		static std::hash<T> hash;
		return hash(v);
	}
};

namespace HashFunctions
{
	static SizeType FNV1a(const u8* data, SizeType size)
	{
		// 32 bit params
		// uint32_t constexpr fnv_prime = 16777619U;
		// uint32_t constexpr fnv_offset_basis = 2166136261U;

		// 64 bit params
		u64 constexpr fnv_prime = 1099511628211ULL;
		u64 constexpr fnv_offset_basis = 14695981039346656037ULL;

		u64 hash = fnv_offset_basis;

		for (SizeType i = 0; i < size; ++i)
		{
			hash ^= data[i];
			hash *= fnv_prime;
		}

		return hash;
	}
}

#define DECLARE_PRIMITIVE_HASH(Type, HashFn) \
template <> struct Hash<Type> { inline SizeType operator()(const Type& v) const { return HashFn(reinterpret_cast<const u8*>(&v), sizeof(Type));} };

DECLARE_PRIMITIVE_HASH(i32, HashFunctions::FNV1a);
DECLARE_PRIMITIVE_HASH(u32, HashFunctions::FNV1a);
DECLARE_PRIMITIVE_HASH(f32, HashFunctions::FNV1a);
DECLARE_PRIMITIVE_HASH(f64, HashFunctions::FNV1a);

//#if !std::is_same<SizeType, u32>::value
//DECLARE_PRIMITIVE_HASH(SizeType, HashFunctions::FNV1a);
//#endif

template <>
struct Hash<String>
{
	inline SizeType operator()(const String& v) const
	{
		return HashFunctions::FNV1a(reinterpret_cast<const u8*>(v.data()), v.length());
	}
};

template <SizeType N>
struct Hash<CString<N>>
{
	inline SizeType operator()(CString<N> v) const
	{
		return HashFunctions::FNV1a(reinterpret_cast<const u8*>(v.data()), v.length());
	}
};

template <>
struct Hash<StringView>
{
	inline SizeType operator()(StringView v) const
	{
		return HashFunctions::FNV1a(reinterpret_cast<const u8*>(v.data()), v.length());
	}
};