#pragma once
#include <algorithm>
#include "PandaMath.hpp"
#include "portable.hpp"

namespace Panda
{
    typedef Vector3D<float> RGBf;
    typedef Vector3D<float> YCbCrf;

    const Matrix4f RGB2YCbCr = {
        0.229f, -0.168736f,  0.5f,      0.0f,
        0.587f, -0.331264f, -0.418688f, 0.0f,
        0.114f,  0.5f     , -0.081312f, 0.0f,
        0.0f,    128.0f,     128.0f,    0.0f
    };

    const Matrix4f YCbCr2RGB = {
              1.0f,    1.0f     ,    1.0f  ,    0.0f,
              0.0f, -  0.344136f,    1.772f,    0.0f,
            1.402f, -  0.714136f,    0.0f  ,    0.0f,
         -179.456f,  135.458816f, -226.816f,    0.0f,
    };

    YCbCrf ConvertRGB2YCbCr(const RGBf& rgb)
    {
        Vector4Df result(rgb.r, rgb.g, rgb.b, 1.0f);
        TransformCoord(result, RGB2YCbCr);
        return YCbCrf(std::clamp<float>(result.r + 0.5f, 0.0f, 255.0f),
                      std::clamp<float>(result.g + 0.5f, 0.0f, 255.0f),
                      std::clamp<float>(result.b + 0.5f, 0.0f, 255.0f));
    }

    RGBf ConvertYCbCr2RGB(const YCbCrf& ycbcr)
    {
        Vector4Df result(ycbcr.r, ycbcr.g, ycbcr.b, 1.0f);
        TransformCoord(result, YCbCr2RGB);
        return RGBf(std::clamp<float>(result.r + 0.5f, 0.0f, 255.0f),
                    std::clamp<float>(result.g + 0.5f, 0.0f, 255.0f),
                    std::clamp<float>(result.b + 0.5f, 0.0f, 255.0f));
    }
}