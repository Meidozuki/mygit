#pragma once

#include "precompile.h"

#include <string>
#include <set>
#include <filesystem>

#include "common.hpp"
#include "objects.hpp"
#include "sha1_proxy.hpp"
#include "FP_util.hpp"
#include "GitHashObject.hpp"

// GitObjectsProxy为本地文件和database的一一对应，本地文件不存在则无法加入
class GitObjectsProxy {
 public:
    using HashType = SHAString;
 protected:
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
                insertNoCheck(entry.path().filename().string());
            }
        }
    }

    void resetMemory() {
        database_.clear();
    }

    // CRUD
 protected:
    /**
     * @brief Insert SHA1 into database
     * @param hash SHA1 value pointing to an existing file
     * @return The iterator to the hash
     */
    IteratorType insertNoCheck(SHAString hash) {
        auto insert_result = database_.insert(hash);
        return insert_result.first;
    }

    Path getFilePathNoCheck(SHAString hash) const {
        return getObjectsDir() / hash;
    }

    bool eraseOnlyMemory(SHAString hash) {
        auto it = database_.find(hash);
        if (it != database_.end()) {
            database_.erase(it);
            return true;
        }
        else {
            return false;
        }
    }

    friend SHAString hashObjectInterface(StringView msg, InArgType arg_type, ObjectType type, bool if_write);

 public:
    /**
     * @brief Insert SHA1 into database. Checks if the file exists
     * @param hash SHA1 value pointing to an existing file
     * @return The cref to the hash. If file not exists, throw
     */
    const SHAString& insert(SHAString hash) {
        if (filesys::exists(getFilePathNoCheck(hash))) {
            return *insertNoCheck(hash);
        }
        else{
            auto msg = std::string("Trying to insert a non-existing object sha1 ") + hash.data() + '\n';
            throw std::logic_error(msg);
        }
    }

    bool erase(SHAString hash) {
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
    bool find(SHAString hash) const {
        return database_.find(hash) != database_.end();
    }

    // Other functions
    Option<std::string> getFilePath(SHAString hash) const {
        if (find(hash)) {
            Option<std::string> path{(GIT_DIR / "objects/").string()};
            path.value().append(hash);
            return path;
        }
        else {
            return {};
        }
    }

    ObjectType getObjectType(SHAString hash) const;

    ObjectType getObjectTypeNoThrow(SHAString hash) const noexcept;


    Option<std::stringstream> readObject(SHAString hash) const {
        auto path = getFilePath(hash);
        auto content = path.map<std::stringstream>([](const std::string& path){
          return readFileAsStream(path);
        });
        return content;
    }

    Option<std::stringstream> readObjectNoHeader(SHAString hash) const;

    Option<Commit> readCommitObject(SHAString hash);


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
 protected:
    GitObjectsProxy() = default;
    ~GitObjectsProxy() = default;
};
