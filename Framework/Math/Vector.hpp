#pragma once
#include "MathUtility.hpp"
#include <vector>
#include <cassert>

/*
    A common vecotr which is not used in engine currently.
 */
namespace Panda
{
    template <typename T, int N>
    struct Vector
    {
        T data[N] = {0};

        Vector() {}
        Vector(const T val)
        {
            for (size_t i = 0; i < N; ++i)
                data[i] = val;
        }

        Vector(const std::vector<T>& vec)
        {
            size_t sv = vec.size();
            for (size_t i = 0; i < N && i < sv; ++i)
            {
                data[i] = vec[i];
            }
        }

        // Be careful when using this function.
        Vector(const T* list)
        {
            memcpy_s(data, sizeof(T) * N, list, sizeof(T) * N);
        }

        Vector(const Vector<T, N>& rhs)
        {
            memcpy_s(data, sizeof(T) * N, rhs.data, sizeof(T) * N);
        }

        void Set(const T val)
        {
            for (size_t i = 0; i < N; ++i)
                data[i] = val;
        }

        void Set(const T* list)
        {
            memcpy_s(data, sizeof(T) * N, list, sizeof(T) * N);
        }

        void Set(const std::vector<T>& vec)
        {
            size_t sv = vec.size();
            for (size_t i = 0; i < N && i < sv; ++i)
            {
                data[i] = vec[i];
            }
        }

        Vector& operator=(const T* list)
        {
            Set(list);
            return *this;
        }

        Vector& operator=(const std::vector<T>& vec)
        {
            Set(vec);
            return *this;
        }

        Vector& operator=(const Vector<T, N>& rhs)
        {
            memcpy_s(data, sizeof(T) * N, rhs.data, sizeof(T) * N);
            return *this;
        }

        // negative
        Vector<T, N> operator-()
        {
            Vector<T, N> result;
            for (size_t i = 0; i < N; ++i)
            {
                result.data[i] = -data[i];
            }
            return result;
        }

        T GetLength()
        {
            T sum = 0;
            for (int32_t i = 0; i < N; ++i)
            {
                sum += data[i] * data[i];
            }
            return std::sqrt(sum);
        }

        T GetLengthSquare()
        {
            T sum = 0;
            for (int32_t i = 0; i < N; ++i)
            {
                sum += data[i] * data[i];
            }
            return sum;
        }
    };

    template <typename T, int N>
    std::ostream& operator<<(std::ostream& out, const Vector<T, N>& vec)
    {
        out.precision(6);
        out.setf(std::ios::fixed);
        out << "( ";
        for(size_t i = 0; i < N; ++i)
            out << vec.data[i] << ((i == N -1)? "": ", ");
        out << " )" << std::endl;

        return out;
    }

    template<typename T, int N>
    Vector<T, N> operator+(const Vector<T, N>& vec1, const Vector<T, N>& vec2)
    {
        Vector<T, N> result;
        for (size_t i = 0; i < N; ++i)
        {
            result.data[i] = vec1.data[i] + vec2.data[i];
        }
        return result;
    }

    template <typename T, int N>
    Vector<T, N> operator-(const Vector<T, N>& vec1, const Vector<T, N>& vec2)
    {
        Vector<T, N> result;
        for (size_t i = 0; i < N; ++i)
        {
            result.data[i] = vec1.data[i] - vec2.data[i];
        }
        return result;
    }

    template <typename T, int N>
    Vector<T, N> operator*(const Vector<T, N>& vec, const float scaler)
    {
        Vector<T, N> result;
        for (size_t i = 0; i < N; ++i)
        {
            result.data[i] = vec.data[i] * scaler;
        }
        return result;
    }

    template <typename T, int N>
    Vector<T, N> operator*(const float scaler, const Vector<T, N>& vec)
    {
        Vector<T, N> result;
        for (size_t i = 0; i < N; ++i)
        {
            result.data[i] = vec.data[i] * scaler;
        }
        return result;
    }

    template <typename T, int N>
    Vector<T, N> operator/(const Vector<T, N>& vec, const float scaler)
    {
        assert(scaler != 0.0f);
        float t = 1.0f / scaler;
        return vec * t;
    }

	template <typename T, int N>
	T DotProduct(const Vector<T, N>& vec1, const Vector<T, N>& vec2)
	{
		T result = 0.0;
		for (size_t i = 0; i < N; ++i)
			result += vec1.data[i] * vec2.data[i];
		return result;
	}
}