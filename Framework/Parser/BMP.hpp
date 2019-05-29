#pragma once
#include <iostream>
#include "ImageParser.hpp"

namespace Panda
{
#pragma pack(push, 1)
    typedef struct _BITMAP_FILEHEADER
    {
        uint16_t signature;
        uint32_t size;
        uint32_t reserved;
        uint32_t bitsOffset;
    } BITMAP_FILEHEADER;

#define BITMAP_FILEHEADER_SIZE 14

    typedef struct _BITMAP_HEADER
    {
        uint32_t headerSize;
        int32_t width;
        int32_t height;
        uint16_t planes;
        uint16_t bitCount;
        uint32_t compression;
        uint32_t sizeImage;
        int32_t pelsPerMeterX;
        int32_t pelsPerMeterY;
        uint32_t clrUsed;
        uint32_t clrImportant;
    } BITMAP_HEADER; 
#pragma pack(pop)

    class BmpParser : implements ImageParser
    {
    public:
        virtual Image Parse(Buffer& buf)
        {
            Image img;
            const BITMAP_FILEHEADER* pFileHeader = reinterpret_cast<const BITMAP_FILEHEADER*>(buf.GetData());
            const BITMAP_HEADER* pBmpHeader = reinterpret_cast<const BITMAP_HEADER*>(reinterpret_cast<const uint8_t*>(buf.GetData())+ BITMAP_FILEHEADER_SIZE);
            if (pFileHeader->signature == 0X4D42 /* 'M' 'B'*/)
            {
                std::cout << "Aset is Windows BMP file" << std::endl;
                std::cout << "BMP HEADER" << std::endl;
                std::cout << "-----------------------------" << std::endl;
                std::cout << "File Size: " << pFileHeader->size << std::endl;
                std::cout << "Data Offset: " << pFileHeader->bitsOffset << std::endl;
                std::cout << "Iamge Width: " << pBmpHeader->width << std::endl;
                std::cout << "Image Height: " << pBmpHeader->height << std::endl;
                std::cout << "Image Planes: " << pBmpHeader->planes << std::endl;
                std::cout << "Image BitCount: " << pBmpHeader->bitCount << std::endl;
                std::cout << "Image Compression: " << pBmpHeader->compression << std::endl;
                std::cout << "Image Size: " << pBmpHeader->sizeImage << std::endl;
                std::cout << std::endl;
            }

            img.Width = pBmpHeader->width;
            img.Height = pBmpHeader->height;
            img.BitCount = 32;
            auto byteCount = img.BitCount >> 3;
            img.Pitch = ((img.Width * byteCount) + 3) & ~3; // 4 bytes alignment
            img.DataSize = img.Pitch * img.Height;
            img.Data = g_pMemoryManager->Allocate(img.DataSize);

            if (img.BitCount < 24)
            {
                std::cout << "Sorry, only true color BMP is supported now." << std::endl;
            }
            else
            {
                const uint8_t* pSourceData = reinterpret_cast<const uint8_t*>(buf.GetData()) + pFileHeader->bitsOffset;
                for (int32_t y = img.Height - 1; y >= 0; --y)
                {
					if (y == 0)
						int m = 0;
                    for (int32_t x = 0; x < img.Width; ++x)
                    {
                        uint8_t* pDst = reinterpret_cast<uint8_t*>(img.Data) + img.Width * (img.Height - y - 1) + x;
                        const uint8_t* pSrc = pSourceData + img.Pitch * y + x * byteCount;
                        pDst[2] = *pSrc;
                        pDst[1] = *(pSrc + 1);
                        pDst[0] = *(pSrc + 2);
						pDst[3] = *(pSrc + 3);

                        // (img.data + img.width * (img.height - y - 1) + x)->bgra = 
                        //     *reinterpret_cast<ColorRGBA*>(pSourceData + img.pitch * y + x * (img.bitcount >> 3));
                    }
                }
            }
            
            return img;
        }
    };
}
