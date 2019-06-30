#include "Numerical.hpp"

namespace Panda
{
    void TransformCoord(Vector3Df& inVec, const Matrix4f& inMat)
    {
		Vector4Df temp({ inVec[0], inVec[1], inVec[2], 1.0f });
        TransformCoord(temp, inMat);
        
		inVec.Set({ temp[0], temp[1], temp[2] });

        return;
    }

    void TransformCoord(Vector4Df& inVec, const Matrix4f& inMat)
    {
        Vector4Df temp;
		temp.Set({ inVec[0] * inMat.m[0][0] + inVec[1] * inMat.m[1][0] + inVec[2] * inMat.m[2][0] + inVec[3] * inMat.m[3][0],
			inVec[0] * inMat.m[0][1] + inVec[1] * inMat.m[1][1] + inVec[2] * inMat.m[2][1] + inVec[3] * inMat.m[3][1],
			inVec[0] * inMat.m[0][2] + inVec[1] * inMat.m[1][2] + inVec[2] * inMat.m[2][2] + inVec[3] * inMat.m[3][2],
			inVec[0] * inMat.m[0][3] + inVec[1] * inMat.m[1][3] + inVec[2] * inMat.m[2][3] + inVec[3] * inMat.m[3][3] });
        inVec = temp;
        return;
    }

    void MatrixScale(Matrix4f& outMat, const Vector3Df& vec)
    {
        outMat.SetIdentity();
        outMat.m[0][0] = vec.data[0];
        outMat.m[1][1] = vec.data[1];
        outMat.m[2][2] = vec.data[2];
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

    void MatrixScale(Matrix4f& outMat, const float scalar)
    {
        outMat.SetIdentity();
        outMat.m[0][0] = outMat.m[1][1] = outMat.m[2][2] = scalar;
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
        outMat.m[3][0] = inVec[0];
        outMat.m[3][1] = inVec[1];
        outMat.m[3][2] = inVec[2];

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
        outMat.m[0][0] = c + inVec[0] * inVec[0] * one_minus_c;
        outMat.m[0][1] = s * inVec[2] + inVec[0] * inVec[1] * one_minus_c;
        outMat.m[0][2] = -s * inVec[1] + inVec[0] * inVec[2] * one_minus_c;

        outMat.m[1][0] = -s * inVec[2] + inVec[0] * inVec[1] * one_minus_c;
        outMat.m[1][1] = c + inVec[1] * inVec[1] * one_minus_c;
        outMat.m[1][2] = s * inVec[0] + inVec[1] * inVec[2] * one_minus_c;

        outMat.m[2][0] = s * inVec[1] + inVec[0] * inVec[2] * one_minus_c;
        outMat.m[2][1] = -s * inVec[0] + inVec[1] * inVec[2] * one_minus_c;
        outMat.m[2][2] = c + inVec[2] * inVec[2] * one_minus_c;

        return;
    }

    void MatrixRotationQuaternion(Matrix4f& outMat, const Quaternion& q)
    {
        outMat.SetIdentity();

        outMat.m[0][0] = 1.f - 2.f * q[1] * q[1] - 2.f * q[2] * q[2];
        outMat.m[0][1] = 2.f * q[0] * q[1] + 2.f * q[3] * q[2];
        outMat.m[0][2] = 2.f * q[0] * q[2] - 2.f * q[3] * q[1];

        outMat.m[1][0] = 2.f * q[0] * q[1] - 2.f * q[3] * q[2];
        outMat.m[1][1] = 1.f - 2.f * q[0] * q[0] - 2.f * q[2] * q[2];
        outMat.m[1][2] = 2.f * q[1] * q[2] + 2.f * q[3] * q[0];

        outMat.m[2][0] = 2.f * q[0] * q[2] + 2.f * q[3] * q[1];
        outMat.m[2][1] = 2.f * q[1] * q[2] - 2.f * q[3] * q[0];
        outMat.m[2][2] = 1.f - 2.f * q[0] * q[0] - 2.f * q[1] * q[1];
        
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
		n = Normalize(n);
        Vector3Df u = CrossProduct(up, n);
        u = Normalize(u);
        Vector3Df v = CrossProduct(n, u);

        result.SetIdentity();
        result.m[0][0] = u[0];
        result.m[0][1] = v[0];
        result.m[0][2] = n[0];

        result.m[1][0] = u[1];
        result.m[1][1] = v[1];
        result.m[1][2] = n[1];

        result.m[2][0] = u[2];
        result.m[2][1] = v[2];
        result.m[2][2] = n[2];

        result.m[3][0] = -DotProduct(pos, u);
        result.m[3][1] = -DotProduct(pos, v);
        result.m[3][2] = -DotProduct(pos, n);

        return;
    }

    void BuildViewMatrixLH(Matrix4f& result, const Vector3Df& pos, const Vector3Df& target, const Vector3Df& up)
    {
        Vector3Df n = target - pos;
        n = Normalize(n);
        Vector3Df u = CrossProduct(up, n);
        u = Normalize(u);
        Vector3Df v = CrossProduct(n, u);

        result.SetIdentity();
        result.m[0][0] = u[0];
        result.m[0][1] = v[0];
        result.m[0][2] = n[0];

        result.m[1][0] = u[1];
        result.m[1][1] = v[1];
        result.m[1][2] = n[1];

        result.m[2][0] = u[2];
        result.m[2][1] = v[2];
        result.m[2][2] = n[2];

        result.m[3][0] = -DotProduct(pos, u);
        result.m[3][1] = -DotProduct(pos, v);
        result.m[3][2] = -DotProduct(pos, n);

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
        result.Set(0);

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
            result.m[2][2] = (near + far) / (near - far);
            result.m[3][2] = (2 * near * far) / (near - far);
        }
        
        return;
    }

    void BuildPerspectiveFovLHMatrix(Matrix4f& result, const float fov, const float aspect, const float near, const float far)
    {
        result.Set(0);

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