#pragma once

#include <iostream>
#include "Utility.hpp"
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

        FORCEINLINE ColorRGBA() { r = g = b = a = 0; }
        FORCEINLINE ColorRGBA(const ColorRGBA& inColor) : r(inColor.r), g(inColor.g), b(inColor.b), a(inColor.a) {}
        FORCEINLINE ColorRGBA(uint8_t _v): r(_v), g(_v), b(_v), a(_v) {}
        FORCEINLINE ColorRGBA(uint8_t _r, uint8_t _g, uint8_t _b, uint8_t _a) : r(_r), g(_g), b(_b), a(_a) {}

        FORCEINLINE ColorRGBA& operator= (const ColorRGBA& inColor) 
        {
            r = inColor.r;
            g = inColor.g;
            b = inColor.b;
            a = inColor.a;
            return *this;
        }

        FORCEINLINE ColorRGBA& operator+= (const ColorRGBA& inColor)
        {
            r += inColor.r;
            g += inColor.g;
            b += inColor.b;
            a += inColor.a;
            return *this;
        }

        FORCEINLINE ColorRGBA& operator-= (const ColorRGBA& inColor)
        {
            r -= inColor.r;
            g -= inColor.g;
            b -= inColor.b;
            a -= inColor.a;
            return *this;
        }

        FORCEINLINE ColorRGBA operator+(const ColorRGBA& inColor)
        {
            return ColorRGBA(r + inColor.r, g + inColor.g, b + inColor.b, a + inColor.a);
        }

        FORCEINLINE ColorRGBA operator- (const ColorRGBA& inColor)
        {
            return ColorRGBA(r - inColor.r, g - inColor.g, b - inColor.b, a - inColor.a);
        }

        FORCEINLINE ColorRGBA operator+(uint8_t bias)
        {
            return ColorRGBA(r + bias, g + bias, b + bias, a + bias);
        }

        FORCEINLINE ColorRGBA operator-(uint8_t bias)
        {
            return ColorRGBA(r - bias, g - bias, b - bias, a - bias);
        }

        FORCEINLINE ColorRGBA operator*(float scale)
        {
            return ColorRGBA(r * scale, g * scale, b * scale, a * scale);
        }

        FORCEINLINE ColorRGBA operator/(float scale)
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

		FORCEINLINE ColorRGBAf() : r(0), g(0), b(0), a(0) {}
        FORCEINLINE ColorRGBAf(const ColorRGBAf& inColor) : r(inColor.r), g(inColor.g), b(inColor.b), a(inColor.a) {}
        FORCEINLINE ColorRGBAf(float _v): r(_v), g(_v), b(_v), a(_v) {}
        FORCEINLINE ColorRGBAf(float _r, float _g, float _b, float _a) : r(_r), g(_g), b(_b), a(_a) {}

        FORCEINLINE ColorRGBAf& operator= (const ColorRGBAf& inColor)
        {
            r = inColor.r;
            g = inColor.g;
            b = inColor.b;
            a = inColor.a;
            return *this;
        }

        FORCEINLINE ColorRGBAf& operator+= (const ColorRGBAf& inColor)
        {
            r += inColor.r;
            g += inColor.g;
            b += inColor.b;
            a += inColor.a;
            return *this;
        }

        FORCEINLINE ColorRGBAf& operator-= (const ColorRGBAf& inColor)
        {
            r -= inColor.r;
            g -= inColor.g;
            b -= inColor.b;
            a -= inColor.a;
            return *this;
        }

        FORCEINLINE ColorRGBAf operator+(const ColorRGBAf& inColor)
        {
            return ColorRGBAf(r + inColor.r, g + inColor.g, b + inColor.b, a + inColor.a);
        }

        FORCEINLINE ColorRGBAf operator- (const ColorRGBAf& inColor)
        {
            return ColorRGBAf(r - inColor.r, g - inColor.g, b - inColor.b, a - inColor.a);
        }

        FORCEINLINE ColorRGBAf operator+(float bias)
        {
            return ColorRGBAf(r + bias, g + bias, b + bias, a + bias);
        }

        FORCEINLINE ColorRGBAf operator-(float bias)
        {
            return ColorRGBAf(r - bias, g - bias, b - bias, a - bias);
        }

        FORCEINLINE ColorRGBAf operator*(float scale)
        {
            return ColorRGBAf(r * scale, g * scale, b * scale, a * scale);
        }

        FORCEINLINE ColorRGBAf operator/(float scale)
        {
            const float rScale = 1.f / scale;
            //return (*this) * rScale;
            return ColorRGBAf(r * rScale, g * rScale, b * rScale, a * rScale);
        }
    };
}
