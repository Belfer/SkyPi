#pragma once

#include <engine/string.hpp>

#include <fstream>

using InputFileStream = std::ifstream;
using OutputFileStream = std::ofstream;

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

    static CString<512> GetPath(StringView filename)
    {
        CString<512> filepath = GAME_PATH;
        filepath.append(filename.data());
        return filepath;
    }
};