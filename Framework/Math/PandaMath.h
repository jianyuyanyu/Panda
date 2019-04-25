#pragma once

#include "Vector2D.h"
#include "Vector3D.h"
#include "Vector4D.h"
#include "Matrix3.h"
#include "Matrix4.h"
#include <math.h>

namespace Panda
{
    void TranslateVector3D(Vector3D& inVec, const Matrix3& inMat)
    {
        float x, y, z;

        x = inMat.m[0][0] * inVec.x + inMat.m[0][1] * inVec.y + inMat.m[0][2] * inVec.z;
        y = inMat.m[1][0] * inVec.x + inMat.m[1][1] * inVec.y + inMat.m[1][2] * inVec.z;
        z = inMat.m[2][0] * inVec.x + inMat.m[2][1] * inVec.y + inMat.m[2][2] * inVec.z;

        inVec.x = x;
        inVec.y = y;
        inVec.z = z;

        return;
    }

    void TranslateVector3D(Vector3D& inVec, const Matrix4& inMat)
    {
        float x, y, z;

        x = inMat.m[0][0] * inVec.x + inMat.m[0][1] * inVec.y + inMat.m[0][2] * inVec.z;
        y = inMat.m[1][0] * inVec.x + inMat.m[1][1] * inVec.y + inMat.m[1][2] * inVec.z;
        z = inMat.m[2][0] * inVec.x + inMat.m[2][1] * inVec.y + inMat.m[2][2] * inVec.z;

        inVec.x = x;
        inVec.y = y;
        inVec.z = z;

        return;
    }

    void MatrixTranslation(Matrix4& outMat, float x, float y, float z)
    {
        outMat.SetIdentity();
        outMat.m[0][3] = x;
        outMat.m[1][3] = y;
        outMat.m[2][3] = z;

        return;
    }

    void MatrixRotationZ(Matrix4& outMat, const float angle)
    {
        outMat.SetIdentity();

        float cosValue = cosf(angle);
        float sinValue = sinf(angle);

        outMat.m[0][0] = cosValue;
        outMat.m[0][1] = -sinValue;
        outMat.m[1][0] = sinValue;
        outMat.m[1][1] = cosValue;

        return;
    }

    void MatrixRotationX(Matrix4& outMat, const float angle)
    {
        outMat.SetIdentity();
        float cosValue = cosf(angle);
        float sinValue = sinf(angle);

        outMat.m[1][1] = cosValue;
        outMat.m[1][2] = -sinValue;
        outMat.m[2][1] = sinValue;
        outMat.m[2][2] = cosValue;

        return;
    }

    void MatrixRotationY(Matrix4& outMat, const float angle)
    {
        outMat.SetIdentity();
        float cosValue = cosf(angle);
        float sinValue = sinf(angle);

        outMat.m[0][0] = cosValue;
        outMat.m[0][2] = sinValue;
        outMat.m[2][0] = -sinValue;
        outMat.m[2][2] = cosValue;

        return;
    }
}
