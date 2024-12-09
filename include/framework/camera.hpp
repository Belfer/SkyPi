#pragma once

#include <engine/math.hpp>

class Camera
{
public:
	inline void SetView(const Mat4& view) { m_view = view; }
	inline const Mat4& GetView() const { return m_view; }

	inline void SetProjection(const Mat4& proj) { m_projection = proj; }
	inline const Mat4& GetProjection() const { return m_projection; }

    inline const Mat4& GetViewProjection() const { return m_viewProjection; }
    inline const Frustrum& GetFrustrum() const { return m_frustrum; }

    inline void Update()
    {
        // Combine the projection and view matrices
        Mat4 vp = m_projection * m_view;
        m_viewProjection = vp;

        // Extract planes from the VP matrix
        // Left plane
        m_frustrum.planes[0] = vp[3] + vp[0];
        // Right plane
        m_frustrum.planes[1] = vp[3] - vp[0];
        // Bottom plane
        m_frustrum.planes[2] = vp[3] + vp[1];
        // Top plane
        m_frustrum.planes[3] = vp[3] - vp[1];
        // Near plane
        m_frustrum.planes[4] = vp[3] + vp[2];
        // Far plane
        m_frustrum.planes[5] = vp[3] - vp[2];

        // Normalize the planes
        m_frustrum.Normalize();
    }

private:
    Mat4 m_view{ Mat4::Identity() };
	Mat4 m_projection{ Mat4::Identity() };
    Mat4 m_viewProjection{ Mat4::Identity() };
    Frustrum m_frustrum{};
};