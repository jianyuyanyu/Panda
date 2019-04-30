#include "PandaMath.h"

namespace Panda
{
    // build unv camera system
    void BuildViewMatrix(const Vector3D& pos, const Vector3D& target, const Vector3D& up, Matrix4& result)
    {
        Vector3D n = target - pos;
        n.Normalize();
        Vector3D u = up.CrossProduct(n);
        u.Normalize();
        Vector3D v = n.CrossProduct(u);

        result.SetIdentity();
        result.m[0][0] = u.x;
        result.m[0][1] = u.y;
        result.m[0][2] = u.z;

        result.m[1][0] = v.x;
        result.m[1][1] = v.y;
        result.m[1][2] = v.z;

        result.m[2][0] = n.x;
        result.m[2][1] = n.y;
        result.m[2][2] = n.z;

        result.m[0][3] = -pos.DotProduct(u);
        result.m[1][3] = -pos.DotProduct(v);
        result.m[2][3] = -pos.DotProduct(n);

        return;
    }

    void BuildPerspectiveFovLHMatrix(Matrix4& result, const float fov, const float aspect, const float near, const float far)
    {
        result.m[0][0] = 1.0f / (aspect * tanf(fov / 2.0f));
        result.m[0][1] = 0.0f;
        result.m[0][2] = 0.0f;
        result.m[0][3] = 0.0f;

        result.m[1][0] = 0;
        result.m[1][1] = 1.0f / tanf(fov / 2.0f);
        result.m[1][2] = 0.0f;
        result.m[1][3] = 0.0f;

        result.m[2][0] = 0.0f;
        result.m[2][1] = 0.0f;
        result.m[2][2] = far / (far - near);
        result.m[2][3] = near * far / (near - far);

        result.m[3][0] = 0.0f;
        result.m[3][1] = 0.0f;
        result.m[3][2] = 1.0f;
        result.m[3][3] = 0.0f;

        return;
    }
}