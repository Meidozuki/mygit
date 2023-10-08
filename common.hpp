#include <cstdint>
#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>

enum class ObjectType: uint8_t{
    kBlob,
    kTree,
    kCommit,
    kTag
};

enum class InArgType: uint8_t{
    kRawString,
    kFilename
};

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