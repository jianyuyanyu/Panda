#pragma once
#include "Interface.hpp"
#include "Image.hpp"
#include "Buffer.hpp"

namespace Panda
{
    interface ImageParser
    {
    public:
        virtual Image Parse(const Buffer& buf) = 0;
    };
}