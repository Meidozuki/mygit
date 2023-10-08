#pragma once

#include "objects.hpp"
#include "sha1_proxy.hpp"
#include "FP_util.hpp"

class GitObjectsManager {
 public:
    using HashType = SHAString;
    using HashArg = const HashType&;
 private:
    std::set<HashType> set_;
    using IteratorType = typename decltype(set_)::iterator;

 public:
    static GitObjectsManager& getInstance() {
        static GitObjectsManager instance;
        return instance;
    }

    [[gnu::always_inline]]
    bool find(HashArg hash) const {
        return set_.find(hash) != set_.end();
    }

    /**
     * 
     * @param hash 
     * @return The iterator to the hash
     */
    auto insert(HashArg hash) {
        auto insert_result = set_.insert(hash);
        return insert_result.first;
    }

    Option<std::string> getFilePath(HashArg hash) const {
        if (find(hash)) {
            Option<std::string> path(".mgit/objects/");
            path.value().append(hash);
            return path;
        }
        else {
            return {};
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


    GitObjectsManager& operator=(const GitObjectsManager &ano) = delete;
 private:
    GitObjectsManager() = default;
    ~GitObjectsManager() = default;
};