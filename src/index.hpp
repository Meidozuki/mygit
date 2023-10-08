//
// Created by user on 2023/9/15.
//

#pragma once

#include "precompile.h"

#include <filesystem>
#include <map>
#include <memory>
#include <utility>

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
        : RegularFile(FileMode::kRegular, 0, sha,std::move(fname)) {}
    
    DryRunFile(SHAString sha, VariableString fname): DryRunFile(sha.data(), std::move(fname)) {}
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
 private:
    std::map<SHAString, std::unique_ptr<IndexEntry>> dict_;
    void insert(SHAString const& key, std::unique_ptr<IndexEntry>&& value) {
        dict_.emplace(key, std::move(value));
    }
 public:
    static Index& getInstance() {
        static Index instance;
        return instance;
    }

    const auto& getDict() {return dict_;}

    template <typename T, std::enable_if_t<
                          std::is_base_of_v<IndexEntry, T>>>
    void addEntry(T &&val) {
        insert(val.sha1, std::make_unique<T>(std::forward(val)));
    }

    void updateIndexCacheInfo(FileMode mode, const char *hash, std::filesystem::path path);

    void updateIndex(const Path& filename);
    void updateIndex(const Path& filename, std::error_code &ec) noexcept;


    Index& operator=(const Index &ano) = delete;
 private:
    Index() = default;
    ~Index() = default;

    friend std::ostream& operator<<(std::ostream& os, const Index &index);
    friend std::istream& operator>>(std::istream& is, Index &index);
};
