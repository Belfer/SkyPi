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

    inline const Mat4& GetInvView() const { return m_invView; }
    inline const Mat4& GetInvProjection() const { return m_invProjection; }
    inline const Mat4& GetInvViewProjection() const { return m_invViewProjection; }

    inline const Frustrum& GetFrustum() const { return m_frustum; }

    inline void Update()
    {
        // Combine the projection and view matrices
        m_viewProjection = m_projection * m_view;

        // Compute inverses
        m_invView = m_view.Inverse();
        m_invProjection = m_view.Inverse();
        m_invViewProjection = m_view.Inverse();

        // Recalculate frustum
        const Mat4& vp = m_viewProjection;

        // Left plane
        m_frustum.planes[0].normal.x = vp[0][3] + vp[0][0];
        m_frustum.planes[0].normal.y = vp[1][3] + vp[1][0];
        m_frustum.planes[0].normal.z = vp[2][3] + vp[2][0];
        m_frustum.planes[0].d = vp[3][3] + vp[3][0];

        // Right plane
        m_frustum.planes[1].normal.x = vp[0][3] - vp[0][0];
        m_frustum.planes[1].normal.y = vp[1][3] - vp[1][0];
        m_frustum.planes[1].normal.z = vp[2][3] - vp[2][0];
        m_frustum.planes[1].d = vp[3][3] - vp[3][0];

        // Bottom plane
        m_frustum.planes[2].normal.x = vp[0][3] + vp[0][1];
        m_frustum.planes[2].normal.y = vp[1][3] + vp[1][1];
        m_frustum.planes[2].normal.z = vp[2][3] + vp[2][1];
        m_frustum.planes[2].d = vp[3][3] + vp[3][1];

        // Top plane
        m_frustum.planes[3].normal.x = vp[0][3] - vp[0][1];
        m_frustum.planes[3].normal.y = vp[1][3] - vp[1][1];
        m_frustum.planes[3].normal.z = vp[2][3] - vp[2][1];
        m_frustum.planes[3].d = vp[3][3] - vp[3][1];

        // Near plane
        m_frustum.planes[4].normal.x = vp[0][3] + vp[0][2];
        m_frustum.planes[4].normal.y = vp[1][3] + vp[1][2];
        m_frustum.planes[4].normal.z = vp[2][3] + vp[2][2];
        m_frustum.planes[4].d = vp[3][3] + vp[3][2];

        // Far plane
        m_frustum.planes[5].normal.x = vp[0][3] - vp[0][2];
        m_frustum.planes[5].normal.y = vp[1][3] - vp[1][2];
        m_frustum.planes[5].normal.z = vp[2][3] - vp[2][2];
        m_frustum.planes[5].d = vp[3][3] - vp[3][2];

        // Normalize the planes
        m_frustum.Normalize();
    }

private:
    Mat4 m_view{ Mat4::Identity() };
    Mat4 m_invView{ Mat4::Identity() };

	Mat4 m_projection{ Mat4::Identity() };
    Mat4 m_invProjection{ Mat4::Identity() };
    
    Mat4 m_viewProjection{ Mat4::Identity() };
    Mat4 m_invViewProjection{ Mat4::Identity() };

    Frustrum m_frustum{};
};