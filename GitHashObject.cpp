//
// Created by user on 2023/9/3.
//

#include "GitHashObject.hpp"

#include "cryptopp/cryptlib.h"
#include "cryptopp/sha.h"
#include "cryptopp/hex.h"

StringType HashObject(const SV msg) {
    using namespace CryptoPP;
    SHA1 sha1;
    std::string binary_digest, encoded;

    static_assert(std::is_same_v<const char*, decltype(msg.data())>);
    sha1.Update((const byte*)msg.data(), msg.size());
    binary_digest.resize(sha1.DigestSize());
    sha1.Final((byte*)binary_digest.c_str());

    bool uppercase = false;
    StringSource ss(binary_digest.data(), true, new HexEncoder(
            new StringSink(encoded), uppercase
            ));

    static_assert(std::is_same<std::string, StringType>::value);
    return encoded;
}