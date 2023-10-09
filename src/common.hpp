#pragma once

#include <cstdio>
#include <cstdint>
#include <cassert>
#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <optional>

using Path = std::filesystem::path;
using VariableString = std::string;

namespace filesys = std::filesystem;

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

[[gnu::const]]
inline std::optional<FileMode> modeFromInt(uint32_t arg) {
    switch (arg) {
        case 40000: return FileMode::kDirectories;
        case 100644: return FileMode::kRegular;
        case 100755: return FileMode::kRegularExecutable;
        case 120000: return FileMode::kSymbolicLink;
        case 160000: return FileMode::kGitLink;
        default: break;
    }
    return std::nullopt;
}

enum class InArgType: uint8_t{
    kRawString,
    kFilename
};

#pragma endregion

#pragma region Classes
template<typename T, T modulo_n>
struct CongruenceClass {
    T value;

    CongruenceClass() = default;
    explicit CongruenceClass(T x):value(x % modulo_n) {}
    explicit CongruenceClass(int x):value(x % modulo_n) {if (value < 0) value += modulo_n;}

    CongruenceClass operator+(const CongruenceClass& ano) {
        return value + ano.value;
    }
    CongruenceClass operator-() {
        return modulo_n - value;
    }
};

struct TimeZone {
    bool positive;
    CongruenceClass<uint8_t, 24> hour;
    CongruenceClass<uint8_t, 60> minute;

    TimeZone(): positive(true),hour(0),minute(0) {}
    TimeZone(int hh, int mm):
        positive(hh >= 0),hour(std::abs(hh)),minute(mm) {
        assert(hh >= -12 && hh <= 12);
        assert(mm >= 0 && mm < 60);
    }

    friend std::ostream& operator<<(std::ostream &os, const TimeZone &tz) {
        char c = tz.positive ? '+' : '-';
        char str[8];
        snprintf(str, 8, "%c%02d%02d", c, tz.hour.value, tz.minute.value);
        os << str;
        return os;
    }
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