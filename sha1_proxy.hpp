//
// Created by user on 2023/9/16.
//
#pragma once

#include "precompile.h"

#include <string_view>
#include <cstring>
#include <set>
#include <optional>

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

    bool find(const StringProxy& str) const {
        return existing_.find(str) != existing_.end();
    }

    SHAString create(const char* hash) {
        StringProxy t(hash);
        if (auto it = retrieve(t);
            it.has_value()){
            return it.value()->get();
        }
        else {
            existing_.insert(t);
            return retrieve(hash).value()->get();
        }
    }

    void remove(const char* hash) {
        StringProxy t(hash);
        if (auto it = retrieve(t);
            it.has_value()) {
            existing_.erase(it.value());
        }
    }

 private:
    std::set<StringProxy> existing_;

 public:
    std::optional<decltype(existing_)::iterator>
    retrieve(const StringProxy& str) {
        auto it = existing_.find(str);
        if (it != existing_.end()) {
            return it;
        }
        else {
            return {};
        }
    }


    // Convenient wrappers
    // C++禁止这种重载，所以使用后缀_s区分静态函数
    static auto create_s(const char* hash) { return getInstance().create(hash);}
    static auto remove_s(const char* hash) {return getInstance().remove(hash);}
    static auto find_s(const StringProxy& str) {return getInstance().find(str);}

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