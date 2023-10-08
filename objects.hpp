//
// Created by user on 2023/9/10.
//

#pragma once

#include "precompile.h"

#include <vector>
#include <array>
#include <string>
#include <set>
#include <filesystem>
#include <optional>

#include "FP_util.hpp"
#include "common.hpp"

class GitObject {
 public:
    std::string sha1_;
    ObjectType type_;
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
            default: throw std::logic_error("Uncaught git object type! (in GitObject::type())");
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

    bool operator==(const TreeItem &ano) const {return filename_ == ano.filename_;}
};

class Tree: public GitObject {
 private:
    int num_entries, num_subtrees;
 public:
    std::vector<TreeItem> sub_items_;

    Tree():sub_items_(),num_entries(0),num_subtrees(0) {type_ = ObjectType::kTree;}

    std::string freeze() const override;

    void addItem(const TreeItem &item);
};
