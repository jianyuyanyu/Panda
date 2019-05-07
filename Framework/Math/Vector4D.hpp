#pragma once

#include <math.h>
#include "Utility.hpp"

/**
 * A vector in 4D with components X, Y , Z , W
 */

namespace Panda
{
    template <typename T>
    struct Vector4D
    {
    public:
        union {
            T data[4];
			struct { T x, y, z, w; };
			struct { T r, g, b, a; };
        };

        FORCEINLINE Vector4D<T>() {}
        FORCEINLINE Vector4D<T>(const Vector4D<T>& inV) : x(inV.x), y(inV.y), z(inV.z), w(inV.w) {}
        FORCEINLINE Vector4D<T>(T _v): x(_v), y(_v), z(_v), w(_v) {}
        FORCEINLINE Vector4D<T>(T _x, T _y, T _z, T _w) : x(_x), y(_y), z(_z), w(_w) {}

        FORCEINLINE Vector4D<T>& operator= (const Vector4D<T>& inV) 
        {
            x = inV.x;
            y = inV.y;
            z = inV.z;
            w = inV.w;
            return *this;
        }

        FORCEINLINE Vector4D<T>& operator= (const T* array)
        {
            x = array[0];
            y = array[1];
            z = array[2];
            w = array[3];
            return *this;
        }

        FORCEINLINE Vector4D<T>& operator+= (const Vector4D<T>& inV)
        {
            x += inV.x;
            y += inV.y;
            z += inV.z;
            w += inV.w;
            return *this;
        }

        FORCEINLINE Vector4D<T>& operator-= (const Vector4D<T>& inV)
        {
            x -= inV.x;
            y -= inV.y;
            z -= inV.z;
            w -= inV.w;
            return *this;
        }

        FORCEINLINE Vector4D<T> operator+(const Vector4D<T>& inV)
        {
            return Vector4D<T>(x + inV.x, y + inV.y, z + inV.z, w + inV.w);
        }

        FORCEINLINE Vector4D<T> operator- (const Vector4D<T>& inV)
        {
            return Vector4D<T>(x - inV.x, y - inV.y, z - inV.z, w - inV.w);
        }

        FORCEINLINE Vector4D<T> operator*(float scale)
        {
            return Vector4D<T>(x * scale, y * scale, z * scale, w * scale);
        }

        FORCEINLINE Vector4D<T> operator/(float scale)
        {
            const float rScale = 1.f / scale;
            //return (*this) * rScale;
            return Vector4D<T>(x * rScale, y * rScale, z * rScale, w * rScale);
        }

        FORCEINLINE float DotProduct(const Vector4D<T>& inV)
        {
            return x * inV.x + y * inV.y + z * inV.z + w * inV.w;
        }
        
        // Mul product use for Quaternion multiply Quaternion
        FORCEINLINE Vector4D<T> MulProduct(const Vector4D<T>& inV)
        {
            Vector4D<T> result;
            result.w = w * inV.w - x * inV.x - y * inV.y - z * inV.z;
            result.x = w * inV.x + x * inV.w + y * inV.z - z * inV.y;
            result.y = w * inV.y - x * inV.z + y * inV.w + z * inV.x;
            result.z = w * inV.z + x * inV.y - y * inV.x + z * inV.w;
            return result;
        }

        FORCEINLINE float Length()
        {
            return sqrtf(x * x + y * y + z * z + w * w);
        }

        FORCEINLINE float SquareLength()
        {
            return x * x + y * y + z * z + w * w;
        }

        FORCEINLINE void Normalize()
        {
            float len = Length();
            x /= len;
            y /= len;
            z /= len;
            w /= len;
        }
    };

    typedef Vector4D<uint32_t>  Vector4Di;
    typedef Vector4D<float>     Vector4Df;
    typedef Vector4D<uint8_t>   ColorRGBAi;
    typedef Vector4D<float>     ColorRGBAf;
    typedef Vector4D<float>     Quaternion;
}

