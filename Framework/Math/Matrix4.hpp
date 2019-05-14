#pragma once

#include <math.h>
#include <iostream>
#include "Vector4D.hpp"
#include "Utility.hpp"

/**
 * 4x4 matrix of floating point values.
 */

namespace Panda
{
    template<typename T>
    struct Matrix4
    {
    public:
        T m[4][4];

        FORCEINLINE Matrix4<T>()
        {
            m[0][0] = m[0][1] = m[0][2] = m[0][3] = 0;
            m[1][0] = m[1][1] = m[1][2] = m[1][3] = 0;
            m[2][0] = m[2][1] = m[2][2] = m[2][3] = 0;
            m[3][0] = m[3][1] = m[3][2] = m[3][3] = 0;
        }

        FORCEINLINE Matrix4<T> (const Vector4D<T>& in1, const Vector4D<T>& in2, const Vector4D<T>& in3, const Vector4D<T>& in4)
        {
            m[0][0] = in1.x; m[0][1] = in1.y; m[0][2] = in1.z; m[0][3] = in1.w;
            m[1][0] = in2.x; m[1][1] = in2.y; m[1][2] = in2.z; m[1][3] = in2.w;
            m[2][0] = in3.x; m[2][1] = in3.y; m[2][2] = in3.z; m[2][3] = in3.w;
            m[3][0] = in4.x; m[3][1] = in4.y; m[3][2] = in4.z; m[3][3] = in4.w;
        }

        FORCEINLINE Matrix4<T> (T m00, T m01, T m02, T m03, T m10, T m11, T m12, T m13, T m20, T m21, T m22, T m23, T m30, T m31, T m32, T m33)
        {
            m[0][0] = m00; m[0][1] = m01; m[0][2] = m02; m[0][3] = m03;
            m[1][0] = m10; m[1][1] = m11; m[1][2] = m12; m[1][3] = m13;
            m[2][0] = m20; m[2][1] = m21; m[2][2] = m22; m[2][3] = m23;
            m[3][0] = m30; m[3][1] = m31; m[3][2] = m32; m[3][3] = m33;
        }

        FORCEINLINE Matrix4<T>(const Matrix4<T>& inMat)
        {
            for (int i = 0; i < 4; ++i)
                for (int j = 0; j < 4; ++j)
                    m[i][j] = inMat.m[i][j];
        }

        void SetZero()
        {
            m[0][0] = m[0][1] = m[0][2] = m[0][3] = 0;
            m[1][0] = m[1][1] = m[1][2] = m[1][3] = 0;
            m[2][0] = m[2][1] = m[2][2] = m[2][3] = 0;
            m[3][0] = m[3][1] = m[3][2] = m[3][3] = 0;
        }
        
        void SetIdentity() 
        {
            m[0][0] = 1; m[0][1] = 0; m[0][2] = 0; m[0][3] = 0;
            m[1][0] = 0; m[1][1] = 1; m[1][2] = 0; m[1][3] = 0;
            m[2][0] = 0; m[2][1] = 0; m[2][2] = 1; m[2][3] = 0;
            m[3][0] = 0; m[3][1] = 0; m[3][2] = 0; m[3][3] = 1;
        }

        void SetTransposed()
        {
            for (size_t i = 0; i < 4; ++i)
                for (size_t j = 0; j < i; ++j)
                {
                    T t = m[i][j];
                    m[i][j] = m[j][i];
                    m[j][i] = t;
                }
        }

        bool SetInverse()
        {
            if (GetDeterminant() == 0)
                return false;
            
            Matrix4<T> transpose(*this);
            transpose.SetTransposed();

            T det = GetDeterminant();
            for (int32_t i = 0; i < 4; ++i)
            {
                for (int32_t j = 0; j < 4; ++j)
                {
                    m[i][j] = transpose.GetCofactor(i, j) / det;
                }
            }
            return true;
        }

        // 行列式
        T GetDeterminant()
        {
            return  m[0][0] * (m[1][1] * (m[2][2] * m[3][3] - m[2][3] * m[3][2]) - m[1][2] * (m[2][1] * m[3][3] - m[2][3] * m[3][1]) + m[1][3] * (m[2][1] * m[3][2] - m[2][2] * m[3][1])) 
                  - m[0][1] * (m[1][0] * (m[2][2] * m[3][3] - m[2][3] * m[3][2]) - m[1][2] * (m[2][0] * m[3][3] - m[2][3] * m[3][0]) + m[1][3] * (m[2][0] * m[3][2] - m[2][2] * m[3][0]))
                  + m[0][2] * (m[1][0] * (m[2][1] * m[3][3] - m[2][3] * m[3][1]) - m[1][1] * (m[2][0] * m[3][3] - m[2][3] * m[3][0]) + m[1][3] * (m[2][0] * m[3][1] - m[2][1] * m[3][0]))
                  - m[0][3] * (m[1][0] * (m[2][1] * m[3][2] - m[2][2] * m[3][1]) - m[1][1] * (m[2][0] * m[3][2] - m[2][2] * m[3][0]) + m[1][2] * (m[2][0] * m[3][1] - m[2][1] * m[3][0]));
        }

