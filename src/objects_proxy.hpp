#pragma once

#include "../precompile.h"

#include <set>
#include <filesystem>

#include "common.hpp"
#include "objects.hpp"
#include "sha1_proxy.hpp"
#include "FP_util.hpp"

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

    [[gnu::const]]
    static inline Path getGitDir() {return GIT_DIR;}
    [[gnu::const]]
    static inline Path getObjectsDir() {return GIT_DIR / "objects/";}

    [[gnu::always_inline]]
    bool find(HashArg hash) const {
        return database_.find(hash) != database_.end();
    }

    /**
     * @param hash 
     * @return The iterator to the hash
     */
    auto insert(HashArg hash) {
        auto insert_result = database_.insert(hash);
        return insert_result.first;
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

    void initFromDisk() {
        const Path objects_dir(getObjectsDir());
        if (filesys::exists(objects_dir)) {
            for (auto &entry: filesys::directory_iterator(objects_dir)){
                insert(entry.path().filename().string());
            }
        }
    }

    [[deprecated]]
    void save(const GitObject &object) {
        Option<SHAString> hash(object.sha1_);
        auto it = hash.map<IteratorType>(
            [this](HashArg x) 
            {return this->insert(x);}
        );

        auto file_path = it.flatMap<std::string>(
            [this](const IteratorType &iterator)
             {return this->getFilePath(*iterator);}
        );

        auto file = file_path.flatMap<bool>(
            // string can convert to path
            [&object](const std::filesystem::path &path)->Option<bool> {
            std::filesystem::create_directories(path.parent_path());
            std::ofstream fs(path);
            if (fs.is_open()) {
                auto &&content = object.freeze();
                fs << object.type() << ' ' << content.size() << '\n' << content;
                return true;
            }
            else {
                return {};
            }
        });

        if (!file.has_value()) {
            std::cerr << "Save file " << std::quoted(object.sha1_) << " failed\n";
        }
    }

    GitObjectsProxy& operator=(const GitObjectsProxy &ano) = delete;
 private:
    GitObjectsProxy() = default;
    ~GitObjectsProxy() = default;
};