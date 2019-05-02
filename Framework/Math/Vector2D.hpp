#pragma once

#include <math.h>
#include <iostream>

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

        __forceinline Vector2D() {};
        __forceinline Vector2D(const Vector2D& inV) : x(inV.x), y(inV.y) {}
        __forceinline Vector2D(float _v): x(_v), y(_v) {}
        __forceinline Vector2D(float _x, float _y) : x(_x), y(_y) {}

        __forceinline Vector2D& operator= (const Vector2D& inV) 
        {
            x = inV.x;
            y = inV.y;
            return *this;
        }

        __forceinline Vector2D& operator+= (const Vector2D& inV)
        {
            x += inV.x;
            y += inV.y;
            return *this;
        }

        __forceinline Vector2D& operator-= (const Vector2D& inV)
        {
            x -= inV.x;
            y -= inV.y;
            return *this;
        }

        __forceinline Vector2D operator+(const Vector2D& inV)
        {
            return Vector2D(x + inV.x, y + inV.y);
        }

        __forceinline Vector2D operator- (const Vector2D& inV)
        {
            return Vector2D(x - inV.x, y - inV.y);
        }

        // negative
        __forceinline Vector2D operator-()
        {
            return Vector2D(-x, -y);
        }

        __forceinline Vector2D operator+(float bias)
        {
            return Vector2D(x + bias, y + bias);
        }

        __forceinline Vector2D operator-(float bias)
        {
            return Vector2D(x - bias, y - bias);
        }

        __forceinline Vector2D operator*(float scale)
        {
            return Vector2D(x * scale, y * scale);
        }

        __forceinline Vector2D operator/(float scale)
        {
            const float rScale = 1.f / scale;
            //return (*this) * rScale;
            return Vector2D(x * rScale, y * rScale);
        }

        __forceinline float DotProduct(const Vector2D& inV)
        {
            return x * inV.x + y * inV.y;
        }

        __forceinline float Length()
        {
            return sqrtf(x * x + y * y);
        }

        __forceinline float SquareLength()
        {
            return x * x + y * y;
        }

        __forceinline void Normalize()
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

