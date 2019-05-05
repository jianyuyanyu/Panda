#pragma once

#include <math.h>
#include <iostream>
#include "Utility.hpp"

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

        FORCEINLINE Vector3D() {}
        FORCEINLINE Vector3D(const Vector3D& inV) : x(inV.x), y(inV.y), z(inV.z) {}
        FORCEINLINE Vector3D(float _v): x(_v), y(_v), z(_v) {}
        FORCEINLINE Vector3D(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}

        FORCEINLINE Vector3D& operator= (const Vector3D& inV) 
        {
            x = inV.x;
            y = inV.y;
            z = inV.z;
            return *this;
        }

        FORCEINLINE Vector3D& operator+= (const Vector3D& inV)
        {
            x += inV.x;
            y += inV.y;
            z += inV.z;

            return *this;
        }

        FORCEINLINE Vector3D& operator-= (const Vector3D& inV)
        {
            x -= inV.x;
            y -= inV.y;
            z -= inV.z;
            return *this;
        }

        FORCEINLINE Vector3D operator+(const Vector3D& inV)
        {
            return Vector3D(x + inV.x, y + inV.y, z + inV.z);
        }

        FORCEINLINE Vector3D operator- (const Vector3D& inV) const 
        {
            return Vector3D(x - inV.x, y - inV.y, z - inV.z);
        }

        // negative
        FORCEINLINE Vector3D operator-() const
        {
            return Vector3D(-x, -y, -z);
        }

        FORCEINLINE Vector3D operator+(float bias)
        {
            return Vector3D(x + bias, y + bias, z + bias);
        }

        FORCEINLINE Vector3D operator-(float bias)
        {
            return Vector3D(x - bias, y - bias, z - bias);
        }

        FORCEINLINE Vector3D operator*(float scale)
        {
            return Vector3D(x * scale, y * scale, z * scale);
        }

        FORCEINLINE Vector3D operator/(float scale)
        {
            const float rScale = 1.f / scale;
            //return (*this) * rScale;
            return Vector3D(x * rScale, y * rScale, z * rScale);
        }

        FORCEINLINE float DotProduct(const Vector3D& inV) const
        {
            return x * inV.x + y * inV.y + z * inV.z;
        }

        FORCEINLINE Vector3D CrossProduct(const Vector3D& inV) const
        {
            return Vector3D(
                y * inV.z - z * inV.y, 
                z * inV.x - x * inV.z,
                x * inV.y - y * inV.x
                );
        }

        FORCEINLINE float Length()
        {
            return sqrtf(x * x + y * y + z * z);
        }

        FORCEINLINE float SquareLength()
        {
            return x * x + y * y + z * z;
        }

        FORCEINLINE void Normalize()
        {
            float len = Length();
            x /= len;
            y /= len;
            z /= len;
        }

        friend std::ostream& operator<< (std::ostream& out, const Vector3D& vec)
        {
			out << "(";

            out << vec.x << "," << vec.y << "," << vec.z;

            out << ")";

			return out;
        }
    };
}

