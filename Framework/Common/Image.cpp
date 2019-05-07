#include "Image.hpp"

namespace Panda
{
    std::ostream& operator<<(std::ostream& out, const Image& image)
    {
        out << "Image" << std::endl;
        out << "-----" << std::endl;
        out << "Width: 0x" << image.Width << std::endl;
        out << "Height: 0x" << image.Height << std::endl;
        out << "Bit Count: 0x" << image.BitCount << std::endl;
        out << "Pitch: 0x" << image.Pitch << std::endl;
        out << "Data Size: 0x" << image.DataSize << std::endl;

        return out;
    }
}