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
    template<typename T>
    struct Matrix3
    {
    public:
        T m[3][3];

        FORCEINLINE Matrix3<T>() 
        {
        }

        FORCEINLINE Matrix3<T>(const Matrix3<T>& inMat)
        {
            for (int i = 0; i < 3; ++i)
                for (int j = 0; j < 3; ++j)
                    m[i][j] = inMat.m[i][j];
        }

        FORCEINLINE Matrix3<T>(const Vector3D<T>& in1, const Vector3D<T>& in2, const Vector3D<T>& in3)
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

        FORCEINLINE Matrix3<T>& operator= (const Matrix3<T>& inMat)
        {
            for (int i = 0; i < 3; ++i)
                for (int j = 0; j < 3; ++j)
                    m[i][j] = inMat.m[i][j];

            return *this;
        }

        FORCEINLINE Matrix3<T>& operator+= (const Matrix3<T>& inMat)
        {
            for (int i = 0; i < 3; ++i)
                for (int j = 0; j < 3; ++j)
                    m[i][j] += inMat.m[i][j];

            return *this;
        }

        FORCEINLINE Matrix3<T>& operator-= (const Matrix3<T>& inMat)
        {
            for (int i = 0; i < 3; ++i)
                for (int j = 0; j < 3; ++j)
                    m[i][j] -= inMat.m[i][j];

            return *this;
        }

        FORCEINLINE Matrix3<T> operator+ (const Matrix3<T>& inMat)
        {
            Matrix3<T> result;
            for (int i = 0; i < 3; ++i)
                for (int j = 0; j < 3; ++j)
                    result.m[i][j] = m[i][j] + inMat.m[i][j];

            return result;
        }

        FORCEINLINE Matrix3<T> operator- (const Matrix3<T>& inMat)
        {
            Matrix3<T> result;
            for (int i = 0; i < 3; ++i)
                for (int j = 0; j < 3; ++j)
                    result.m[i][j] = m[i][j] - inMat.m[i][j];

            return result;
        }

        FORCEINLINE Matrix3<T> operator* (float scale)
        {
            Matrix3<T> result;
            for (int i = 0; i < 3; ++i)
                for (int j = 0; j < 3; ++j)
                    result.m[i][j] = m[i][j] * scale;
            return result;
        }

        FORCEINLINE Matrix3<T>& operator*= (float scale)
        {
            for (int i = 0; i < 3; ++i)
                for (int j = 0; j < 3; ++j)
                    m[i][j] *= scale;
            return *this;
        }

        FORCEINLINE Matrix3<T> operator* (const Matrix3<T>& inMat)
        {
            Matrix3<T> result;
            for (int i = 0; i < 3; ++i)
                for (int j = 0; j < 3; ++j)
                    for (int k = 0; k < 3; ++k)
                        result.m[i][j] += m[i][k] * inMat.m[k][j];
            return result;
        }

        FORCEINLINE Matrix3<T>& operator*= (const Matrix3<T>& inMat)
        {
            Matrix3<T> result = (*this) * inMat;
            *this = result;
            return *this;
        }

        FORCEINLINE Matrix3<T> operator/ (float scale)
        {
            Matrix3<T> result;
            float rScale = 1.f / scale;
            for (int i = 0; i < 3; ++i)
                for (int j = 0; j < 3; ++j)
                    result.m[i][j] = m[i][j] * rScale;
            return result;
        }

        FORCEINLINE Matrix3<T>& operator/= (float scale)
        {
            float rScale = 1.f / scale;
            for (int i = 0; i < 3; ++i)
                for (int j = 0; j < 3; ++j)
                    m[i][j] *= rScale;
            return *this;
        }

        FORCEINLINE bool operator== (const Matrix3<T>& inMat)
        {
            for (int i = 0; i < 3; ++i)
                for (int j = 0; j < 3; ++j)
                    if (m[i][j] != inMat.m[i][j])
                        return false;
            return true;
        }

        friend std::ostream& operator<< (std::ostream& out, const Matrix3<T>& mat)
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

    typedef Matrix3<float> Matrix3f;
}

