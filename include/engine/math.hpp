#pragma once

#include <engine/byte_types.hpp>

#include <cmath>
#include <glm/glm.hpp>

namespace Math
{
	static constexpr f32 F32Min = FLT_MIN;
	static constexpr f32 F32Max = FLT_MAX;

	template <typename T>
	static T Abs(const T& a)
	{
		return glm::abs(a);
	}

	template <typename T>
	static T FMod(const T& a, const T& b)
	{
		return std::fmod(a, b);
	}

	template <typename T>
	static T Max(const T& a, const T& b)
	{
		return (a > b) ? a : b;
	}

	template <typename T>
	static T Min(const T& a, const T& b)
	{
		return (a < b) ? a : b;
	}

	template <typename T>
	static T Clamp(const T& x, const T& min, const T& max)
	{
		return Max(Min(x, max), min);
	}
}

struct Vec2
{
	Vec2() : data{ 0, 0 } {}
	Vec2(f32 x, f32 y)
		: data{ x, y }
	{}

	union
	{
		f32 data[2];
		struct { f32 x, y; };
	};

	f32 At(i32 i);
	inline f32& operator[](i32 i) { return data[i]; }
	inline const f32& operator[](i32 i) const { return data[i]; }

	f32 SqrMagnitude();
	f32 Magnitude();
	Vec2 Normalized();

	void Set(f32 x, f32 y);

	Vec2 Plus(const Vec2& rhs) const;
	inline Vec2 operator+(const Vec2& rhs) const { return Plus(rhs); }
	inline Vec2& operator+=(const Vec2& rhs) { *this = *this + rhs; return *this; }

	Vec2 Negate() const;
	inline Vec2 operator-() const { return Negate(); }

	Vec2 Minus(const Vec2& rhs) const;
	inline Vec2 operator-(const Vec2& rhs) const { return Minus(rhs); }

	Vec2 Mul(f32 rhs) const;
	inline Vec2 operator*(f32 rhs) const { return Mul(rhs); }

	Vec2 Div(f32 rhs) const;
	inline Vec2 operator/(f32 rhs) const { return Div(rhs); }

	//static Vec3 Right() { return Vec3(1, 0, 0); }
	//static Vec3 Up() { return Vec3(0, 1, 0); }
	//static Vec3 Forward() { return Vec3(0, 0, 1); }

	static f32 Dot(const Vec2& a, const Vec2& b);

	static void Normalize(Vec2& v);

	//static Vec2 Cross(const Vec2& a, const Vec2& b);

	static Vec2 Lerp(const Vec2& a, const Vec2& b, f32 t);

	static Vec2 FromValuePtr(f32* v);
};

struct Vec3
{
	Vec3() : data{ 0, 0, 0 } {}
	Vec3(f32 x, f32 y, f32 z)
		: data{ x, y, z }
	{}

	union
	{
		f32 data[3];
		struct { f32 x, y, z; };
	};

	f32 At(i32 i);
	inline f32& operator[](i32 i) { return data[i]; }
	inline const f32& operator[](i32 i) const { return data[i]; }

	f32 SqrMagnitude();
	f32 Magnitude();
	Vec3 Normalized();

	void Set(f32 x, f32 y, f32 z);

	Vec3 Plus(const Vec3& rhs) const;
	inline Vec3 operator+(const Vec3& rhs) const { return Plus(rhs); }
	inline Vec3& operator+=(const Vec3& rhs) { *this = *this + rhs; return *this; }

	Vec3 Negate() const;
	inline Vec3 operator-() const { return Negate(); }

	Vec3 Minus(const Vec3& rhs) const;
	inline Vec3 operator-(const Vec3& rhs) const { return Minus(rhs); }

	Vec3 Mul(f32 rhs) const;
	inline Vec3 operator*(f32 rhs) const { return Mul(rhs); }

	Vec3 Div(f32 rhs) const;
	inline Vec3 operator/(f32 rhs) const { return Div(rhs); }

	static Vec3 Right() { return Vec3(1, 0, 0); }
	static Vec3 Up() { return Vec3(0, 1, 0); }
	static Vec3 Forward() { return Vec3(0, 0, 1); }

	static f32 Dot(const Vec3& a, const Vec3& b);

	static void Normalize(Vec3& v);

	static Vec3 Cross(const Vec3& a, const Vec3& b);

	static Vec3 Lerp(const Vec3& a, const Vec3& b, f32 t);

	static Vec3 FromValuePtr(f32* v);

	static Vec3 Min(const Vec3& a, const Vec3& b)
	{
		return Vec3(Math::Min(a.x, b.x), Math::Min(a.y, b.y), Math::Min(a.z, b.z));
	}

	static Vec3 Max(const Vec3& a, const Vec3& b)
	{
		return Vec3(Math::Max(a.x, b.x), Math::Max(a.y, b.y), Math::Max(a.z, b.z));
	}
};

struct Vec4
{
	Vec4() : data{ 0, 0, 0, 0 } {}
	Vec4(f32 x, f32 y, f32 z, f32 w)
		: data{ x, y, z, w }
	{}

