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
        virtual Image Parse(const Buffer& buf)
        {
            Image img;
            BITMAP_FILEHEADER* pFileHeader = reinterpret_cast<BITMAP_FILEHEADER*>(buf.m_pData);
            BITMAP_HEADER* pBmpHeader = reinterpret_cast<BITMAP_HEADER*>(buf.m_pData + BITMAP_FILEHEADER_SIZE);
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

            img.width = pBmpHeader->width;
            img.height = pBmpHeader->height;
            img.bitcount = 32;
            img.pitch = ((img.width * img.bitcount >> 3) + 3) & ~3; // 4 bytes alignment
            img.dataSize = img.pitch * img.height;
            img.data = reinterpret_cast<ColorRGBA*>(g_pMemoryManager->Allocate(img.dataSize));

            if (img.bitcount < 24)
            {
                std::cout << "Sorry, only true color BMP is supported now." << std::endl;
            }
            else
            {
                uint8_t* pSourceData = buf.m_pData + pFileHeader->bitsOffset;
                for (int32_t y = img.height - 1; y >= 0; --y)
                {
					if (y == 0)
						int m = 0;
                    for (int32_t x = 0; x < img.width; ++x)
                    {
                        ColorRGBA* pDst = img.data + img.width * (img.height - y - 1) + x;
                        uint8_t* pSrc = pSourceData + img.pitch * y + x * (img.bitcount >> 3);
                        pDst->b = *pSrc;
                        pDst->g = *(pSrc + 1);
                        pDst->r = *(pSrc + 2);
						pDst->a = *(pSrc + 3);

                        // (img.data + img.width * (img.height - y - 1) + x)->bgra = 
                        //     *reinterpret_cast<ColorRGBA*>(pSourceData + img.pitch * y + x * (img.bitcount >> 3));
                    }
                }
            }
            
            return img;
        }
    };
}
