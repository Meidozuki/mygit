//
// Created by user on 2023/9/3.
//

#include "GitHashObject.hpp"

#include <iostream>
#include <fstream>
#include <sstream>

#include <cryptopp/cryptlib.h>
#include <cryptopp/sha.h>
#include <cryptopp/hex.h>

#include "objects.hpp"

// Do a direct SHA1 hash on message
SHAString hashObject(StringView msg) {
    using namespace CryptoPP;
    SHA1 sha1;
    std::string binary_digest, encoded;

    static_assert(std::is_same_v<const char*, decltype(msg.data())>);
    sha1.Update((const byte*)msg.data(), msg.size());
    binary_digest.resize(sha1.DigestSize());
    sha1.Final((byte*)binary_digest.data());

    bool uppercase = false;
    StringSource(binary_digest, true, new HexEncoder(
            new StringSink(encoded), uppercase
            ));

    return SHA1Proxy::create_s(encoded.c_str());
}

// whether to sync with git or for better deserialize
static const char SEPERATOR = '\0';

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
 * @param msg The message to hash, which a header will be added to
 * @param arg_type
 * @param type
 * @param if_write
 * @return The result SHA1 hash
 */
SHAString hashObjectInterface(StringView msg, InArgType arg_type, ObjectType type, bool if_write) {
    std::string original_str;
    if (arg_type == InArgType::kRawString) {
        original_str = msg;
    }
    else if (arg_type == InArgType::kFilename) {
        if (type != ObjectType::kBlob) {
            throw std::invalid_argument("When hashObject receives a filename, it should be a *blob*.");
        }
        original_str = readFile(msg);
    }
    else {
        throw std::invalid_argument("");
    }

    std::string full = getStringToHash(type, original_str);
    // It allows string to string_view implicitly, but do it explicitly for clearness
    std::string_view view(full.c_str(), full.size());
    SHAString encoded = hashObject(view);

    if (if_write) {
        // create GitObject
        if (type == ObjectType::kBlob) {
            Blob blob;
            blob.content_ = original_str;
            blob.sha1_ = encoded.data();
            // call Proxy to save GitObject
            GitObjectsManager::getInstance().save(blob);
        }

    }

    return encoded;
}
