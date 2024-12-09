#include <engine/math.hpp>

#define GLM_LANG_STL11_FORCED
#define GLM_ENABLE_EXPERIMENTAL
//#define GLM_FORCE_QUAT_DATA_XYZW

#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/random.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>

// FIXME: There is a bug with glm when GLM_FORCE_QUAT_DATA_XYZW is defined
#define GLM_PATCH_QUAT_DATA_XYZW

#ifdef GLM_PATCH_QUAT_DATA_XYZW
static Quat QuatFromGLM(glm::quat q)
{
	return Quat(q.x, q.y, q.z, q.w);
}

static glm::quat QuatToGLM(const Quat& q)
{
	return glm::quat(q.w, q.x, q.y, q.z);
}
#else
static Quat QuatFromGLM(glm::quat q)
{
	return Quat::FromValuePtr(glm::value_ptr(q));
}

static glm::quat QuatToGLM(const Quat& q)
{
	return glm::make_quat(q.data);
}
#endif

f32 Vec2::SqrMagnitude()
{
	glm::vec2 v = glm::make_vec2(data);
	return glm::dot(v, v);
}

f32 Vec2::Magnitude()
{
	glm::vec2 v = glm::make_vec2(data);
	return glm::length(v);
}

Vec2 Vec2::Normalized()
{
	glm::vec2 v = glm::normalize(glm::make_vec2(data));
	return Vec2(v.x, v.y);
}

f32 Vec2::At(i32 i)
{
	return data[i];
}

void Vec2::Set(f32 x, f32 y)
{
	data[0] = x; data[1] = y;
}

Vec2 Vec2::Plus(const Vec2& rhs) const
{
	glm::vec2 v = glm::make_vec2(data) + glm::make_vec2(rhs.data);
	return Vec2(v.x, v.y);
}

Vec2 Vec2::Negate() const
{
	return Vec2(-x, -y);
}

Vec2 Vec2::Minus(const Vec2& rhs) const
{
	glm::vec2 v = glm::make_vec2(data) - glm::make_vec2(rhs.data);
	return Vec2(v.x, v.y);
}

Vec2 Vec2::Mul(f32 rhs) const
{
	glm::vec2 v = glm::make_vec2(data) * rhs;
	return Vec2(v.x, v.y);
}

Vec2 Vec2::Div(f32 rhs) const
{
	glm::vec2 v = glm::make_vec2(data) / rhs;
	return Vec2(v.x, v.y);
}

f32 Vec2::Dot(const Vec2& a, const Vec2& b)
{
	return glm::dot(glm::make_vec2(a.data), glm::make_vec2(b.data));
}

void Vec2::Normalize(Vec2& v)
{
	v = v.Normalized();
}

//Vec2 Vec2::Cross(const Vec2& a, const Vec2& b)
//{
//	glm::vec2 v = glm::cross(glm::make_vec2(a.data), glm::make_vec2(b.data));
//	return Vec2(v.x, v.y);
//}

Vec2 Vec2::Lerp(const Vec2& a, const Vec2& b, f32 t)
{
	glm::vec2 va = glm::make_vec2(a.data);
	glm::vec2 vb = glm::make_vec2(b.data);
	glm::vec2 vl = glm::mix(va, vb, t);
	return Vec2::FromValuePtr(glm::value_ptr(vl));
}

Vec2 Vec2::FromValuePtr(f32* vptr)
{
	Vec2 v;
	memcpy(v.data, vptr, sizeof(Vec2));
	return v;
}

f32 Vec3::SqrMagnitude()
{
	glm::vec3 v = glm::make_vec3(data);
	return glm::dot(v, v);
}

f32 Vec3::Magnitude()
{
	glm::vec3 v = glm::make_vec3(data);
	return glm::length(v);
}

Vec3 Vec3::Normalized()
{
	glm::vec3 v = glm::normalize(glm::make_vec3(data));
	return Vec3(v.x, v.y, v.z);
}

f32 Vec3::At(i32 i)
{
	return data[i];
}

void Vec3::Set(f32 x, f32 y, f32 z)
{
	data[0] = x; data[1] = y; data[2] = z;
}

Vec3 Vec3::Plus(const Vec3& rhs) const
{
	glm::vec3 v = glm::make_vec3(data) + glm::make_vec3(rhs.data);
	return Vec3(v.x, v.y, v.z);
}

Vec3 Vec3::Negate() const
{
	return Vec3(-x, -y, -z);
}

Vec3 Vec3::Minus(const Vec3& rhs) const
{
	glm::vec3 v = glm::make_vec3(data) - glm::make_vec3(rhs.data);
	return Vec3(v.x, v.y, v.z);
}

Vec3 Vec3::Mul(f32 rhs) const
{
	glm::vec3 v = glm::make_vec3(data) * rhs;
	return Vec3(v.x, v.y, v.z);
}

Vec3 Vec3::Div(f32 rhs) const
{
	glm::vec3 v = glm::make_vec3(data) / rhs;
	return Vec3(v.x, v.y, v.z);
}

f32 Vec3::Dot(const Vec3& a, const Vec3& b)
{
	return glm::dot(glm::make_vec3(a.data), glm::make_vec3(b.data));
}

