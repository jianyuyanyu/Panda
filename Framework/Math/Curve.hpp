#pragma once
#include <vector>
#include "portable.hpp"

namespace Panda
{
    ENUM(CurveType)
    {
        kLinear = "LINE"_i32,
        kBezier = "BEZI"_i32
    };

    template <typename T>
    struct Curve
    {
        Curve() = default;
        virtual ~Curve() = default;
        virtual T Reverse(const T p) const = 0;
        virtual T Interpolate(const T t) const = 0;
        virtual CurveType GetCurveType() const = 0;
    };
}