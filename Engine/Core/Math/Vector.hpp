#pragma once
#include "MathUtility.hpp"
#include <vector>
#include <cassert>

namespace Panda
{
    template <typename T, int N>
    struct Vector
    {
        T data[N] = {0};

        Vector() = default;
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

        operator T*() {
            return reinterpret_cast<T*>(this);
        }

        operator const T*() const 
        {
            return reinterpret_cast<const T*>(this);
        }

        T operator[] (int32_t index)
        {
            return data[index];
        }

        const T operator[](int32_t index) const
        {
            return data[index];
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

        Vector& operator+=(T scalar)
        {
            for (int32_t i = 0; i < N; ++i)
                data[i] += scalar;
            return *this;
        }

        Vector& operator+=(const Vector<T, N>& rhs)
        {
            for (int32_t i = 0; i < N; ++i)
                data[i] = rhs[i];
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

		int32_t GetAbsMaxElementIndex()
		{
			int32_t index = 0;
			T value = 0;

			for (int32_t i = 0; i < N; ++i)
			{
				if (std::abs(data[i]) > value) {
					value = std::abs(data[i]);
					index = i;
				}
			}

			return index;
		}
    };

    typedef Vector<float, 2> Vector2Df;
    typedef Vector<int16_t, 2> Pixel2D;
    
    typedef Vector<float, 3> Vector3Df;
    typedef Vector<double, 3> Vector3Dd;
    typedef Vector<int32_t, 3> Vector3Di;

    typedef Vector<float, 4> Vector4Df;
    typedef Vector<float, 4> Quaternion;
    typedef Vector<int32_t, 4> Vector4Di;
	typedef Vector<uint8_t, 4> R8G8B8A8Unorm;

    template <typename T, int N>
    std::ostream& operator<<(std::ostream& out, const Vector<T, N>& vec)
    {
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
    Vector<T, N> operator+(const Vector<T, N>& vec, T scalar)
    {
        Vector<T, N> result;
        for (size_t i = 0; i < N; ++i)
        {
            result[i] = vec[i] + scalar;
        }
        return result;
    }

    template <typename T, int N>
    Vector<T, N> operator+(T scalar, const Vector<T, N>& vec)
    {
        Vector<T, N> result;
        for (size_t i = 0; i < N; ++i)
        {
            result[i] = vec[i] + scalar;
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
    Vector<T, N> operator-(const Vector<T, N>& vec, T scalar)
    {
        Vector<T, N> result;
        for (size_t i = 0; i < N; ++i)
        {
            result[i] = vec[i] - scalar;
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

	template<typename T, int N>
	Vector<T, N> operator*(const Vector<T, N>& vec1, const Vector<T, N>& vec2)
	{
		Vector<T, N> result;
		for (size_t i = 0; i < N; ++i)
		{
			result.data[i] = vec1[i] * vec2[i];
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

	template<typename T, int N>
	Vector<T, N> operator/(const Vector<T, N>& vec1, const Vector<T, N>& vec2)
	{
		Vector<T, N> result;
		for (int32_t i = 0; i < N; ++i)
			result.data[i] = vec1[i] / vec2[i];
		return result;
	}

    template <typename T, int N>
    bool operator>(const Vector<T, N>& vec, const float scalar)
    {
        return GetLength(vec) > scalar;
    }

    template <typename T, int N>
    bool operator>=(const Vector<T, N>& vec, const float scalar)
    {
        return GetLength(vec) >= scalar;
    }

    template <typename T, int N>
    bool operator<(const Vector<T, N>& vec, const float scalar)
    {
        return GetLength(vec) < scalar;
    }

    template <typename T, int N>
    bool operator<=(const Vector<T, N>& vec, const float scalar)
    {
        return GetLength(vec) <= scalar;
    }

	template <typename T>
	T pow(const T base, const float exponent)
	{
		return std::pow(base, exponent);
	}

	template<typename T, int N>
	Vector<T, N> pow(const Vector<T, N>& vec, const float exponent)
	{
		Vector<T, N> result;

		for (int32_t i = 0; i < N; ++i)
		{
			result.data[i] = pow(vec[i], exponent);
		}

		return result;
	}

	template <typename T>
	T abs(const T data)
	{
		return std::abs(data);
	}

	template <typename T, int N>
	Vector<T, N> abs(const Vector<T, N>& vec)
	{
		Vector<T, N> result;
		for (int32_t i = 0; i < N; ++i)
			result.data[i] = abs(vec[i]);
		return result;
	}

	template <typename T, int N>
	T DotProduct(const Vector<T, N>& vec1, const Vector<T, N>& vec2)
	{
		T result = 0.0;
		for (size_t i = 0; i < N; ++i)
			result += vec1.data[i] * vec2.data[i];
		return result;
	}

    template <typename T>
    Vector<T, 3> CrossProduct(const Vector<T, 3>& vec1, const Vector<T, 3>& vec2)
    {
        Vector<T, 3> result;
        
		result.Set({ vec1[1] * vec2[2] - vec1[2] * vec2[1],
			vec1[2] * vec2[0] - vec1[0] * vec2[2],
			vec1[0] * vec2[1] - vec1[1] * vec2[0] });

        return result;
    }
    template <typename T>
    T CrossProduct(const Vector<T, 2>& vec1, const Vector<T, 2>& vec2)
    {
        return vec1.data[0] * vec2.data[1] - vec1.data[1] * vec2.data[0];
    }

    template <typename T, int N>
    Vector<T, N> MulByElement(const Vector<T, N>& vec1, const Vector<T, N>& vec2)
    {
        Vector<T, N> result;
        for (int32_t i = 0; i < N; ++i)
            result.data[i] = vec1[i] * vec2[i];
        return result;
    }

	template <typename T, int N>
	Vector<T, N> Normalize(const Vector<T, N>& vec)
	{
		Vector<T, N> result(vec);
		result = result / GetLength(result);
		return result;
	}

    template <typename T, int N>
    T GetLength(const Vector<T, N>& vec)
    {
        T sum = 0;
        for (int32_t i = 0; i < N; ++i)
        {
            sum += vec[i] * vec[i];
        }
        return std::sqrt(sum);
    }

    template <typename T, int N>
    T GetLengthSquare(const Vector<T, N>& vec)
    {
        T sum = 0;
        for (int32_t i = 0; i < N; ++i)
        {
            sum += vec[i] * vec[i];
        }
        return sum;
    }
}