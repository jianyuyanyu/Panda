#pragma once
#include "Interface.hpp"
#include "Image.hpp"
#include "Buffer.hpp"

namespace Panda
{
    Interface ImageParser
    {
    public:
        virtual Image Parse(Buffer& buf) = 0;
    };
}
