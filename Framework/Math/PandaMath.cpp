#include "PandaMath.hpp"

namespace Panda
{
    void TransformCoord(Vector3Df& inVec, const Matrix4f& inMat)
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

    void MatrixTranspose(Matrix4f& outMat, const Matrix4f& inMat)
    {
        outMat = inMat;
        outMat.SetTransposed();

        return;
    }

    void MatrixScale(Matrix4f& outMat, const float x, const float y, const float z)
    {
        outMat.SetIdentity();

        outMat.m[0][0] = x;
        outMat.m[1][1] = y;
        outMat.m[2][2] = z;

        return;
    }

    void MatrixTranslation(Matrix4f& outMat, float x, float y, float z)
    {
        outMat.SetIdentity();
        outMat.m[3][0] = x;
        outMat.m[3][1] = y;
        outMat.m[3][2] = z;

        return;
    }

    void MatrixTranslation(Matrix4f& outMat, const Vector3Df& inVec)
    {
        outMat.SetIdentity();
        outMat.m[3][0] = inVec.x;
        outMat.m[3][1] = inVec.y;
        outMat.m[3][2] = inVec.z;

        return;
    }

    void MatrixRotationZ(Matrix4f& outMat, const float angle)
    {
        outMat.SetIdentity();

        float cosValue = cosf(angle);
        float sinValue = sinf(angle);

        outMat.m[0][0] = cosValue;
        outMat.m[0][1] = sinValue;
        outMat.m[1][0] = -sinValue;
        outMat.m[1][1] = cosValue;

        return;
    }

    void MatrixRotationX(Matrix4f& outMat, const float angle)
    {
        outMat.SetIdentity();
        float cosValue = cosf(angle);
        float sinValue = sinf(angle);

        outMat.m[1][1] = cosValue;
        outMat.m[1][2] = sinValue;
        outMat.m[2][1] = -sinValue;
        outMat.m[2][2] = cosValue;

        return;
    }

    void MatrixRotationY(Matrix4f& outMat, const float angle)
    {
        outMat.SetIdentity();
        float cosValue = cosf(angle);
        float sinValue = sinf(angle);

        outMat.m[0][0] = cosValue;
        outMat.m[0][2] = -sinValue;
        outMat.m[2][0] = sinValue;
        outMat.m[2][2] = cosValue;

        return;
    }

	// yaw - y, pitch - x, roll - z
	// order: yaw -> pitch ->roll
    void MatrixRotationYawPitchRoll(Matrix4f& outMat, const float yaw, const float pitch, const float roll)
    {
        outMat.SetIdentity();

        float cYaw, cPitch, cRoll, sYaw, sPitch, sRoll;

        cYaw = cosf(yaw);
        cPitch = cosf(pitch);
        cRoll = cosf(roll);

        sYaw = sinf(yaw);
        sPitch = sinf(pitch);
        sRoll = sinf(roll);

        outMat.m[0][0] = cRoll * cYaw + sRoll * sPitch * sYaw;
        outMat.m[0][1] = sRoll * cPitch;
        outMat.m[0][2] = -cRoll * sYaw + sRoll * sPitch * cYaw;

        outMat.m[1][0] = -sRoll * cYaw + cRoll * sYaw * sPitch;
        outMat.m[1][1] = cPitch * cRoll;
        outMat.m[1][2] = sRoll * sYaw + cRoll * sPitch * cYaw;

        outMat.m[2][0] = cPitch * sYaw;
        outMat.m[2][1] = -sPitch;
        outMat.m[2][2] = cYaw * cPitch;

        return;
    }

    void MatrixRotationAxis(Matrix4f& outMat, const Vector3Df& inVec, const float angle)
    {
        outMat.SetIdentity();

        float c = cosf(angle);
        float s = sinf(angle);
        float one_minus_c = 1.f - c;
        outMat.m[0][0] = c + inVec.x * inVec.x * one_minus_c;
        outMat.m[0][1] = s * inVec.z + inVec.x * inVec.y * one_minus_c;
        outMat.m[0][2] = -s * inVec.y + inVec.x * inVec.z * one_minus_c;

        outMat.m[1][0] = -s * inVec.z + inVec.x * inVec.y * one_minus_c;
        outMat.m[1][1] = c + inVec.y * inVec.y * one_minus_c;
        outMat.m[1][2] = s * inVec.x + inVec.y * inVec.z * one_minus_c;

        outMat.m[2][0] = s * inVec.y + inVec.x * inVec.z * one_minus_c;
        outMat.m[2][1] = -s * inVec.x + inVec.y * inVec.z * one_minus_c;
        outMat.m[2][2] = c + inVec.z * inVec.z * one_minus_c;

        return;
    }

