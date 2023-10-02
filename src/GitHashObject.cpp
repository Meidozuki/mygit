
#include "GitHashObject.hpp"

#include <string>
#include <fstream>
#include <stdexcept>
#include <filesystem>

#include <cryptopp/cryptlib.h>
#include <cryptopp/sha.h>
#include <cryptopp/hex.h>

#include "objects_proxy.hpp"

// Do a direct SHA1 hash on message
SHAString hashObject(StringView msg) {
    using namespace CryptoPP;
    SHA1 sha1;
    std::string binary_digest, encoded;

    static_assert(std::is_same_v<const char*, decltype(msg.data())>);
    sha1.Update((const byte*)msg.data(), msg.size());
    binary_digest.resize(sha1.DigestSize());
    sha1.Final((byte*)binary_digest.data());

    constexpr bool uppercase = false;
    StringSource(binary_digest, true, new HexEncoder(
            new StringSink(encoded), uppercase
            ));

    return SHA1Proxy::create_s(encoded.c_str());
}

// whether to sync with git or for better deserialize
static constexpr char SEPERATOR = '\0';

/**
 * @param type
 * @param content std::string. 由于basic_string::append()只重载了const&，不使用右值传递
 * @return std::string
 */
std::string getStringToHash(ObjectType type, const std::string& content) {
    std::string res = GitObject::typeName(type);
    res = res + ' ' + std::to_string(content.size());
    res.resize(res.size()+1, SEPERATOR); // Add terminator '\0'
    res.append(content);
    return res;
}

/**
 *
 * @param msg The message to hash, which later a header will be added to
 * @param arg_type
 * @param type
 * @param if_write
 * @return The result SHA1 hash
 */
SHAString hashObjectInterface(StringView msg, InArgType arg_type, ObjectType type, bool if_write) {
    using namespace std::string_literals;
    std::string original_str;
    if (arg_type == InArgType::kRawString) {
        original_str = msg;
    }
    else if (arg_type == InArgType::kFilename) {
        if (type != ObjectType::kBlob) {
            throw std::invalid_argument("When hashObject receives a filename, it should be a *blob*."); //throws
        }
        original_str = readFile(msg);
    }
    else {
        throw std::invalid_argument(""); //throws
    }

    std::string full = getStringToHash(type, original_str);
    // It allows string to string_view implicitly, but do it explicitly for clearness
    std::string_view view(full.c_str(), full.size());
    // Hash header+content
    SHAString hash = hashObject(view);

    if (if_write) {
        // Get path to save
        auto &objects_proxy = GitObjectsProxy::getInstance();
        objects_proxy.insert(hash);
        std::filesystem::path file_path = objects_proxy.getFilePath(hash).value(); // no failure
        
        // Open file to save
        std::filesystem::create_directories(file_path.parent_path());
        std::ofstream fs(file_path);
        if (fs.is_open()) {
            fs << full;
        }
        else {
            throw std::runtime_error("Open file "s + std::string(hash) + " failed when saving it."s); //throws
        }
    }

    return hash;
}
