#pragma once

#include <math.h>
#include "Utility.hpp"

/**
 * A vector in 4D with components X, Y , Z , W
 */

namespace Panda
{
    struct Vector4D
    {
    public:
        float x;
        float y;
        float z;
        float w;

        FORCEINLINE Vector4D() {}
        FORCEINLINE Vector4D(const Vector4D& inV) : x(inV.x), y(inV.y), z(inV.z), w(inV.w) {}
        FORCEINLINE Vector4D(float _v): x(_v), y(_v), z(_v), w(_v) {}
        FORCEINLINE Vector4D(float _x, float _y, float _z, float _w) : x(_x), y(_y), z(_z), w(_w) {}

        FORCEINLINE Vector4D& operator= (const Vector4D& inV) 
        {
            x = inV.x;
            y = inV.y;
            z = inV.z;
            w = inV.w;
            return *this;
        }

        FORCEINLINE Vector4D& operator+= (const Vector4D& inV)
        {
            x += inV.x;
            y += inV.y;
            z += inV.z;
            w += inV.w;
            return *this;
        }

        FORCEINLINE Vector4D& operator-= (const Vector4D& inV)
        {
            x -= inV.x;
            y -= inV.y;
            z -= inV.z;
            w -= inV.w;
            return *this;
        }

        FORCEINLINE Vector4D operator+(const Vector4D& inV)
        {
            return Vector4D(x + inV.x, y + inV.y, z + inV.z, w + inV.w);
        }

        FORCEINLINE Vector4D operator- (const Vector4D& inV)
        {
            return Vector4D(x - inV.x, y - inV.y, z - inV.z, w - inV.w);
        }

        FORCEINLINE Vector4D operator+(float bias)
        {
            return Vector4D(x + bias, y + bias, z + bias, w + bias);
        }

        FORCEINLINE Vector4D operator-(float bias)
        {
            return Vector4D(x - bias, y - bias, z - bias, w - bias);
        }

        FORCEINLINE Vector4D operator*(float scale)
        {
            return Vector4D(x * scale, y * scale, z * scale, w * scale);
        }

        FORCEINLINE Vector4D operator/(float scale)
        {
            const float rScale = 1.f / scale;
            //return (*this) * rScale;
            return Vector4D(x * rScale, y * rScale, z * rScale, w * rScale);
        }

        FORCEINLINE float DotProduct(const Vector4D& inV)
        {
            return x * inV.x + y * inV.y + z * inV.z + w * inV.w;
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
}

