#pragma once

#include <math.h>
#include <iostream>
#include "Utility.hpp"

/**
 * A vector in 2D with components X, Y  
 */

namespace Panda
{
    struct Vector2D
    {
    public:
        float x;
        float y;

        FORCEINLINE Vector2D() {}
        FORCEINLINE Vector2D(const Vector2D& inV) : x(inV.x), y(inV.y) {}
        FORCEINLINE Vector2D(float _v): x(_v), y(_v) {}
        FORCEINLINE Vector2D(float _x, float _y) : x(_x), y(_y) {}

        FORCEINLINE Vector2D& operator= (const Vector2D& inV) 
        {
            x = inV.x;
            y = inV.y;
            return *this;
        }

        FORCEINLINE Vector2D& operator+= (const Vector2D& inV)
        {
            x += inV.x;
            y += inV.y;
            return *this;
        }

        FORCEINLINE Vector2D& operator-= (const Vector2D& inV)
        {
            x -= inV.x;
            y -= inV.y;
            return *this;
        }

        FORCEINLINE Vector2D operator+(const Vector2D& inV)
        {
            return Vector2D(x + inV.x, y + inV.y);
        }

        FORCEINLINE Vector2D operator- (const Vector2D& inV)
        {
            return Vector2D(x - inV.x, y - inV.y);
        }

        // negative
        FORCEINLINE Vector2D operator-()
        {
            return Vector2D(-x, -y);
        }

        FORCEINLINE Vector2D operator+(float bias)
        {
            return Vector2D(x + bias, y + bias);
        }

        FORCEINLINE Vector2D operator-(float bias)
        {
            return Vector2D(x - bias, y - bias);
        }

        FORCEINLINE Vector2D operator*(float scale)
        {
            return Vector2D(x * scale, y * scale);
        }

        FORCEINLINE Vector2D operator/(float scale)
        {
            const float rScale = 1.f / scale;
            //return (*this) * rScale;
            return Vector2D(x * rScale, y * rScale);
        }

        FORCEINLINE float DotProduct(const Vector2D& inV)
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

        friend std::ostream& operator<< (std::ostream& out, const Vector2D& vec)
        {
			out << "(";

            out << vec.x << "," << vec.y;

            out << ")";

            return out;
        }
    };
}

