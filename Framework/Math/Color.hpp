#pragma once

#include <iostream>

/**
 * A color with r,g,b,a 
 */
namespace Panda
{
    struct ColorRGBA
    {
    public:
        union {
            struct {
                uint8_t r, g, b, a;
            };
            
        };

        __forceinline ColorRGBA() { r = g = b = a = 0; }
        __forceinline ColorRGBA(const ColorRGBA& inColor) : r(inColor.r), g(inColor.g), b(inColor.b), a(inColor.a) {}
        __forceinline ColorRGBA(uint8_t _v): r(_v), g(_v), b(_v), a(_v) {}
        __forceinline ColorRGBA(uint8_t _r, uint8_t _g, uint8_t _b, uint8_t _a) : r(_r), g(_g), b(_b), a(_a) {}

        __forceinline ColorRGBA& operator= (const ColorRGBA& inColor) 
        {
            r = inColor.r;
            g = inColor.g;
            b = inColor.b;
            a = inColor.a;
            return *this;
        }

        __forceinline ColorRGBA& operator+= (const ColorRGBA& inColor)
        {
            r += inColor.r;
            g += inColor.g;
            b += inColor.b;
            a += inColor.a;
            return *this;
        }

        __forceinline ColorRGBA& operator-= (const ColorRGBA& inColor)
        {
            r -= inColor.r;
            g -= inColor.g;
            b -= inColor.b;
            a -= inColor.a;
            return *this;
        }

        __forceinline ColorRGBA operator+(const ColorRGBA& inColor)
        {
            return ColorRGBA(r + inColor.r, g + inColor.g, b + inColor.b, a + inColor.a);
        }

        __forceinline ColorRGBA operator- (const ColorRGBA& inColor)
        {
            return ColorRGBA(r - inColor.r, g - inColor.g, b - inColor.b, a - inColor.a);
        }

        __forceinline ColorRGBA operator+(uint8_t bias)
        {
            return ColorRGBA(r + bias, g + bias, b + bias, a + bias);
        }

        __forceinline ColorRGBA operator-(uint8_t bias)
        {
            return ColorRGBA(r - bias, g - bias, b - bias, a - bias);
        }

        __forceinline ColorRGBA operator*(float scale)
        {
            return ColorRGBA(r * scale, g * scale, b * scale, a * scale);
        }

        __forceinline ColorRGBA operator/(float scale)
        {
            const float rScale = 1.f / scale;
            //return (*this) * rScale;
            return ColorRGBA(r * rScale, g * rScale, b * rScale, a * rScale);
        }
    };

    struct ColorRGBAf
    {
    public:
        union {
            struct {
                float r, g, b, a;
            };
        };

		__forceinline ColorRGBAf() : r(0), g(0), b(0), a(0) {}
        __forceinline ColorRGBAf(const ColorRGBAf& inColor) : r(inColor.r), g(inColor.g), b(inColor.b), a(inColor.a) {}
        __forceinline ColorRGBAf(float _v): r(_v), g(_v), b(_v), a(_v) {}
        __forceinline ColorRGBAf(float _r, float _g, float _b, float _a) : r(_r), g(_g), b(_b), a(_a) {}

        __forceinline ColorRGBAf& operator= (const ColorRGBAf& inColor)
        {
            r = inColor.r;
            g = inColor.g;
            b = inColor.b;
            a = inColor.a;
            return *this;
        }

        __forceinline ColorRGBAf& operator+= (const ColorRGBAf& inColor)
        {
            r += inColor.r;
            g += inColor.g;
            b += inColor.b;
            a += inColor.a;
            return *this;
        }

        __forceinline ColorRGBAf& operator-= (const ColorRGBAf& inColor)
        {
            r -= inColor.r;
            g -= inColor.g;
            b -= inColor.b;
            a -= inColor.a;
            return *this;
        }

        __forceinline ColorRGBAf operator+(const ColorRGBAf& inColor)
        {
            return ColorRGBAf(r + inColor.r, g + inColor.g, b + inColor.b, a + inColor.a);
        }

        __forceinline ColorRGBAf operator- (const ColorRGBAf& inColor)
        {
            return ColorRGBAf(r - inColor.r, g - inColor.g, b - inColor.b, a - inColor.a);
        }

        __forceinline ColorRGBAf operator+(float bias)
        {
            return ColorRGBAf(r + bias, g + bias, b + bias, a + bias);
        }

        __forceinline ColorRGBAf operator-(float bias)
        {
            return ColorRGBAf(r - bias, g - bias, b - bias, a - bias);
        }

        __forceinline ColorRGBAf operator*(float scale)
        {
            return ColorRGBAf(r * scale, g * scale, b * scale, a * scale);
        }

        __forceinline ColorRGBAf operator/(float scale)
        {
            const float rScale = 1.f / scale;
            //return (*this) * rScale;
            return ColorRGBAf(r * rScale, g * rScale, b * rScale, a * rScale);
        }
    };
}