#pragma once

#include "precompile.h"

#include <string_view>
#include <cstring>
#include <array>
#include <set>
#include <optional>
#include <stdexcept>

class SHA1Proxy {
 public:
    using SHAString = std::string_view;

    struct StringProxy {
        char str[41];

        StringProxy() = default;
        StringProxy(const char* src) { assign(src);}

        const char* get() const {return &str[0];}

        void assign(const char* src) {
            strncpy(str, src, 41);
        }

        bool operator<(const StringProxy &ano) const {
            return strncmp(str, ano.str, 40) < 0;
        }
    };
 private:
    std::set<StringProxy> existing_;
 public:
    static constexpr inline int hex2dec(char c) {
        switch (c) {
            case '0': return 0;
            case '1': return 1;
            case '2': return 2;
            case '3': return 3;
            case '4': return 4;
            case '5': return 5;
            case '6': return 6;
            case '7': return 7;
            case '8': return 8;
            case '9': return 9;
            case 'a': return 10;
            case 'b': return 11;
            case 'c': return 12;
            case 'd': return 13;
            case 'e': return 14;
            case 'f': return 15;
            default: throw std::invalid_argument("");
        }
    }

    bool find(const StringProxy& str) const {
        return existing_.find(str) != existing_.end();
    }

    SHAString create(const char* hash) {
        auto insert_result = existing_.insert(StringProxy(hash));
        return insert_result.first->get();
    }

    void remove(const char* hash) {
        auto it = existing_.find(StringProxy(hash));
        if (it != existing_.end()) {
            existing_.erase(it);
        }
    }
    
    SHAString findAndGet(const char* hash) {
        auto it = existing_.find(StringProxy(hash));
        if (it != existing_.end()) {
            return it->get();
        }
        else {
            return {};
        }
    }


    // Convert from human-readable string to hex string
    static auto compressSHA(const char* hash) {
        if (strlen(hash) != 40) {
            throw std::invalid_argument("");
        }

        std::array<unsigned char,21> array;
        for (int i=0;i < 20;++i) {
            array[i] = hex2dec(hash[2*i]) * 16 + hex2dec(hash[2*i+1]);
        }
        array[20]='\0';
        return array;
    }

    // Convenient wrappers
    static auto invokeCreate(const char* hash) { return getInstance().create(hash);}
//    static auto invokeRemove(const char* hash) {return getInstance().remove(hash);}
    static auto invokeFindAndGet(const char* hash) {return getInstance().findAndGet(hash);}

    // Singleton declare
    static SHA1Proxy& getInstance() {
        static SHA1Proxy instance;
        return instance;
    }

    SHA1Proxy& operator=(const SHA1Proxy&) = delete;
 private:
    SHA1Proxy() = default;
    ~SHA1Proxy() = default;
};

using SHAString = SHA1Proxy::SHAString;
using StringProxy = SHA1Proxy::StringProxy;