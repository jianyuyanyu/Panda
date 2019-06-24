#pragma once
#include "MathUtility.hpp"
#include "Vector.hpp"
#include <algorithm>

/*
    A common matrix which is not used in engine currently.
 */
namespace Panda
{
    template <typename T, int M, int N>
    struct Matrix
    {
		union {
			Vector<T, N> v[M];
			T data[M * N] = { 0 };
			T m[M][N];
		};

        Matrix() {}
        Matrix(const T val)
        {
            size_t mn = M * N;
            for (size_t i = 0; i < mn; ++i)
                data[i] = val;
        }
        Matrix(const std::vector<T>& vec)
        {
            size_t st = vec.size();
            size_t mn = M * N;
            for (size_t i = 0; i < st && i < mn; ++i)
            {
                data[i] = vec[i];
            }
        }

		Matrix(const Matrix& rhs)
		{
			memcpy(data, rhs.data, sizeof(T) * M * N);
		}
        // Be careful when using this function.
        Matrix(const T* list)
        {
            memcpy_s(data, sizeof(T) * M * N, list, sizeof(T) * M * N);
        }

		void Set(const T val)
		{
			size_t mn = M * N;
			for (size_t i = 0; i < mn; ++i)
				data[i] = val;
		}

		void SetRow(const Vector<T, N>& row, int32_t index)
		{
			v[index] = row;
		}

		void SetCol(const Vector<T, M>& col, int32_t index)
		{
			for (int32_t i = 0; i < M; ++i)
			{
				m[i][index] = col.data[i];
			}
		}

		void SetIdentity()
		{
			Set(0);
			int32_t n = std::min(M, N);
			for (int32_t i = 0; i < n; ++i)
				m[i][i] = 1;
		}

        Vector<T, N> GetRow(int32_t row) const
        {
            return v[row];
        }

        Vector<T, M> GetCol(int32_t col) const
        {
            Vector<T, M> result;
            for (int32_t i = 0; i < M; ++i)
            {
                result.data[i] = m[i][col];
            }
            return result;
        }

		Matrix& operator=(const Matrix& rhs)
		{
			memcpy(data, rhs.data, sizeof(T) * M * N);
			return *this;
		}
    };

    template <typename T, int M, int N>
    std::ostream& operator<<(std::ostream& out, Matrix<T, M, N>& matrix)
    {
        out << std::endl;
        out << "{ ";
        for (size_t i = 0; i < M; i++) {
            for (size_t j = 0; j < N; j++) {
                out << matrix.data[i * N + j] << ((j == N - 1)? (i != M - 1? "\n  " : "") : ", ");
            }
        }
        out << " }";
        out << std::endl;

        return out;
    }

    template <typename T, int M, int N>
    Matrix<T, M, N> operator+(const Matrix<T, M, N>& mat1, const Matrix<T, M, N>& mat2)
    {
        Matrix<T, M, N> result;
        for (size_t i = 0; i < M; ++i)
            for (size_t j = 0; j < N; ++j)
                result.data[i * N + j] = mat1.data[i * N + j] + mat2.data[i * N + j];
        return result;
    }

    template <typename T, int M, int N>
    Matrix<T, M, N> operator-(const Matrix<T, M, N>& mat1, const Matrix<T, M, N>& mat2)
    {
        Matrix<T, M, N> result;
        for (size_t i = 0; i < M; ++i)
            for (size_t j = 0; j < N; ++j)
                result.data[i * N + j] = mat1.data[i * N + j] - mat2.data[i * N + j];
        return result;
    }

    template <typename T, int M, int N>
    Matrix<T, M, N> operator*(const Matrix<T, M, N>& mat, const float scaler)
    {
        Matrix<T, M, N> result;
        for(size_t i = 0; i < M; ++i)
            for (size_t j = 0; j < N; ++j)
                result.data[i * N + j] = mat.data[i * N + j];
        return result;
    }

    template <typename T, int M, int N>
    Matrix<T, M, N> operator*(const float scaler, const Matrix<T, M, N>& mat)
    {
        return mat * scaler;
    }

    template <typename T, int M, int N>
    Matrix<T, M, N> operator/(const Matrix<T, M, N>& mat, const float scaler)
    {
        float inv = 1.0f / scaler;
        return mat * inv;
    }

    template <typename T, int M, int N, int R>
    Matrix<T, M, R> operator*(const Matrix<T, M, N>& mat1, const Matrix<T, N, R>& mat2)
    {
        Matrix<T, M, R> result;
        for (size_t i = 0; i < M; ++i)
            for (size_t k = 0; k < R; ++k)
                for (size_t j = 0; j < N; ++j)
                    result.data[i * R + k] += mat1.data[i * N + j] * mat2.data[j * R + k];
        return result;
    }

	template <typename T, int M, int N>
	Vector<T, M> operator*(const Matrix<T, M, N>& mat, const Vector<T, N>& vec)
	{
		Vector<T, M> result;
		
		for (int32_t i = 0; i < M; ++i)
		{
			T sum = 0;
			for (int32_t j = 0; j < N; ++j)
			{
				sum += mat.m[i][j] * vec.data[j];
			}
			result.data[i] = sum;
		}

		return result;
	}
}