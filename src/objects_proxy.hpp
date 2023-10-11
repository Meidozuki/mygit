#pragma once

#include "precompile.h"

#include <set>
#include <filesystem>

#include "common.hpp"
#include "objects.hpp"
#include "sha1_proxy.hpp"
#include "FP_util.hpp"
#include "GitHashObject.hpp"

class GitObjectsProxy {
 public:
    using HashType = SHAString;
    using HashArg = const HashType&;
 private:
    static inline const Path GIT_DIR = ".mgit/"; // warning针对Path的构造函数没有noexcept，可无视
    std::set<HashType> database_;
    using IteratorType = typename decltype(database_)::iterator;

 public:
    static GitObjectsProxy& getInstance() {
        static GitObjectsProxy instance;
        return instance;
    }

    // configure
    [[gnu::const]]
    static inline Path getGitDir() {return GIT_DIR;}
    [[gnu::const]]
    static inline Path getObjectsDir() {return GIT_DIR / "objects/";}
    [[gnu::const]]
    static inline Path getRefsDir() {return GIT_DIR / "refs/";}
    [[gnu::const]]
    static inline Path getHeadPath() {return GIT_DIR / "HEAD";}

    // initialize
    void initFromDisk() {
        const Path objects_dir(getObjectsDir());
        if (filesys::exists(objects_dir)) {
            for (auto &entry: filesys::directory_iterator(objects_dir)){
                insert(entry.path().filename().string());
            }
        }
    }

    // CRUD
    /**
     * @param hash
     * @return The iterator to the hash
     */
    IteratorType insert(HashArg hash) {
        auto insert_result = database_.insert(hash);
        return insert_result.first;
    }

    bool erase(HashArg hash) {
        auto path = getFilePath(hash);
        auto exists = path.map<bool>([](const Path &path){return filesys::exists(path);});
        if (exists.has_value() && exists.get()) {
            filesys::remove(path.get());
        }

        auto it = database_.find(hash);
        if (it != database_.end()) {
            database_.erase(it);
            return true;
        }
        else {
            return false;
        }
    }

    [[gnu::always_inline]]
    bool find(HashArg hash) const {
        return database_.find(hash) != database_.end();
    }


    Option<std::string> getFilePath(HashArg hash) const {
        if (find(hash)) {
            Option<std::string> path{(GIT_DIR / "objects/").string()};
            path.value().append(hash);
            return path;
        }
        else {
            return {};
        }
    }

    ObjectType getObjectType(SHAString hash) const {
        auto path = getFilePath(hash);
        if (path.has_value()) {
            auto ss = readFileAsStream(path.value());
            std::string type_s;
            int n;
            if (ss >> type_s >> n) {
                if (auto type = objectTypeFromString(type_s);
                    type.has_value()) {
                    return type.value();
                }
            }
            throw std::logic_error("cannot recognize object type");
        }
        else {
            throw std::invalid_argument(std::string("cannot find object ") + hash.data());
        }
    }

    ObjectType getObjectTypeNoExcept(SHAString hash) const noexcept {
        auto path = getFilePath(hash);
        if (path.has_value()) {
            auto ss = readFileAsStream(path.value());
            std::string type_s;
            int n;
            if (ss >> type_s >> n) {
                if (auto type = objectTypeFromString(type_s);
                    type.has_value()) {
                    return type.value();
                }
            }
        }
        return ObjectType::kUnknownObject;
    }

    Option<std::string> catFile(SHAString hash) const {
        auto path = getFilePath(hash);
        auto content = path.map<std::string>([](const std::string& path){
            return readFile(path);
            });
        return content;
    }


    /**
     * convenient wrapper to hash an already-built object (i.e. tree object).
     * equals `git hash-object -w --stdin`
     */
    static SHAString hashObjectWrapper(const GitObject &object) {
        return hashObjectInterface(object.freeze(),
                                   InArgType::kRawString,
                                   object.type_,
                                   true);
    }

    GitObjectsProxy& operator=(const GitObjectsProxy &ano) = delete;
 private:
    GitObjectsProxy() = default;
    ~GitObjectsProxy() = default;
};
