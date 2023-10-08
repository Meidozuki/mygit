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


StringType hashObject(const SV msg) {
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

    static_assert(std::is_same<std::string, StringType>::value);
    return encoded;
}

StringType hashObjectInterface(const SV msg, InArgType arg_type, ObjectType type, bool if_write) {
    std::string original_str;
    if (arg_type == InArgType::kRawString) {
        original_str = msg;
    }
    else if (arg_type == InArgType::kFilename) {
        original_str = readFile(msg);
    }
    else {
        throw std::domain_error("");
    }

    StringType encoded(hashObject(original_str));

    if (if_write) {
        // write to file
    }

    return encoded;
}
