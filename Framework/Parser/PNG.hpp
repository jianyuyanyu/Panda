#pragma once

#include <cstdio>
#include <iostream>
#include <string>
#include <cassert>
#include <queue>
#include <algorithm>
#include "Utility.hpp"
#include "ImageParser.hpp"
#include "portable.hpp"
#include "zlib.h"

namespace Panda
{
#pragma pack(push, 1)
    struct PNG_FILEHEADER
    {
        uint64_t Signature;
    };

    ENUM(PNG_CHUNK_TYPE)
    {
        IHDR = "IHDR"_u32,
        PLTE = "PLTE"_u32,
        IDAT = "IDAT"_u32,
        IEND = "IEND"_u32
    };

    static std::ostream& operator<<(std::ostream& out, PNG_CHUNK_TYPE type)
    {
        int32_t n = static_cast<int32_t> (type);
        n = to_endian_native(n);
        char* c = reinterpret_cast<char*>(&n);

        for (size_t i = 0; i < sizeof(int32_t); ++i)
        {
            out << *c++;
        }

        return out;
    }

    struct PNG_CHUNK_HEADER 
    {
        uint32_t    Length;
        PNG_CHUNK_TYPE Type;
    };

    struct PNG_IHDR_HEADER : PNG_CHUNK_HEADER
    {
        uint32_t    Width;
        uint32_t    Height;
        uint8_t     BitDepth;
        uint8_t     ColorType;
        uint8_t     CompressionMethod;
        uint8_t     FilterMethod;
        uint8_t     InterlaceMethod;
    };

    struct PNG_PLTE_HEADER : PNG_CHUNK_HEADER
    {
        Vector3D<uint8_t>* pEntries;
    };
#pragma pack(pop)

    // report a zlib or i/o error
    static void zerr(int ret)
    {
        fputs("zpipe: ", stderr);
        switch(ret)
        {
            case Z_ERRNO:
                if (ferror(stdin))
                    fputs("error reading stdin\n", stderr);
                if (ferror(stdout))
                    fputs("error writing stdout\n", stderr);
                break;
            case Z_STREAM_ERROR:
                fputs("invalid compression level\n", stderr);
                break;
            case Z_DATA_ERROR:
                fputs("invalid or incomplete deflate data\n", stderr);
                break;
            case Z_MEM_ERROR:
                fputs("out of memory\n", stderr);
                break;
            case Z_VERSION_ERROR:
                fputs("zlib version mismatch!\n", stderr);
                break;
        }
    }

    class PngParser : implements ImageParser
    {
        protected:
            uint16_t m_Width;
            uint16_t m_Height;
            uint8_t  m_BitDepth;
            uint8_t  m_ColorType;
            uint8_t  m_CompressionMethod;
            uint8_t  m_FilterMethod;
            uint8_t  m_InterlaceMethod;
            size_t   m_ScanLineSize;
            uint8_t  m_BytesPerPixel;

