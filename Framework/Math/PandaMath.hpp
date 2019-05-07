#pragma once

#include "Vector2D.hpp"
#include "Vector3D.hpp"
#include "Vector4D.hpp"
#include "Matrix3.hpp"
#include "Matrix4.hpp"
#include <math.h>

#ifndef PI
#define PI 3.14159265358979323846f
#endif

#ifndef TWO_PI
#define TWO_PI 3.14159265358979323846f * 2.0f
#endif

namespace Panda
{
    void TranslateVector3D(Vector3Df& inVec, const Matrix3f& inMat);

    void TranslateVector3D(Vector3Df& inVec, const Matrix4f& inMat);

    void MatrixScale(Matrix4f& outMat, const float x, const float y, const float z);

    void MatrixTranslation(Matrix4f& outMat, float x, float y, float z);

    void MatrixTranslation(Matrix4f& outMat, const Vector3Df& inVec);

    void MatrixRotationZ(Matrix4f& outMat, const float angle);

    void MatrixRotationX(Matrix4f& outMat, const float angle);

    void MatrixRotationY(Matrix4f& outMat, const float angle);

    void MatrixRotationYawPitchRoll(Matrix4f& outMat, const float yaw, const float pitch, const float roll);

    void MatrixRotationAxis(Matrix4f& outMat, const Vector3Df& inVec, const float angle);

    void MatrixRotationQuaternion(Matrix4f& outMat, const Quaternion& q);

    // build unv camera system
    void BuildViewMatrix(const Vector3Df& pos, const Vector3Df& target, const Vector3Df& up, Matrix4f& result);

    // our uvn camera system is left-handed
    void BuildPerspectiveFovLHMatrix(Matrix4f& result, const float fov, const float screenAspect, const float near, const float far);

}
