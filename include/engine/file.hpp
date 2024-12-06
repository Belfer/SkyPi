#pragma once

#include <engine/string.hpp>

class File
{
public:
    static StringView GetFilename(StringView file)
    {
        SizeType lastSlash = file.find_last_of("/\\");
        if (lastSlash == std::string::npos)
            return file;

        return file.substr(lastSlash + 1);
    }
};