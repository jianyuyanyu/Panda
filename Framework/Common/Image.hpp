#pragma once
#include "PandaMath.hpp"

namespace Panda
{
    typedef struct _Image
    {
        uint32_t width; // width in pixels
        uint32_t height; // height in pixels
        ColorRGBA* data;
        uint32_t bitcount; // bit count per pixel
        uint32_t pitch; // size of one line, in bytes
        size_t dataSize; // the size of data area, which is pitch * height rather than width * height * bitcount / 8, because of alignments
    } Image;
}

