#include "PandaMath.h"

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

    void MatrixTranslation(Matrix4& outMat, const Vector3D& inVec)
    {
        outMat.SetIdentity();
        outMat.m[0][3] = inVec.x;
        outMat.m[1][3] = inVec.y;
        outMat.m[2][3] = inVec.z;

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

    void MatrixRotationYawPitchRoll(Matrix4& outMat, const float yaw, const float pitch, const float roll)
    {
        outMat.SetIdentity();

        float cYaw, cPitch, cRoll, sYaw, sPitch, sRoll;

        cYaw = cosf(yaw);
        cPitch = cosf(pitch);
        cRoll = cosf(roll);

        sYaw = sinf(yaw);
        sPitch = sinf(pitch);
        sRoll = sinf(roll);

        outMat.m[0][0] = (cRoll * cYaw) + (sRoll * sPitch * sYaw);
        outMat.m[0][1] = (-sRoll * cYaw) + (cRoll * sPitch * sYaw);
        outMat.m[0][2] = (sYaw * cPitch);

        outMat.m[1][0] = (cPitch * sRoll);
        outMat.m[1][1] = (cPitch * cRoll);
        outMat.m[1][2] = -sPitch;

        outMat.m[2][0] = (-sYaw * cRoll) + (cYaw * sPitch * sRoll);
        outMat.m[2][1] = (sYaw * sRoll) + (cYaw * sPitch * cRoll);
        outMat.m[2][2] = (cYaw * cPitch);

        return;
    }

    void MatrixRotation(Matrix4& outMat, const Vector3D& inVec, const float angle)
    {
        outMat.SetIdentity();

        float c = cosf(angle);
        float s = sinf(angle);
        outMat.m[0][0] = c + inVec.x * inVec.x * (1 - c);
        outMat.m[0][1] = inVec.x * inVec.y * (1 - c) - inVec.z * s;
        outMat.m[0][2] = inVec.x * inVec.z * (1 - c) + inVec.y * s;
        outMat.m[1][0] = inVec.z * s + inVec.x * inVec.y * (1 - c);
        outMat.m[1][1] = c + inVec.y * inVec.y * (1 - c);
        outMat.m[1][2] = inVec.y * inVec.z * (1 - c) - inVec.x * s;
        outMat.m[2][0] = inVec.x * inVec.z * (1 - c) - inVec.y * s;
        outMat.m[2][1] = inVec.x * s + inVec.y * inVec.z * (1 - c);
        outMat.m[2][2] = c + inVec.z * inVec.z * (1 - c);

        return;
    }

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

    // our uvn camera system is left-handed
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