    void MatrixRotationQuaternion(Matrix4f& outMat, const Quaternion& q)
    {
        outMat.SetIdentity();

        outMat.m[0][0] = 1.f - 2.f * q.y * q.y - 2.f * q.z * q.z;
        outMat.m[0][1] = 2.f * q.x * q.y + 2.f * q.w * q.z;
        outMat.m[0][2] = 2.f * q.x * q.z - 2.f * q.w * q.y;

        outMat.m[1][0] = 2.f * q.x * q.y - 2.f * q.w * q.z;
        outMat.m[1][1] = 1.f - 2.f * q.x * q.x - 2.f * q.z * q.z;
        outMat.m[1][2] = 2.f * q.y * q.z + 2.f * q.w * q.x;

        outMat.m[2][0] = 2.f * q.x * q.z + 2.f * q.w * q.y;
        outMat.m[2][1] = 2.f * q.y * q.z - 2.f * q.w * q.x;
        outMat.m[2][2] = 1.f - 2.f * q.x * q.x - 2.f * q.y * q.y;
        
        return;
    }

    
    void BuildViewMatrix(Matrix4f& result, const Vector3Df& pos, const Vector3Df& target, const Vector3Df& up, Handness handness)
    {
        if (handness == Handness::kHandnessRight)
            BuildViewMatrixRH(result, pos, target, up);
        else
            BuildViewMatrixLH(result, pos, target, up);
        
        return;
    }

    void BuildViewMatrixRH(Matrix4f& result, const Vector3Df& pos, const Vector3Df& target, const Vector3Df& up)
    {
        Vector3Df n = pos - target;
        n.Normalize();
        Vector3Df u = up.CrossProduct(n);
        u.Normalize();
        Vector3Df v = n.CrossProduct(u);

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

        result.m[3][0] = -pos.DotProduct(u);
        result.m[3][1] = -pos.DotProduct(v);
        result.m[3][2] = -pos.DotProduct(n);

        return;
    }

    void BuildViewMatrixLH(Matrix4f& result, const Vector3Df& pos, const Vector3Df& target, const Vector3Df& up)
    {
        Vector3Df n = target - pos;
        n.Normalize();
        Vector3Df u = up.CrossProduct(n);
        u.Normalize();
        Vector3Df v = n.CrossProduct(u);

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

        result.m[3][0] = -pos.DotProduct(u);
        result.m[3][1] = -pos.DotProduct(v);
        result.m[3][2] = -pos.DotProduct(n);

        return;
    }

    void BuildPerspectiveFovMatrix(Matrix4f& result, const float fov, const float aspect, const float near, const float far, Handness handness)
    {
        if (handness == Handness::kHandnessRight)
            BuildPerspectiveFovRHMatrix(result, fov, aspect, near, far);
        else
            BuildPerspectiveFovLHMatrix(result, fov, aspect, near, far);

        return;
    }

    void BuildPerspectiveFovRHMatrix(Matrix4f& result, const float fov, const float aspect, const float near, const float far)
    {
        result.SetZero();

        float cFov = 1.f / tanf(fov / 2);
        result.m[0][0] = cFov / aspect;
        result.m[1][1] = cFov;
        result.m[2][3] = -1;

        if (g_DepthClipSpace == DepthClipSpace::kDepthClipZeroToOne)
        {
            result.m[2][2] = far / (near - far);
            result.m[3][2] = near * far / (near - far);
        }
        else /* g_DepthClipSpace == DepthClipSpace::kDepthClipNegativeOneToOne */
        {
            result.m[2][2] = (near + far) * (near - far);
            result.m[3][2] = (2 * near * far) / (near - far);
        }
        
        return;
    }

    void BuildPerspectiveFovLHMatrix(Matrix4f& result, const float fov, const float aspect, const float near, const float far)
    {
        result.SetZero();

        float cFov = 1.f / tanf(fov / 2.f);
        result.m[0][0] = cFov / aspect;
        result.m[1][1] = cFov;
        result.m[2][3] = 1.0f;

        if (g_DepthClipSpace == DepthClipSpace::kDepthClipZeroToOne)
        {
            result.m[2][2] = far / (far - near);
            result.m[3][2] = near * far / (near - far);
        }
        else
        {
            result.m[2][2] = (far + near) / (far - near);
            result.m[3][2] = (2 * near * far) / (near - far);
        }

        return;
    }
}