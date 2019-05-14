#pragma once

#include <math.h>
#include <iostream>
#include "Utility.hpp"

/**
 * A vector in 3D with components X, Y, Z   
 */

namespace Panda
{
    template<typename T>
    struct Vector3D
    {
    public:
        union 
        {
            T data[3];
			struct { T x, y, z; };
			struct { T r, g, b; };
        };

        FORCEINLINE Vector3D<T>() {x = y = z = 0;}
        FORCEINLINE Vector3D<T>(const Vector3D<T>& inV) : x(inV.x), y(inV.y), z(inV.z) {}
        FORCEINLINE Vector3D<T>(T _v): x(_v), y(_v), z(_v) {}
        FORCEINLINE Vector3D<T>(T _x, T _y, T _z) : x(_x), y(_y), z(_z) {}

        FORCEINLINE Vector3D<T>& operator= (const Vector3D<T>& inV) 
        {
            x = inV.x;
            y = inV.y;
            z = inV.z;
            return *this;
        }

        FORCEINLINE Vector3D<T>& operator= (const T* array)
        {
            x = array[0];
            y = array[1];
            z = array[2];
            return *this;
        }

        FORCEINLINE Vector3D<T>& operator+= (const Vector3D<T>& inV)
        {
            x += inV.x;
            y += inV.y;
            z += inV.z;

            return *this;
        }

        FORCEINLINE Vector3D<T>& operator-= (const Vector3D<T>& inV)
        {
            x -= inV.x;
            y -= inV.y;
            z -= inV.z;
            return *this;
        }

        FORCEINLINE Vector3D<T> operator+(const Vector3D<T>& inV)
        {
            return Vector3D<T>(x + inV.x, y + inV.y, z + inV.z);
        }

        FORCEINLINE Vector3D<T> operator- (const Vector3D<T>& inV) const 
        {
            return Vector3D<T>(x - inV.x, y - inV.y, z - inV.z);
        }

        // negative
        FORCEINLINE Vector3D<T> operator-() const
        {
            return Vector3D<T>(-x, -y, -z);
        }

        FORCEINLINE Vector3D<T> operator*(float scale)
        {
            return Vector3D<T>(x * scale, y * scale, z * scale);
        }

        FORCEINLINE Vector3D<T> operator/(float scale)
        {
            const float rScale = 1.f / scale;
            //return (*this) * rScale;
            return Vector3D<T>(x * rScale, y * rScale, z * rScale);
        }

        FORCEINLINE float DotProduct(const Vector3D<T>& inV) const
        {
            return x * inV.x + y * inV.y + z * inV.z;
        }

        FORCEINLINE Vector3D<T> CrossProduct(const Vector3D<T>& inV) const
        {
            return Vector3D<T>(
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

        FORCEINLINE T* GetAddressOf()
        {
            return &data[0];
        }

        friend std::ostream& operator<< (std::ostream& out, const Vector3D<T>& vec)
        {
			out << "(";

            out << vec.x << "," << vec.y << "," << vec.z;

            out << ")";

			return out;
        }
    };

    typedef Vector3D<uint32_t>  Vector3Di;
    typedef Vector3D<float>     Vector3Df;
    typedef Vector3D<uint8_t>   ColorRGBi;
    typedef Vector3D<float>     ColorRGBf;
}