	union
	{
		f32 data[4];
		struct { f32 x, y, z, w; };
	};

	f32 At(i32 i);
	inline f32& operator[](i32 i) { return data[i]; }
	inline const f32& operator[](i32 i) const { return data[i]; }

	f32 SqrMagnitude();
	f32 Magnitude();
	Vec4 Normalized();

	void Set(f32 x, f32 y, f32 z, f32 w);

	Vec4 Plus(const Vec4& rhs) const;
	inline Vec4 operator+(const Vec4& rhs) const { return Plus(rhs); }
	inline Vec4& operator+=(const Vec4& rhs) { *this = *this + rhs; return *this; }

	Vec4 Negate() const;
	inline Vec4 operator-() const { return Negate(); }

	Vec4 Minus(const Vec4& rhs) const;
	inline Vec4 operator-(const Vec4& rhs) const { return Minus(rhs); }

	Vec4 Mul(f32 rhs) const;
	inline Vec4 operator*(f32 rhs) const { return Mul(rhs); }

	Vec4 Div(f32 rhs) const;
	inline Vec4 operator/(f32 rhs) const { return Div(rhs); }

	static Vec4 FromValuePtr(f32* v);
};

struct Color
{
	Color() : data{ 0, 0, 0, 0 } {}
	Color(f32 r, f32 g, f32 b, f32 a)
		: data{ r, g, b, a }
	{}

	union
	{
		f32 data[4];
		struct { f32 r, g, b, a; };
	};

	//f32 At(i32 i);
	inline f32& operator[](i32 i) { return data[i]; }
	inline const f32& operator[](i32 i) const { return data[i]; }

	static Vec4 FromValuePtr(f32* v);
};

struct Vec4i
{
	Vec4i() : data{ 0, 0, 0, 0 } {}
	Vec4i(i32 x, i32 y, i32 z, i32 w)
		: data{ x, y, z, w }
	{}

	union
	{
		i32 data[4];
		struct { i32 x, y, z, w; };
	};

	//i32 At(i32 i);
	inline i32& operator[](i32 i) { return data[i]; }
	inline const i32& operator[](i32 i) const { return data[i]; }

	static Vec4i FromValuePtr(i32* v);
};

struct Quat
{
	Quat() : data{ 0, 0, 0, 0 } {}
	Quat(f32 x, f32 y, f32 z, f32 w)
		: data{ x, y, z, w }
	{}

	union
	{
		f32 data[4];
		struct { f32 x, y, z, w; };
	};

	//f32 At(i32 i);
	inline f32& operator[](i32 i) { return data[i]; }
	inline const f32& operator[](i32 i) const { return data[i]; }

	Quat Normalized();

	Quat MulQuat(Quat rhs) const;
	Vec3 MulVec3(Vec3 rhs) const;
	inline Quat operator*(Quat rhs) const { return MulQuat(rhs); }
	inline Vec3 operator*(Vec3 rhs) const { return MulVec3(rhs); }

	Vec3 EulerAngles() const;
	Quat Inverse() const;

	static Quat Euler(f32 x, f32 y, f32 z);
	static Quat AngleAxis(f32 a, const Vec3& axis);

	static void Normalize(Quat& q);

	static Quat Slerp(const Quat& a, const Quat& b, f32 t);

	static Quat FromValuePtr(f32* v);
};

struct Mat4
{
	Mat4() : data{ 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 } {}
	Mat4(Vec4 x, Vec4 y, Vec4 z, Vec4 w)
		: basis{ x, y, z, w }
	{}

	union
	{
		f32 data[16];
		Vec4 basis[4];
		//f32 m00, m01, m02, m03,
		//	m10, m11, m12, m13,
		//	m20, m21, m22, m23,
		//	m30, m31, m32, m33;
	};

	//Vec4 At(i32 i);
	inline Vec4& operator[](i32 i) { return basis[i]; }
	inline const Vec4& operator[](i32 i) const { return basis[i]; }

	//f32 At(i32 i, i32 j);
	//inline f32 operator[](i32 i, i32 j) { return At(i, j); }

	Mat4 Mul(const Mat4& rhs) const;
	inline Mat4 operator*(const Mat4& rhs) const { return Mul(rhs); }

	Mat4 Inverse() const;

	static Mat4 Identity();
	static Mat4 Zero();

	static Mat4 LookAt(const Vec3& eye, const Vec3& target, const Vec3& up);
	static Mat4 Ortho(f32 left, f32 right, f32 bottom, f32 top, f32 zNear, f32 zFar);
	static Mat4 Perspective(f32 fov, f32 aspect, f32 zNear, f32 zFar);
	static Mat4 TRS(const Vec3& pos, const Quat& q, const Vec3& s);

	static Mat4 Translate(const Vec3& translation, const Mat4& mat = Mat4::Identity());
	static Mat4 Rotate(const Quat& rotation, const Mat4& mat = Mat4::Identity());
	static Mat4 Scale(const Vec3& scaling, const Mat4& mat = Mat4::Identity());