void Vec3::Normalize(Vec3& v)
{
	v = v.Normalized();
}

Vec3 Vec3::Cross(const Vec3& a, const Vec3& b)
{
	glm::vec3 v = glm::cross(glm::make_vec3(a.data), glm::make_vec3(b.data));
	return Vec3(v.x, v.y, v.z);
}

Vec3 Vec3::Lerp(const Vec3& a, const Vec3& b, f32 t)
{
	glm::vec3 va = glm::make_vec3(a.data);
	glm::vec3 vb = glm::make_vec3(b.data);
	glm::vec3 vl = glm::mix(va, vb, t);
	return Vec3::FromValuePtr(glm::value_ptr(vl));
}

Vec3 Vec3::FromValuePtr(f32* vptr)
{
	Vec3 v;
	memcpy(v.data, vptr, sizeof(Vec3));
	return v;
}

f32 Vec4::SqrMagnitude()
{
	glm::vec3 v = glm::make_vec3(data);
	return glm::dot(v, v);
}

f32 Vec4::Magnitude()
{
	glm::vec3 v = glm::make_vec3(data);
	return glm::length(v);
}

Vec4 Vec4::Normalized()
{
	glm::vec4 v = glm::normalize(glm::make_vec4(data));
	return Vec4(v.x, v.y, v.z, v.w);
}

f32 Vec4::At(i32 i)
{
	return data[i];
}

void Vec4::Set(f32 x, f32 y, f32 z, f32 w)
{
	data[0] = x; data[1] = y; data[2] = z; data[3] = w;
}

Vec4 Vec4::Plus(const Vec4& rhs) const
{
	glm::vec4 v = glm::make_vec4(data) + glm::make_vec4(rhs.data);
	return Vec4(v.x, v.y, v.z, v.w);
}

Vec4 Vec4::Negate() const
{
	return Vec4(-x, -y, -z, -w);
}

Vec4 Vec4::Minus(const Vec4& rhs) const
{
	glm::vec4 v = glm::make_vec4(data) - glm::make_vec4(rhs.data);
	return Vec4(v.x, v.y, v.z, v.w);
}

Vec4 Vec4::Mul(f32 rhs) const
{
	glm::vec4 v = glm::make_vec4(data) * rhs;
	return Vec4(v.x, v.y, v.z, v.w);
}

Vec4 Vec4::Div(f32 rhs) const
{
	glm::vec4 v = glm::make_vec4(data) / rhs;
	return Vec4(v.x, v.y, v.z, v.w);
}

Vec4 Vec4::FromValuePtr(f32* vptr)
{
	Vec4 v;
	memcpy(v.data, vptr, sizeof(Vec4));
	return v;
}

Vec4i Vec4i::FromValuePtr(i32* vptr)
{
	Vec4i v;
	memcpy(v.data, vptr, sizeof(Vec4i));
	return v;
}

Quat Quat::Normalized()
{
	glm::quat q = glm::normalize(QuatToGLM(*this));
	return QuatFromGLM(q);
}

Quat Quat::MulQuat(Quat rhs) const
{
	glm::quat l = QuatToGLM(*this);
	glm::quat r = QuatToGLM(rhs);
	glm::quat q = l * r;
	return QuatFromGLM(q);
}

Vec3 Quat::MulVec3(Vec3 rhs) const
{
	glm::quat l = QuatToGLM(*this);
	glm::vec3 r = glm::make_vec3(rhs.data);
	glm::vec3 v = l * r;
	return Vec3::FromValuePtr(glm::value_ptr(v));
}

Vec3 Quat::EulerAngles() const
{
	glm::quat q = QuatToGLM(*this);
	glm::vec3 e = glm::degrees(glm::eulerAngles(q));
	return Vec3::FromValuePtr(glm::value_ptr(e));
}

Quat Quat::Inverse() const
{
	glm::quat q = QuatToGLM(*this);
	glm::quat iq = glm::inverse(q);
	return QuatFromGLM(iq);
}

Quat Quat::Euler(f32 x, f32 y, f32 z)
{
	glm::quat q = glm::quat(glm::radians(glm::vec3(x, y, z)));
	return QuatFromGLM(q);
}

Quat Quat::AngleAxis(f32 a, const Vec3& axis)
{
	glm::quat q = glm::angleAxis(glm::radians(a), glm::vec3(axis.x, axis.y, axis.z));
	return QuatFromGLM(q);
}

void Quat::Normalize(Quat& q)
{
	q = q.Normalized();
}

Quat Quat::Slerp(const Quat& a, const Quat& b, f32 t)
{
	glm::quat qa = QuatToGLM(a);
	glm::quat qb = QuatToGLM(b);
	glm::quat qs = glm::slerp(qa, qb, t);
	return QuatFromGLM(qs);
}

Quat Quat::FromValuePtr(f32* vptr)
{
	Quat q;
	memcpy(q.data, vptr, sizeof(Quat));
	return q;
}

