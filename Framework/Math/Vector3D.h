#pragma once

#include <math.h>

/**
 * A vector in 3D with components X, Y, Z   
 */

namespace Panda
{
    struct Vector3D
    {
    public:
        float x;
        float y;
        float z;

        __forceinline Vector3D() {};
        __forceinline Vector3D(const Vector3D& inV) : x(inV.x), y(inV.y), z(inV.z) {}
        __forceinline Vector3D(float _v): x(_v), y(_v), z(_v) {}
        __forceinline Vector3D(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}

        __forceinline Vector3D& operator= (const Vector3D& inV) 
        {
            x = inV.x;
            y = inV.y;
            z = inV.z;
            return *this;
        }

        __forceinline Vector3D& operator+= (const Vector3D& inV)
        {
            x += inV.x;
            y += inV.y;
            z += inV.z;

            return *this;
        }

        __forceinline Vector3D& operator-= (const Vector3D& inV)
        {
            x -= inV.x;
            y -= inV.y;
            z -= inV.z;
            return *this;
        }

        __forceinline Vector3D operator+(const Vector3D& inV)
        {
            return Vector3D(x + inV.x, y + inV.y, z + inV.z);
        }

        __forceinline Vector3D operator- (const Vector3D& inV)
        {
            return Vector3D(x - inV.x, y - inV.y, z - inV.z);
        }

        __forceinline Vector3D operator+(float bias)
        {
            return Vector3D(x + bias, y + bias, z + bias);
        }

        __forceinline Vector3D operator-(float bias)
        {
            return Vector3D(x - bias, y - bias, z - bias);
        }

        __forceinline Vector3D operator*(float scale)
        {
            return Vector3D(x * scale, y * scale, z * scale);
        }

        __forceinline Vector3D operator/(float scale)
        {
            const float rScale = 1.f / scale;
            //return (*this) * rScale;
            return Vector3D(x * rScale, y * rScale, z * rScale);
        }

        __forceinline float DotProduct(const Vector3D& inV)
        {
            return x * inV.x + y * inV.y + z * inV.z;
        }

        __forceinline Vector3D CrossProduct(const Vector3D& inV)
        {
            return Vector3D(
                y * inV.z - z * inV.y, 
                z * inV.x - x * inV.z,
                x * inV.y - y * inV.x
                );
        }

        __forceinline float Length()
        {
            return sqrtf(x * x + y * y + z * z);
        }

        __forceinline float SquareLength()
        {
            return x * x + y * y + z * z;
        }
    };
}

