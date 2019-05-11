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