Mat4 Mat4::Mul(const Mat4& rhs) const
{
	glm::mat4 l = glm::make_mat4(data);
	glm::mat4 r = glm::make_mat4(rhs.data);
	glm::mat4 m = l * r;
	return Mat4::FromValuePtr(glm::value_ptr(m));
}

Mat4 Mat4::Inverse() const
{
	glm::mat4 m = glm::make_mat4(data);
	glm::mat4 im = glm::inverse(m);
	return Mat4::FromValuePtr(glm::value_ptr(im));
}

Mat4 Mat4::Identity()
{
	static Mat4 s_identity = Mat4(Vec4(1, 0, 0, 0), Vec4(0, 1, 0, 0), Vec4(0, 0, 1, 0), Vec4(0, 0, 0, 1));
	return s_identity;
}

Mat4 Mat4::Zero()
{
	return Mat4(Vec4(0, 0, 0, 0), Vec4(0, 0, 0, 0), Vec4(0, 0, 0, 0), Vec4(0, 0, 0, 0));
}

Mat4 Mat4::LookAt(const Vec3& eye, const Vec3& center, const Vec3& up)
{
	glm::vec3 e = glm::make_vec3(eye.data);
	glm::vec3 c = glm::make_vec3(center.data);
	glm::vec3 u = glm::make_vec3(up.data);
	glm::mat4 m = glm::lookAt(e, c, u);
	return Mat4::FromValuePtr(glm::value_ptr(m));
}

Mat4 Mat4::Ortho(f32 left, f32 right, f32 bottom, f32 top, f32 zNear, f32 zFar)
{
	glm::mat4 m = glm::ortho(left, right, bottom, top, zNear, zFar);
	return Mat4::FromValuePtr(glm::value_ptr(m));
}

Mat4 Mat4::Perspective(f32 fov, f32 aspect, f32 zNear, f32 zFar)
{
	glm::mat4 m = glm::perspective(glm::radians(fov), aspect, zNear, zFar);
	return Mat4::FromValuePtr(glm::value_ptr(m));
}

Mat4 Mat4::TRS(const Vec3& pos, const Quat& rot, const Vec3& scl)
{
	/* This creates a mat4 directly from pos, euler, scale
	r = rad(r)
		var cr = r.map { |n| n.cos }.toList
		var sr = r.map { |n| n.sin }.toList

		return [s[0] * ( cr[1] * cr[2]),    s[1] * (sr[0] * sr[1] * cr[2] - cr[0] * sr[2]),     s[2] * (cr[0] * sr[1] * cr[2] + sr[0] * sr[2]),     0,
				s[0] * ( cr[1] * sr[2]),    s[1] * (sr[0] * sr[1] * sr[2] + cr[0] * cr[2]),     s[2] * (cr[0] * sr[1] * sr[2] - sr[0] * cr[2]),     0,
				s[0] * (-sr[1]),            s[1] * (sr[0] * cr[1]),                             s[2] * (cr[0] * cr[1]),                             0,
				p[0],                       p[1],                                               p[2],                                               1]
	*/

	glm::vec3 p = glm::make_vec3(pos.data);
	glm::quat r = QuatToGLM(rot);
	glm::vec3 s = glm::make_vec3(scl.data);
	glm::mat4 m = glm::translate(glm::mat4(1.0f), p) * glm::mat4(r) * glm::scale(glm::mat4(1.0f), s);
	return Mat4::FromValuePtr(glm::value_ptr(m));
}

Mat4 Mat4::Translate(const Vec3& translation, const Mat4& mat)
{
	glm::vec3 t = glm::make_vec3(translation.data);
	glm::mat4 m = glm::make_mat4(mat.data);
	glm::mat4 tm = glm::translate(m, t);
	return Mat4::FromValuePtr(glm::value_ptr(tm));
}

Mat4 Mat4::Rotate(const Quat& rotation, const Mat4& mat)
{
	glm::quat r = QuatToGLM(rotation);
	glm::mat4 m = glm::make_mat4(mat.data);
	glm::mat4 rm = m * glm::mat4(r);
	return Mat4::FromValuePtr(glm::value_ptr(rm));
}

Mat4 Mat4::Scale(const Vec3& scaling, const Mat4& mat)
{
	glm::vec3 s = glm::make_vec3(scaling.data);
	glm::mat4 m = glm::make_mat4(mat.data);
	glm::mat4 sm = glm::scale(m, s);
	return Mat4::FromValuePtr(glm::value_ptr(sm));
}

void Mat4::Decompose(const Mat4& m, Vec3& pos, Quat& rot, Vec3& scl)
{
	glm::mat4 transformation = glm::make_mat4(m.data);
	glm::vec3 scale;
	glm::quat rotation;
	glm::vec3 translation;
	glm::vec3 skew;
	glm::vec4 perspective;
	glm::decompose(transformation, scale, rotation, translation, skew, perspective);
	pos = Vec3::FromValuePtr(glm::value_ptr(translation));
	rot = QuatFromGLM(rotation);
	scl = Vec3::FromValuePtr(glm::value_ptr(scale));
}

Mat4 Mat4::FromValuePtr(f32* vptr)
{
	Mat4 m;
	memcpy(m.data, vptr, sizeof(Mat4));
	return m;
}