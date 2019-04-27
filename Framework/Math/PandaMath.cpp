#include "PandaMath.h"

namespace Panda
{
    void BuildViewMatrixImpl(const Vector3D& pos, const Vector3D& eye, const Vector3D& up, Matrix4& result)
    {

    }

    void BuildViewMatrixLH(const Vector3D& pos, const Vector3D& target, const Vector3D& up, Matrix4& result)
    {
        Vector3D eye;
        eye = pos - target;
        eye.Normalize();

        BuildViewMatrixImpl(pos, eye, up, result);
    }

    void BuildViewMatrixRH(const Vector3D& pos, const Vector3D& target, const Vector3D& up, Matrix4& result)
    {
        Vector3D eye;
        eye = target - pos;
        eye.Normalize();

        BuildViewMatrixImpl(pos, eye, up, result);
    }

    void BuildViewMatrix(const Vector3D& pos, const Vector3D& target, const Vector3D& up, Matrix4& result)
    {
        BuildViewMatrixLH(pos, target, up, result);
    }
}