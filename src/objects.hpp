
#pragma once

#include "precompile.h"

#include <vector>
#include <string>
#include <chrono>

#include "common.hpp"
#include "sha1_proxy.hpp"

class GitObject {
 public:
    SHAString sha1_;
    ObjectType type_;
    // get the content to write to disk
    virtual std::string freeze() const = 0;

    const char* type() const {
        return typeName(type_);
    }

    static const char* typeName(ObjectType type) {
        switch (type) {
            case ObjectType::kBlob: return "blob";
            case ObjectType::kTree: return "tree";
            case ObjectType::kCommit: return "commit";
            case ObjectType::kTag: return "tag";
            default: throw std::logic_error("Uncaught git object type!");
        }
    }
};

class Blob: public GitObject {
 public:
     VariableString content_;

     Blob():content_() {type_ = ObjectType::kBlob;}

     std::string freeze() const override {
         return content_;
     }
};

struct TreeItem {
    Path filename_;
    const char *hash_;
    FileMode file_mode_;
    ObjectType object_type_;

    TreeItem() = default;
    TreeItem(Path filename, const char *hash, FileMode mode, ObjectType type)
        :filename_(std::move(filename)), hash_(hash), file_mode_(mode), object_type_(type) {}

    // std::filesystem::path is not trivially-copyable so needs to declare these ops
//    TreeItem(const TreeItem&) = default;
//    TreeItem& operator=(const TreeItem&) = default;
    TreeItem(TreeItem&&) = default;
    TreeItem& operator=(TreeItem&&) = default;

    bool operator==(const TreeItem &ano) const {return filename_ == ano.filename_;}
};

class Tree: public GitObject {
 private:
    int num_entries_, num_subtrees_;
 public:
    std::vector<TreeItem> sub_items_;

    Tree(): sub_items_(), num_entries_(0), num_subtrees_(0) { type_ = ObjectType::kTree;}

    std::string freeze() const override;

    void addItem(TreeItem item);

    void cacheTreeInvalidate() {
        // TODO: impl index cache tree extension
        num_entries_ = -1;
    }
};

struct Author{
    std::string name, email;
    long long int timestamp;
    TimeZone time_zone;

    Author(): name(),email(),timestamp(0),time_zone() {}

    friend std::ostream& operator<<(std::ostream& os, const Author &author);
    friend std::istream& operator>>(std::istream& is, Author &author);
};

class Commit: public GitObject {
 public:
    SHAString tree_, parent_;
    Author author_, committer_;
    std::string message_;

    Commit(): tree_(), parent_(), author_(), committer_(), message_() {type_ = ObjectType::kCommit;}
    Commit(SHAString tree):Commit() {tree_ = tree;}
    Commit(SHAString tree, SHAString parent):Commit(tree) {parent_ = parent;}

    std::string freeze() const override;

    inline bool hasParent() {
        return !parent_.empty() && parent_ != "nan";
    }
};