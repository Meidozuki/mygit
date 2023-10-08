//
// Created by user on 2023/9/15.
//

#pragma once

#include "precompile.h"

#include <filesystem>
#include <map>
#include <memory>
#include <utility>
#include <type_traits>

#include "common.hpp"
#include "sha1_proxy.hpp"
#include "objects.hpp"


struct IndexEntry {
    using FileSizeType = std::size_t;

    FileMode file_mode;
    FileSizeType file_size;
    const char* sha1;
    VariableString filename;

    IndexEntry() = default;
    IndexEntry(FileMode mode, FileSizeType size, const char *sha, VariableString fname)
        : file_mode(mode), file_size(size), sha1(sha), filename(std::move(fname)) {}

    virtual ObjectType getObjectType() const = 0;

    bool operator==(const IndexEntry& ano) const {return filename == ano.filename;}
    bool operator<(const IndexEntry& ano) const {return filename < ano.filename;}

    TreeItem getTreeItem() const {
        TreeItem t{filename, sha1, file_mode, getObjectType()};
        return t;
    }

    void chmod(bool isExec) {
        if (file_mode == FileMode::kRegular && isExec) {
            file_mode = FileMode::kRegularExecutable;
        }
        else if (file_mode == FileMode::kRegularExecutable && !isExec) {
            file_mode = FileMode::kRegular;
        }
    }
};


struct RegularFile: IndexEntry {
    using IndexEntry::IndexEntry;
    [[gnu::const]]
    ObjectType getObjectType() const override {
        return ObjectType::kBlob;
    }
};

struct DryRunFile: RegularFile {
    DryRunFile(const char *sha, VariableString fname)
        : RegularFile(FileMode::kRegular, 0, sha, std::move(fname)) {}
    
    DryRunFile(SHAString sha, VariableString fname)
        : DryRunFile(sha.data(), std::move(fname)) {}
};

struct DirectoryFile: IndexEntry {
    using IndexEntry::IndexEntry;
    DirectoryFile() {
        file_size = 0;
        file_mode = FileMode::kDirectories;
    }
    DirectoryFile(VariableString s):DirectoryFile() {
        filename = std::move(s);
    }

    [[gnu::const]]
    ObjectType getObjectType() const override {
        return ObjectType::kTree;
    }
};

class Index{
 protected:
    std::map<Path, std::unique_ptr<IndexEntry>> dict_;
    bool suppress_warning;
 public:
    static Index& getInstance() {
        static Index instance;
        return instance;
    }

    const auto& getDict() {return dict_;}

    // BE CAREFUL WITH IT!
    void clean() {dict_.clear();}
    bool initFromDisk(const Path& index_path) {
        std::ifstream fs(index_path);
        if (fs.is_open()) {
            fs >> *this;
            return !fs.fail();
        }
        else {
            return false;
        }
    }

    bool find(const Path& x) {return dict_.find(x) != dict_.end();}

    template <typename T, std::enable_if_t<
                          std::is_base_of_v<IndexEntry, std::remove_reference_t<T>>, bool> = true>
    bool addEntry(T entry) {
        Path t(entry.filename);
        return insert(std::move(t), std::make_unique<T>(std::move(entry)));
    }

    Index& operator=(const Index &ano) = delete;
 protected:
    Index():dict_(),suppress_warning(false) {}
    ~Index() = default;

    /**
     * The proxy to insert items to the dict.
     * WARNING: Don't insert entry with it unless you are sure. Use addEntry instead.
     * @param key a rvalue std::string
     * @param value a unique_ptr, which is rvalue only
     */
    bool insert(Path &&key, std::unique_ptr<IndexEntry>&& value) {
        if (!filesys::exists(key) && !suppress_warning) {
            std::cerr << "Trying to add a non-existing filepath " << key << " to index\n";
        }
        auto &&result = dict_.emplace(std::move(key), std::move(value));
        return result.second;
    }

    friend std::ostream& operator<<(std::ostream& os, const Index &index);
    friend std::istream& operator>>(std::istream& is, Index &index);
};