        // 代数余子式
        T GetCofactor(int32_t row, int32_t col)
        {
            int32_t coefficient = 1;
            if((row + col) % 2 == 1)
                coefficient = -1;
            
            Matrix3<T> mat;
            int32_t mi = 0;
            for (int32_t i = 0; i < 4; ++i)
            {
                if (i == row)
                    continue;

                int32_t mj = 0;
                for (int32_t j = 0; j < 4; ++j)
                {
                    if (j == col)
                        continue;
                    mat.m[mi][mj] = m[i][j];
                    ++mj;
                }
                ++mi;
            }

            return coefficient * mat.GetDeterminant();
        }

        FORCEINLINE Matrix4<T> operator= (const Matrix4<T>& inMat)
        {
            for (int i = 0; i < 4; ++i)
                for (int j = 0; j < 4; ++j)
                    m[i][j] = inMat.m[i][j];
            return *this;
        }

        FORCEINLINE Matrix4<T> operator= (const T* pArray)
        {

            for (int i = 0; i < 4; ++i)
                for (int j = 0; j < 4; ++j)
                    m[i][j] = pArray[i * 4 + j];
            return *this;
        }

        FORCEINLINE Matrix4<T>& operator+= (const Matrix4<T>& inMat)
        {
            for (int i = 0; i < 4; ++i)
                for (int j = 0; j < 4; ++j)
                    m[i][j] += inMat.m[i][j];

            return *this;
        }

        FORCEINLINE Matrix4<T>& operator-= (const Matrix4<T>& inMat)
        {
            for (int i = 0; i < 4; ++i)
                for (int j = 0; j < 4; ++j)
                    m[i][j] -= inMat.m[i][j];
            
            return *this;
        }

        FORCEINLINE Matrix4<T> operator+ (const Matrix4<T>& inMat)
        {
            Matrix4<T> result;
            for (int i = 0; i < 4; ++i)
                for (int j = 0; j < 4; ++j)
                    result.m[i][j] = m[i][j] + inMat.m[i][j];

            return result;
        }

        FORCEINLINE Matrix4<T> operator- (const Matrix4<T>& inMat)
        {
            Matrix4<T> result;
            for (int i = 0; i < 4; ++i)
                for (int j = 0; j < 4; ++j)
                    result.m[i][j] = m[i][j] - inMat.m[i][j];

            return result;
        }

        FORCEINLINE Matrix4<T> operator* (float scale)
        {
            Matrix4<T> result;
            for (int i = 0; i < 4; ++i)
                for (int j = 0; j < 4; ++j)
                    result.m[i][j] *= scale;
            return result;
        }

        FORCEINLINE Matrix4<T>& operator*= (float scale)
        {
            for (int i = 0; i < 4; ++i)
                for (int j = 0; j < 4; ++j)
                    m[i][j] *= scale;
            return *this;
        }

        FORCEINLINE Matrix4<T> operator*(const Matrix4<T>& inMat)
        {
            Matrix4<T> result;
            for (int i = 0; i < 4; ++i)
                for (int j = 0; j < 4; ++j)
                    for (int k = 0; k < 4; ++k)
                        result.m[i][j] += m[i][k] * inMat.m[k][j];
            return result;
        }

        FORCEINLINE Matrix4<T>& operator*=(const Matrix4<T>& inMat)
        {
            Matrix4<T> result = (*this) * inMat;
            *this = result;
            return *this;
        }

        FORCEINLINE Matrix4<T> operator/ (float scale)
        {
            float rScale = 1.0f / scale;
            Matrix4<T> result;
            for (int i = 0; i < 4; ++i)
                for (int j = 0; j < 4; ++j)
                    result.m[i][j] = m[i][j] * rScale;
            return result;
        }

        FORCEINLINE Matrix4<T>& operator/= (float scale)
        {
            float rScale = 1.0f / scale;
            Matrix4<T> result;
            for (int i = 0; i < 4; ++i)
                for (int j = 0; j < 4; ++j)
                    m[i][j] *= rScale;
            return *this;
        }

        FORCEINLINE bool operator== (const Matrix4<T>& inMat)
        {
            for (int i = 0; i < 4; ++i)
                for (int j = 0; j < 4; ++j)
                    if (m[i][j] != inMat.m[i][j])
                        return false;
            return true;
        }

        FORCEINLINE T* GetAddressOf()
        {
            return &m[0][0];
        }

        FORCEINLINE const T* GetAddressOf() const
        {
            return &m[0][0];
        }

        friend std::ostream& operator<< (std::ostream& out, const Matrix4<T>& mat)
        {
            out << std::endl;

            out << "Matrix4:" << std::endl;
            for (int i = 0; i < 4; ++i)
            {
                for (int j = 0; j < 4; ++j)
                {
                    out << mat.m[i][j] << (j == 3? '\n' : ',');
                }                
            }

            out << std::endl;

            return out;
        }
    };

    typedef Matrix4<float> Matrix4f;
}