        public:
            virtual Image Parse(const Buffer& buf)
            {
                Image img;

                const uint8_t* pData = buf.GetData();
                const uint8_t* pDataEnd = buf.GetData() + buf.GetDataSize();

                const PNG_FILEHEADER* pFileHeader = reinterpret_cast<const PNG_FILEHEADER*>(pData);
                pData += sizeof(PNG_FILEHEADER);
                if (pFileHeader->Signature == to_endian_net((uint64_t)0x89504E470D0A1A0A))
                {
                    std::cout << "Asset is PNG file" << std::endl;

                    while(pData < pDataEnd)
                    {
                        const PNG_CHUNK_HEADER* pChunkHeader = reinterpret_cast<const PNG_CHUNK_HEADER*>(pData);
                        PNG_CHUNK_TYPE type = static_cast<PNG_CHUNK_TYPE>(to_endian_native(static_cast<uint32_t>(pChunkHeader->Type)));

                        std::cout << "======================" << std::endl;

                        switch(type)
                        {
                            case PNG_CHUNK_TYPE::IHDR:
                            {
                                std::cout << "IHDR (Image Header)" << std::endl;
                                std::cout << "------------------------------" << std::endl;
                                const PNG_IHDR_HEADER* pIHDRHeader = reinterpret_cast<const PNG_IHDR_HEADER*>(pData);
                                m_Width = to_endian_native(pIHDRHeader->Width);
                                m_Height = to_endian_native(pIHDRHeader->Height);
                                m_BitDepth = pIHDRHeader->BitDepth;
                                m_ColorType = pIHDRHeader->ColorType;
                                m_CompressionMethod = pIHDRHeader->CompressionMethod;
                                m_FilterMethod = pIHDRHeader->FilterMethod;
                                m_InterlaceMethod = pIHDRHeader->InterlaceMethod;

                                switch (m_ColorType)
                                {
                                    case 0: // gray scale
                                        m_BytesPerPixel = (m_BitDepth + 7) >> 3;
                                        break;
                                    case 2: // rgb true color
                                        m_BytesPerPixel = (m_BitDepth * 3) >> 3;
                                        break;
                                    case 3: // indexed
                                        m_BytesPerPixel = (m_BitDepth + 7) >> 3;
                                        break;
                                    case 4: // grayscale with alpha
                                        m_BytesPerPixel = (m_BitDepth * 2) >> 3;
                                        break;
                                    case 6:
                                        m_BytesPerPixel = (m_BitDepth * 4) >> 3;
                                        break;
                                    default:
                                        std::cout << "Unknown Color Type: " << m_ColorType << std::endl;
                                        assert(0);
                                }

                                m_ScanLineSize = m_BytesPerPixel * m_Width;

                                img.Width = m_Width;
                                img.Height = m_Height;
                                img.BitCount = 32; // currently we fixed at RGBA for rendering
                                img.Pitch = (img.Width * (img.BitCount >> 3) + 3) & ~3u;    // for GPU address alignment
                                img.DataSize = img.Pitch * img.Height;
                                img.Data = g_pMemoryManager->Allocate(img.DataSize);

                                std::cout << "Width: " << m_Width << std::endl;
                                std::cout << "Height: " << m_Height << std::endl;
                                std::cout << "Bit Depth: " << (int)m_BitDepth << std::endl;
                                std::cout << "Color Type: " << (int)m_ColorType << std::endl;
                                std::cout << "Compression Method: " << (int)m_CompressionMethod << std::endl;
                                std::cout << "Filter Method: " << (int)m_FilterMethod << std::endl;
                                std::cout << "Interlace Method: " << (int)m_InterlaceMethod << std::endl;
                            }
                            break;
                            case PNG_CHUNK_TYPE::PLTE:
                            {
                                std::cout << "PLTE (Palette)" << std::endl;
                                std::cout << "-------------------------" << std::endl;
                                const PNG_PLTE_HEADER* pPLTEHeader = reinterpret_cast<const PNG_PLTE_HEADER*>(pData);
                                size_t maxCount = to_endian_native(pPLTEHeader->Length) / sizeof(*pPLTEHeader->pEntries);
                                for (size_t i = 0; i < maxCount; ++i)
                                {
#if DUMP_DETAILS
                                    std::cout << "Entry " << i << ": " << pPLTEHeader->pEntries[i] << std::endl;
#endif
                                }
                            }
                            break;
                            case PNG_CHUNK_TYPE::IDAT:
                            {
                                std::cout << "IDAT (Image Data Start)" << std::endl;
                                std::cout << "--------------------------" << std::endl;

                                size_t compressedDataSize = to_endian_native(pChunkHeader->Length);

                                std::cout << "Compressed Data Length: " << compressedDataSize << std::endl;

                                const size_t kChunkSize = 256 * 1024;
                                z_stream strm;
                                strm.zalloc = Z_NULL;
                                strm.zfree = Z_NULL;
                                strm.opaque = Z_NULL;
                                strm.avail_in = 0;
                                strm.next_in = Z_NULL;
                                int ret = inflateInit(&strm);
                                if (ret != Z_OK)
                                {
									zerr(ret);
                                    //std::cout << "[Error] Failed to init zlib" << std::endl;
                                    break;
                                }

                                const uint8_t* pIn = pData + sizeof(PNG_CHUNK_HEADER); // point to the start of the input data buffer
                                uint8_t* pOut = reinterpret_cast<uint8_t*>(img.Data); // point to the start of the input data buffer
                                uint8_t* pDecompressedBuffer = new uint8_t[kChunkSize];
                                uint8_t filterType = 0;
                                int currentRow = 0;
                                int currentCol = -1; // -1 means we need to read filter type

                                do 
                                {
                                    size_t nextInSize = (compressedDataSize > kChunkSize)? kChunkSize : compressedDataSize;
                                    if (nextInSize == 0) break;
                                    compressedDataSize -= nextInSize;
                                    strm.next_in = const_cast<Bytef*>(pIn);
                                    strm.avail_in = nextInSize;
                                    do
                                    {
                                        strm.avail_out = kChunkSize; // 256K
                                        strm.next_out = static_cast<Bytef*>(pDecompressedBuffer);
                                        ret = inflate(&strm, Z_NO_FLUSH);
                                        assert(ret != Z_STREAM_ERROR);
                                        switch(ret)
                                        {
                                            case Z_NEED_DICT:
                                            case Z_DATA_ERROR:
                                            case Z_MEM_ERROR:
                                                zerr(ret);
                                                (void)inflateEnd(&strm);
                                                strm.avail_out = 0;
                                                ret = Z_STREAM_END;
                                                assert(0);
                                            default:
                                                ;
                                        }

                                        // now we de-filter the data into image
                                        uint8_t* p = pDecompressedBuffer;
                                        while(p - pDecompressedBuffer < (kChunkSize - strm.avail_out))
                                        {
                                            if (currentCol == -1)
                                            {
                                                // we are at start of scan line, get the filter type and advance the pointer
                                                filterType = *p;
                                            }
                                            else 
                                            {
                                                // prediction filter
                                                // X is current value
                                                //
                                                // C B D
                                                // A X
                                                uint8_t A, B, C;
                                                if (currentRow == 0)
                                                {
                                                    B = C = 0;
                                                }
                                                else 
                                                {
                                                    B = *(pOut + img.Pitch * (currentRow - 1) + currentCol);
                                                    C = (currentCol < m_BytesPerPixel)? 0 : *(pOut + img.Pitch * (currentRow - 1) + currentCol - m_BytesPerPixel);
                                                }

                                                A = (currentCol < m_BytesPerPixel)? 0 : *(pOut + img.Pitch * currentRow + currentCol - m_BytesPerPixel);

                                                switch(filterType)
                                                {
                                                    case 0:
                                                        *(pOut + img.Pitch * currentRow + currentCol) = *p;
                                                        break;
                                                    case 1:
                                                        *(pOut + img.Pitch * currentRow + currentCol) = *p + A;
                                                        break;
                                                    case 2:
                                                        *(pOut + img.Pitch * currentRow + currentCol) = *p + B;
                                                        break;
                                                    case 3:
                                                        *(pOut + img.Pitch * currentRow + currentCol) = *p + (A + B) / 2;
                                                        break;
                                                    case 4:
                                                        {
                                                            int _p = A + B - C;
                                                            int pa = abs(_p - A);
                                                            int pb = abs(_p - B);
                                                            int pc = abs(_p - C);
                                                            if (pa <= pb && pa <= pc)
                                                                *(pOut + img.Pitch * currentRow + currentCol) = *p + A;
                                                            else if (pb <= pc)
                                                                *(pOut + img.Pitch * currentRow + currentCol) = *p + B;
                                                            else 
                                                                *(pOut + img.Pitch * currentRow + currentCol) = *p + C;
                                                        }
                                                        break;
                                                    default:
                                                        std::cout << "[Error] Unknown Filter type!" << std::endl;
                                                        assert(0);
                                                }
                                            }

                                            ++currentCol;
                                            if (currentCol == m_ScanLineSize)
                                            {
                                                currentCol = -1;
                                                ++currentRow;
                                            }

                                            p++;
                                        }
                                    } while (strm.avail_out == 0);
                                    
                                    pIn += nextInSize; // move pIn to next chunk
                                }while (ret != Z_STREAM_END);

                                (void)inflateEnd(&strm);
                                g_pMemoryManager->Free(pDecompressedBuffer, kChunkSize);
                            }
                            break;
                            case PNG_CHUNK_TYPE::IEND:
                            {
                                std::cout << "IEND (Image Data End) " << std::endl;
                                std::cout << "---------------------------" << std::endl;
                            }
                            break;
                            default:
                            {
                                std::cout << "Ingore Unrecognized Chunk. Marker = " << type << std::endl;
                            }
                            break;
                        }
                        pData += to_endian_native(pChunkHeader->Length) + sizeof(PNG_CHUNK_HEADER) + 4 /* length of CRC */;
                    }
                }
                else 
                {
                    std::cout << "File is not a PNG file!" << std::endl;
                }

                return img;
            }
    };
}