	static void Decompose(const Mat4& m, Vec3& pos, Quat& rot, Vec3& scl);

	static Mat4 FromValuePtr(f32* v);
};

struct Line
{
	Vec3 start;
	Vec3 end;
};

struct Plane
{
	Vec3 normal;
	f32 d;

	inline void operator=(const Vec4& rhs)
	{
		normal.Set(rhs.x, rhs.y, rhs.z);
		d = rhs.w;
	}

	void Normalize()
	{
		f32 length = normal.Magnitude();
		if (length > 0)
		{
			f32 inv = 1.f / length;
			normal = normal * inv;
			d *= inv;
		}
	}
};

struct Frustrum
{
	Plane planes[6]{};

	void Normalize()
	{
		for (i32 i = 0; i < 6; ++i)
		{
			planes[i].Normalize();
		}
	}
};

struct Box3
{
	// Constructors
	Box3() : min(0, 0, 0), max(0, 0, 0) {}
	Box3(Vec3 min, Vec3 max)
		: min(min)
		, max(max)
	{}

	Vec3 min;
	Vec3 max;
};

namespace Shape
{
	namespace detail
	{
		static bool OutsidePlane(const Box3& box, const Plane& plane)
		{
			Vec3 corners[8] =
			{
				{box.min.x, box.min.y, box.min.z}, {box.max.x, box.min.y, box.min.z}, {box.min.x, box.max.y, box.min.z}, {box.max.x, box.max.y, box.min.z},
				{box.min.x, box.min.y, box.max.z}, {box.max.x, box.min.y, box.max.z}, {box.min.x, box.max.y, box.max.z}, {box.max.x, box.max.y, box.max.z}
			};

			for (const auto& corner : corners)
			{
				if (Vec3::Dot(plane.normal, corner) + plane.d > 0)
					return false;
			}
			return true;
		}

		static bool IntersectsLineSegment(const Box3& box, const Vec3& start, const Vec3& direction)
		{
			f32 tmin = 0.0f, tmax = 1.0f;

			for (i32 i = 0; i < 3; ++i) // For x, y, z axes
			{
				if (direction[i] != 0.0f)
				{
					f32 t1 = (box.min[i] - start[i]) / direction[i];
					f32 t2 = (box.max[i] - start[i]) / direction[i];

					if (t1 > t2) std::swap(t1, t2);

					tmin = Math::Max(tmin, t1);
					tmax = Math::Min(tmax, t2);

					if (tmin > tmax)
						return false;
				}
				else if (start[i] < box.min[i] || start[i] > box.max[i])
				{
					return false;
				}
			}
			return true;
		}
	}

	static bool Overlaps(const Box3& a, const Box3& b)
	{
		return a.min.x <= b.max.x && a.max.x >= b.min.x
			&& a.min.y <= b.max.y && a.max.y >= b.min.y
			&& a.min.z <= b.max.z && a.max.z >= b.min.z;
	}

	static bool Overlaps(const Box3& box, const Vec3& point)
	{
		return point.x >= box.min.x && point.x <= box.max.x
			&& point.y >= box.min.y && point.y <= box.max.y
			&& point.z >= box.min.z && point.z <= box.max.z;
	}

	static bool Overlaps(const Vec3& point, const Box3& box) { return Overlaps(box, point); }

	static bool Overlaps(const Box3& box, const Line& line)
	{
		Vec3 start = line.start;
		Vec3 end = line.end;

		// Check if either endpoint is inside the box
		if (Overlaps(box, start) || Overlaps(box, end))
			return true;

		// Check if the line intersects any face of the box
		Vec3 direction = end - start;
		return detail::IntersectsLineSegment(box, start, direction);
	}

	static bool Overlaps(const Line& line, const Box3& box) { return Overlaps(box, line); }

	static bool Overlaps(const Box3& box, const Plane& plane)
	{
		// Compute the box extents along the plane normal
		Vec3 center = (box.min + box.max) * 0.5f;
		Vec3 halfExtents = (box.max - box.min) * 0.5f;

		f32 r = halfExtents.x * Math::Abs(plane.normal.x)
			+ halfExtents.y * Math::Abs(plane.normal.y)
			+ halfExtents.z * Math::Abs(plane.normal.z);

		// Distance of box center from plane
		f32 s = Vec3::Dot(plane.normal, center) - plane.d;

		// Overlaps if the distance from the center is less than the radius
		return Math::Abs(s) <= r;
	}

	static bool Overlaps(const Plane& plane, const Box3& box) { return Overlaps(box, plane); }

	static bool Overlaps(const Box3& box, const Frustrum& frustrum)
	{
		for (const auto& plane : frustrum.planes)
		{
			// Check if all corners are outside a single plane (separation axis theorem)
			if (detail::OutsidePlane(box, plane))
				return false;
		}
		return true;
	}

	static bool Overlaps(const Frustrum& frustrum, const Box3& box) { return Overlaps(box, frustrum); }
}