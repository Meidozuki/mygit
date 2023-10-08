//
// Created by user on 2023/9/10.
//

#pragma once

#include "precompile.h"

#include <vector>
#include <string>

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
    int num_entries, num_subtrees;
 public:
    std::vector<TreeItem> sub_items_;

    Tree():sub_items_(),num_entries(0),num_subtrees(0) {type_ = ObjectType::kTree;}

    std::string freeze() const override;

    void addItem(TreeItem item);

    void cacheTreeInvalidate() {
        num_entries = -1;
    }
};

struct Author{
    std::string name, email;
    long long int timestamp;
    int time_zone;

    Author(): name(),email(),timestamp(0),time_zone(0) {}

    std::string string() const {
        return name + " <" + email + "> " +\
            std::to_string(timestamp) + " " + std::to_string(time_zone);
    }
};

class Commit: public GitObject {
 public:
    SHAString tree_, parent_;
    Author author, committer;
    std::string message;

    Commit():tree_(),parent_(),author(),committer(),message() {type_ = ObjectType::kCommit;}
    Commit(SHAString tree):Commit() {tree_ = tree;}
    // Commit(SHAString tree, SHAString parent):Commit(tree) {parent_ = parent;}

    std::string freeze() const override {
        using namespace std::literals::string_literals;
        std::string parents = "\n"s;
        if (!parent_.empty()) {
            parents += "parent "s + parents.data() + "\n"s;
        }
        return "tree "s + tree_.data() + parents +\
            "author "s + author.string() + "\ncommitter "s + committer.string() +\
            "\n\n"s + message;
     }
};