#pragma once

#include <math.h>
#include <iostream>
#include "Vector3D.hpp"
#include "Utility.hpp"

/**
 * 3x3 matrix of floating point values.
 */

namespace Panda
{
    struct Matrix3
    {
    public:
        float m[3][3];

        __forceinline Matrix3() 
        {
            m[0][0] = m[0][1] = m[0][2] = 0;
            m[1][0] = m[1][1] = m[1][2] = 0;
            m[2][0] = m[2][1] = m[2][2] = 0;
        }

        __forceinline Matrix3(const Matrix3& inMat)
        {
            for (int i = 0; i < 3; ++i)
                for (int j = 0; j < 3; ++j)
                    m[i][j] = inMat.m[i][j];
        }

        __forceinline Matrix3(const Vector3D& in1, const Vector3D& in2, const Vector3D& in3)
        {
            m[0][0] = in1.x; m[0][1] = in1.y; m[0][2] = in1.z;
            m[1][0] = in2.x; m[1][1] = in2.y; m[1][2] = in2.z;
            m[2][0] = in3.x; m[2][1] = in3.y; m[2][2] = in3.z;
        }

        void SetIdentity() 
        {
            m[0][0] = 1; m[0][1] = 0; m[0][2] = 0;
            m[1][0] = 0; m[1][1] = 1; m[1][2] = 0;
            m[2][0] = 0; m[2][1] = 0; m[2][2] = 1;
        }

        __forceinline Matrix3& operator= (const Matrix3& inMat)
        {
            for (int i = 0; i < 3; ++i)
                for (int j = 0; j < 3; ++j)
                    m[i][j] = inMat.m[i][j];

            return *this;
        }

        __forceinline Matrix3& operator+= (const Matrix3& inMat)
        {
            for (int i = 0; i < 3; ++i)
                for (int j = 0; j < 3; ++j)
                    m[i][j] += inMat.m[i][j];

            return *this;
        }

        __forceinline Matrix3& operator-= (const Matrix3& inMat)
        {
            for (int i = 0; i < 3; ++i)
                for (int j = 0; j < 3; ++j)
                    m[i][j] -= inMat.m[i][j];

            return *this;
        }

        __forceinline Matrix3 operator+ (const Matrix3& inMat)
        {
            Matrix3 result;
            for (int i = 0; i < 3; ++i)
                for (int j = 0; j < 3; ++j)
                    result.m[i][j] = m[i][j] + inMat.m[i][j];

            return result;
        }

        __forceinline Matrix3 operator- (const Matrix3& inMat)
        {
            Matrix3 result;
            for (int i = 0; i < 3; ++i)
                for (int j = 0; j < 3; ++j)
                    result.m[i][j] = m[i][j] - inMat.m[i][j];

            return result;
        }

        __forceinline Matrix3 operator* (float scale)
        {
            Matrix3 result;
            for (int i = 0; i < 3; ++i)
                for (int j = 0; j < 3; ++j)
                    result.m[i][j] = m[i][j] * scale;
            return result;
        }

        __forceinline Matrix3& operator*= (float scale)
        {
            for (int i = 0; i < 3; ++i)
                for (int j = 0; j < 3; ++j)
                    m[i][j] *= scale;
            return *this;
        }

        __forceinline Matrix3 operator* (const Matrix3& inMat)
        {
            Matrix3 result;
            for (int i = 0; i < 3; ++i)
                for (int j = 0; j < 3; ++j)
                    for (int k = 0; k < 3; ++k)
                        result.m[i][j] += m[i][k] * inMat.m[k][j];
            return result;
        }

        __forceinline Matrix3& operator*= (const Matrix3& inMat)
        {
            Matrix3 result = (*this) * inMat;
            *this = result;
            return *this;
        }

        __forceinline Matrix3 operator/ (float scale)
        {
            Matrix3 result;
            float rScale = 1.f / scale;
            for (int i = 0; i < 3; ++i)
                for (int j = 0; j < 3; ++j)
                    result.m[i][j] = m[i][j] * rScale;
            return result;
        }

        __forceinline Matrix3& operator/= (float scale)
        {
            float rScale = 1.f / scale;
            for (int i = 0; i < 3; ++i)
                for (int j = 0; j < 3; ++j)
                    m[i][j] *= rScale;
            return *this;
        }

        __forceinline bool operator== (const Matrix3& inMat)
        {
            for (int i = 0; i < 3; ++i)
                for (int j = 0; j < 3; ++j)
                    if (m[i][j] != inMat.m[i][j])
                        return false;
            return true;
        }

        friend std::ostream& operator<< (std::ostream& out, const Matrix3& mat)
        {
            out << std::endl;

            out << "Matrix3:" << std::endl;
            for (int i = 0; i < 3; ++i)
            {
                for (int j = 0; j < 3; ++j)
                {
                    out << mat.m[i][j] << (j == 2? '\n' : ',');
                }                
            }

            out << std::endl;

            return out;
        }
    };
}

