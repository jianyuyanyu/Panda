#pragma once

#include <math.h>
#include <iostream>
#include "Utility.hpp"

/**
 * A vector in 2D with components X, Y  
 */

namespace Panda
{
    template<typename T>
    struct Vector2D
    {
    public:
        union 
        {
            T data[2];
			struct { T x, y; };
			struct { T u, v; };
        };

        FORCEINLINE Vector2D<T>() {x = y = 0;}
        FORCEINLINE Vector2D<T>(const Vector2D<T>& inV) : x(inV.x), y(inV.y) {}
        FORCEINLINE Vector2D<T>(T _v): x(_v), y(_v) {}
        FORCEINLINE Vector2D<T>(T _x, T _y) : x(_x), y(_y) {}

        FORCEINLINE Vector2D<T>& operator= (const Vector2D<T>& inV) 
        {
            x = inV.x;
            y = inV.y;
            return *this;
        }

        FORCEINLINE Vector2D<T>& operator= (const T* array)
        {
            x = array[0];
            y = array[1];
            return *this;
        }

        FORCEINLINE Vector2D<T>& operator+= (const Vector2D<T>& inV)
        {
            x += inV.x;
            y += inV.y;
            return *this;
        }

        FORCEINLINE Vector2D<T>& operator-= (const Vector2D<T>& inV)
        {
            x -= inV.x;
            y -= inV.y;
            return *this;
        }

        FORCEINLINE Vector2D<T> operator+(const Vector2D<T>& inV)
        {
            return Vector2D<T>(x + inV.x, y + inV.y);
        }

        FORCEINLINE Vector2D<T> operator- (const Vector2D<T>& inV)
        {
            return Vector2D<T>(x - inV.x, y - inV.y);
        }

        // negative
        FORCEINLINE Vector2D<T> operator-()
        {
            return Vector2D<T>(-x, -y);
        }

        FORCEINLINE Vector2D operator*(float scale)
        {
            return Vector2D<T>(x * scale, y * scale);
        }

        FORCEINLINE Vector2D<T> operator/(float scale)
        {
            const float rScale = 1.f / scale;
            //return (*this) * rScale;
            return Vector2D<T>(x * rScale, y * rScale);
        }

        FORCEINLINE float DotProduct(const Vector2D<T>& inV)
        {
            return x * inV.x + y * inV.y;
        }

        FORCEINLINE float Length()
        {
            return sqrtf(x * x + y * y);
        }

        FORCEINLINE float SquareLength()
        {
            return x * x + y * y;
        }

        FORCEINLINE void Normalize()
        {
            float len = Length();
            x /= len;
            y /= len;
        }

        FORCEINLINE T* GetAddressOf()
        {
            return &data[0];
        }

        friend std::ostream& operator<< (std::ostream& out, const Vector2D<T>& vec)
        {
			out << "(";

            out << vec.x << "," << vec.y;

            out << ")";

            return out;
        }
    };

    typedef Vector2D<uint32_t>  Vector2Di;
    typedef Vector2D<float>     Vector2Df;
}

