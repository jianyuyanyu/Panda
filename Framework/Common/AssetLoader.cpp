#include "AssetLoader.hpp"

int Panda::AssetLoader::Initialize()
{
    return 0;
}

void Panda::AssetLoader::Finalize()
{
    m_SearchPath.clear();
}

void Panda::AssetLoader::Tick()
{

}

bool Panda::AssetLoader::AddSearchPath(const char* path)
{
    std::vector<std::string>::iterator iter = m_SearchPath.begin();

    while (iter != m_SearchPath.end())
    {
        if (!(*iter).compare(path))
            return true;

        ++iter;
    }

    m_SearchPath.push_back(path);
    return true;
}

bool Panda::AssetLoader::RemoveSearchPath(const char* path)
{
    std::vector<std::string>::iterator iter = m_SearchPath.begin();

    while (iter != m_SearchPath.end())
    {
        if (!(*iter).compare(path))
        {
            m_SearchPath.erase(iter);
            return true;
        }

        ++iter;
    }

    return true;
}

/**
 * Try to open the file to check if it exsits.
 */ 
bool Panda::AssetLoader::FileExists(const char* filePath)
{
    AssetFilePtr fp = OpenFile(filePath, PANDA_OPEN_BINARY);
    if (fp != nullptr)
    {
        CloseFile(fp);
        return true;
    }

    return false;
}

/**
 * Search the directories to open the file.
 * It will try some parent directories.
 */ 
Panda::AssetLoader::AssetFilePtr Panda::AssetLoader::OpenFile(const char* name, AssetOpenMode mode)
{
    FILE* fp = nullptr;
    // loop N times up the hierarchy, testing at each level
    std::string upPath;
    std::string fullPath;
    for (int32_t i = 0; i < 10; ++i)
    {
        std::vector<std::string>::iterator iter = m_SearchPath.begin();
        bool looping = true;

        while (looping)
        {
            fullPath.assign(upPath);    // reset to current upPath
            if (iter != m_SearchPath.end())
            {
                fullPath.append(*iter);
                fullPath.append("/Asset/");
                ++iter;
            }
            else
            {
                fullPath.append("Asset/");
                looping = false;
            }
            fullPath.append(name);
#ifdef DEBUG
            fprintf(stderr, "Trying to open %s\n", fullPath.c_str());
#endif
            switch(mode)
            {
                case PANDA_OPEN_TEXT:
                    fp = fopen(fullPath.c_str(), "r");
                    break;
                case PANDA_OPEN_BINARY:
                    fp = fopen(fullPath.c_str(), "rb");
                    break;
            }

            if (fp)
                return (AssetFilePtr)fp;
        }

        upPath.append("../");
    }

    return nullptr;
}

Panda::Buffer Panda::AssetLoader::SyncOpenAndReadText(const char* filePath)
{
    AssetFilePtr fp = OpenFile(filePath, PANDA_OPEN_TEXT);
    Buffer* pBuff = nullptr;

    if (fp)
    {
        size_t length = GetSize(fp);
        pBuff = new Buffer(length + 1);
        size_t result = fread(pBuff->GetData(), 1, length, static_cast<FILE*>(fp));
        pBuff->GetData()[result] = '\0';

        CloseFile(fp);
    }
    else
    {
        fprintf(stderr, "Error opening file '%s' \n", filePath);
        pBuff = new Buffer();
    }
    
#ifdef DEBUG
    fprintf(stderr, "Read file '%s', %d bytes\n", filePath, length);
#endif
    return *pBuff;
}

Panda::Buffer Panda::AssetLoader::SyncOpenAndReadBinary(const char* filePath)
{
    AssetFilePtr fp = OpenFile(filePath, PANDA_OPEN_BINARY);
    Buffer* pBuff = nullptr;

    if (fp)
    {
        size_t length = GetSize(fp);
        pBuff = new Buffer(length);
        fread (pBuff->GetData(), length, 1, static_cast<FILE*>(fp));

        CloseFile(fp);
    }
    else
    {
        fprintf(stderr, "Error opening file '%s' \n", filePath);
        pBuff = new Buffer();
    }
    
#ifdef DEBUG
    fprintf(stderr, "Read file '%s', %d bytes\n", filePath, length);
#endif
    return *pBuff;
}

void Panda::AssetLoader::CloseFile(AssetFilePtr& fp)
{
    fclose((FILE*)fp);
    fp = nullptr;
}

size_t Panda::AssetLoader::GetSize(const AssetFilePtr& fp)
{
    FILE* _fp = static_cast<FILE*>(fp);

    // We can't change its pos.
    long pos = ftell(_fp);
    fseek(_fp, 0, PANDA_SEEK_END);
    size_t length = ftell(_fp);
    fseek(_fp, pos, PANDA_SEEK_SET);

    return length;
}

size_t Panda::AssetLoader::SyncRead(const AssetFilePtr& fp, Buffer& buf)
{
    size_t sz;

    if (!fp)
    {
        fprintf(stderr, "null file discriptor\n");
        return 0;
    }

    sz = fread(buf.GetData(), buf.m_Size, 1, static_cast<FILE*>(fp));

#ifdef DEBUG
    fprintf(stderr, "Read file '%s', %d bytes\n", filePath, length);
#endif 

    return sz;
}

int32_t Panda::AssetLoader::Seek(AssetFilePtr fp, long offset, AssetSeekBase where)
{
    return fseek(static_cast<FILE*>(fp), offset, static_cast<int>(where));
}

