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
            m[0][0] = m[0][1] = m[0][2] = 0;
            m[1][0] = m[1][1] = m[1][2] = 0;
            m[2][0] = m[2][1] = m[2][2] = 0;
        }

        FORCEINLINE Matrix3<T>(const Vector3D<T>& in1, const Vector3D<T>& in2, const Vector3D<T>& in3)
        {
            m[0][0] = in1.x; m[0][1] = in1.y; m[0][2] = in1.z;
            m[1][0] = in2.x; m[1][1] = in2.y; m[1][2] = in2.z;
            m[2][0] = in3.x; m[2][1] = in3.y; m[2][2] = in3.z;
        }

        FORCEINLINE Matrix3<T>(const Matrix3<T>& inMat)
        {
            for (int i = 0; i < 3; ++i)
                for (int j = 0; j < 3; ++j)
                    m[i][j] = inMat.m[i][j];
        }

        void SetIdentity() 
        {
            m[0][0] = 1; m[0][1] = 0; m[0][2] = 0;
            m[1][0] = 0; m[1][1] = 1; m[1][2] = 0;
            m[2][0] = 0; m[2][1] = 0; m[2][2] = 1;
        }

        void SetTransposed()
        {
            for (size_t i = 0; i < 3; ++i)
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

            T det = GetDeterminant();
			Matrix3<T> mat(*this);
			m[0][0] = mat.m[1][1] * mat.m[2][2] - mat.m[1][2] * mat.m[2][1];
			m[0][1] = mat.m[0][2] * mat.m[2][1] - mat.m[0][1] * mat.m[2][2];
			m[0][2] = mat.m[0][1] * mat.m[1][2] - mat.m[0][2] * mat.m[1][1];

			m[1][0] = mat.m[1][2] * mat.m[2][0] - mat.m[1][0] * mat.m[2][2];
			m[1][1] = mat.m[0][0] * mat.m[2][2] - mat.m[0][2] * mat.m[2][0];
			m[1][2] = mat.m[0][2] * mat.m[1][0] - mat.m[0][0] * mat.m[1][2];

			m[2][0] = mat.m[1][0] * mat.m[2][1] - mat.m[1][1] * mat.m[2][0];
			m[2][1] = mat.m[0][1] * mat.m[2][0] - mat.m[0][0] * mat.m[2][1];
			m[2][2] = mat.m[0][0] * mat.m[1][1] - mat.m[0][1] * mat.m[1][0];

			(*this) /= det;
            return true;
        }

		// 行列式
        T GetDeterminant()
        {
            return m[0][0] * (m[1][1] * m[2][2] - m[1][2] * m[2][1]) 
                  -m[0][1] * (m[1][0] * m[2][2] - m[1][2] * m[2][0])
                  +m[0][2] * (m[1][0] * m[2][1] - m[1][1] * m[2][0]);
        }

		// 代数余子式
        T GetCofactor(int32_t row, int32_t col)
        {
            int32_t coefficient = 1;
            if((row + col) % 2 == 1)
                coefficient = -1;

            T data[2][2] = {0};
            int32_t mi = 0;
            for (int32_t i = 0; i < 3; ++i)
            {
                if (i == row)
                    continue;

                int32_t mj = 0;
                for (int32_t j = 0; j < 3; ++j)
                {
                    if (j == col)
                        continue;
                    data[mi][mj] = m[i][j];
                    ++mj;
                }
                ++mi;
            }

            return coefficient * (data[0][0] * data[1][1] - data[0][1] * data[1][0]);
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

        FORCEINLINE T* GetAddressOf()
        {
            return &m[0][0];
        }

        FORCEINLINE const T* GetAddressOf() const
        {
            return &m[0][0];
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

