#pragma once

#include "Vector2D.h"
#include "Vector3D.h"
#include "Vector4D.h"
#include "Matrix3.h"
#include "Matrix4.h"
#include <math.h>

namespace Panda
{
    void TranslateVector3D(Vector3D& inVec, const Matrix3& inMat);

    void TranslateVector3D(Vector3D& inVec, const Matrix4& inMat);

    void MatrixTranslation(Matrix4& outMat, float x, float y, float z);

    void MatrixTranslation(Matrix4& outMat, const Vector3D& inVec);

    void MatrixRotationZ(Matrix4& outMat, const float angle);

    void MatrixRotationX(Matrix4& outMat, const float angle);

    void MatrixRotationY(Matrix4& outMat, const float angle);

    void MatrixRotationYawPitchRoll(Matrix4& outMat, const float yaw, const float pitch, const float roll);

    void MatrixRotation(Matrix4& outMat, const Vector3D& inVec, const float angle);

    // build unv camera system
    void BuildViewMatrix(const Vector3D& pos, const Vector3D& target, const Vector3D& up, Matrix4& result);

    // our uvn camera system is left-handed
    void BuildPerspectiveFovLHMatrix(Matrix4& result, const float fov, const float screenAspect, const float near, const float far);

}
