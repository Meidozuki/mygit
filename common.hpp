#pragma once

#include <cstdint>
#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>

using Path = std::filesystem::path;
using VariableString = std::string;

#pragma region Enums
enum class ObjectType: uint8_t{
    kBlob,
    kTree,
    kCommit,
    kTag
};

// 实际上16bit刚好，为了输入输出方便先用十进制表示
enum class FileMode: uint32_t {
    kDirectories = 40000,
    kRegular = 100644,
    kRegularExecutable = 100755,
    kSymbolicLink = 120000,
    kGitLink = 160000
};

enum class InArgType: uint8_t{
    kRawString,
    kFilename
};

#pragma endregion

#pragma region Classes
struct GeneralRecord{
    std::filesystem::path filename;
    const char *hash;
    FileMode file_mode;
    ObjectType object_type;
};

#pragma endregion

inline std::string readStream(std::ifstream &fs, std::streamsize n) {
    std::string buffer;
    if (fs.is_open()) {
        buffer.resize(n);
        fs.read(&buffer[0], n);
    }
    return buffer;
}

inline std::string readFile(const std::filesystem::path& filename, std::streamsize n) {
    std::ifstream fs(filename);
    return readStream(fs, n);
}

inline std::string readFile(const std::filesystem::path& filename) {
    std::ifstream fs(filename, std::ios::in | std::ios::ate);
    std::string buffer;
    if (fs.is_open()) {
        auto size = fs.tellg();
        fs.seekg(0);
        buffer.resize(size);
        fs.read(&buffer[0], size);
    }
    return buffer;
}

inline std::stringstream readFileAsStream(const std::filesystem::path& filename) {
    std::stringstream ss;
    ss << std::ifstream(filename).rdbuf();
    return ss;
}