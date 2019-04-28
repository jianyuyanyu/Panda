#include "PandaMath.h"

namespace Panda
{
    void BuildViewMatrixImpl(const Vector3D& pos, const Vector3D& n, const Vector3D& v, Matrix4& result)
    {
        const Vector3D u = v.CrossProduct(n);
        u.Normalize();
        const Vector3D v = n.CrossProduct(u);

        result.SetIdentity();
        result.m[0][0] = u.x;
        result.m[0][1] = v.x;
        result.m[0][2] = n.x;

        result.m[1][0] = u.y;
        result.m[1][1] = v.y;
        result.m[1][2] = n.y;

        result.m[2][0] = u.z;
        result.m[2][1] = v.z;
        result.m[2][2] = n.z;

        result.m[0][3] = u.DotProduct(-pos);
        result.m[1][3] = v.DotProduct(-pos);
        result.m[2][3] = n.DotProduct(-pos);

        return;
    }

    void BuildViewMatrixLH(const Vector3D& pos, const Vector3D& target, const Vector3D& up, Matrix4& result)
    {
        Vector3D& n;
        n = target - pos;
        n.Normalize();

        BuildViewMatrixImpl(pos, n, up, result);
    }

    void BuildViewMatrixRH(const Vector3D& pos, const Vector3D& target, const Vector3D& up, Matrix4& result)
    {
        Vector3D n;
        n = pos - target;
        n.Normalize();

        BuildViewMatrixImpl(pos, n, up, result);
    }

    void BuildViewMatrix(const Vector3D& pos, const Vector3D& target, const Vector3D& up, Matrix4& result)
    {
        BuildViewMatrixLH(pos, target, up, result);
    }
}