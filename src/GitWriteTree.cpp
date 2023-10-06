#include "index.hpp"

#include <string>
#include <map>

#include "common.hpp"
#include "sha1_proxy.hpp"
#include "objects_proxy.hpp"
#include "GitHashObject.hpp"

struct TreeNode {
    using PathToParent = Path;
    std::map<PathToParent, TreeNode> children;

    TreeItem self_description;

    TreeNode& getOrAdd(const PathToParent &key){
        if (children.find(key) == children.end()) {
            children.emplace(key, TreeNode());
        }
        return children[key];
    }

    bool hasChildren() const {
        return !children.empty();
    }
};



void traversalTreeStructure(TreeNode &root, bool verbose=false) {
    if (!root.hasChildren()) {
        return;
    }
    else {
        Tree tree_object;
        for (auto &pair: root.children) {
            if (verbose) {
                std::cout << "sub-items of " << root.self_description.filename_ << ": " << pair.first << std::endl;
            }
            traversalTreeStructure(pair.second, verbose);
            tree_object.addItem(std::move(pair.second.self_description));
        }
        root.children.clear();

        // write to object database
        SHAString hash = hashObjectInterface(tree_object.freeze(),InArgType::kRawString,ObjectType::kTree,true);
        tree_object.sha1_ = hash;
        // TODO: extension，写入到index中

        // return the result to the caller
        root.self_description.hash_ = hash.data();
        root.self_description.file_mode_ = FileMode::kDirectories;
        root.self_description.object_type_ = ObjectType::kTree;

        if (verbose) {
            std::cout << "Create tree object " << hash << " for folder " << root.self_description.filename_ << std::endl;
        }
    }
}

/* 迭代处理需要做到
 * (1)确定当前目录的子项范围（递归实现）
 * (2)分割当前目录的每一个子目录（无直接API支持）
 *    i.使用filesystem::path，比较次数为n*depth (path为forward iterator)
 *    ii.使用/分割的路径string，比较次数为n*len(filename)
 * 需要处理edge case包括多层目录递归和文件中夹着目录
 * 如 /a/a1 /a/b/b1 /a/b/c/d/e1 /a/d2
 * 总共时间复杂度约为O(n*depth^2)
 *
 * 递归处理
 * (1)建树，时间O(n*depth)
 * (2)遍历，时间=AverageSearchLength=O(n*depth)
 */

SHAString Index::writeTree(bool verbose) {
    TreeNode head;
    for (auto &pair: Index::getInstance().dict_) {
        // the time of rebuilding a tree object and that of checking whether it changes are the same
        if (pair.second->getObjectType() == ObjectType::kTree) {
            continue;
        }

        TreeNode *last = &head;
        for (const Path &sub: pair.first) {
            last = &(last->getOrAdd(sub));
            last->self_description.filename_ = sub; // prepare for tree object
        }
        last->self_description = pair.second->getTreeItem();
    }

    traversalTreeStructure(head, verbose);

    return head.self_description.hash_;
}



//using Iterator = std::map<Path, std::unique_ptr<IndexEntry>>::iterator;
//
//using namespace std::filesystem;
//Option<SHAString> buildFileTrees(Iterator begin, Iterator end, const Path& current_directory) {
//    bool same_dir = true;
//    for (auto it = begin;it != end;++it) {
//        if (!equivalent(it->first.parent_path(),current_directory)) {
//            same_dir = false;
//            break;
//        }
//    }
//
//    Tree tree;
//    // leaf node
//    if (same_dir) {
//        // 含义清晰，不更改begin了
//        for (auto it = begin;it != end;++it) {
//            tree.addItem(it->second->getTreeItem());
//        }
//    }
//    else {
//        // 空文件夹的case会随着index而删除
//        // 使用begin来记录相同子文件夹
//
//        //注意下++
//        bool sub_tree = false;
//        for (auto it = begin;it != end;) {
//            // 相同目录
//            if (equivalent(it->first.parent_path(),current_directory)) {
//                tree.addItem(it->second->getTreeItem());
//                ++it;
//            }
//            else {
//                if (sub_tree) {}
//                else {
//
//                }
//            }
//        }
//    }
//
//    // check
//    for (auto &item: tree.sub_items_) {
//        if (!GitObjectsProxy::getInstance().find(item.hash_)) {
//            // Object not found, throws
//            throw std::logic_error(std::string("Invalid object: ") + item.hash_);
//        }
//    }
//
//    return hashObjectInterface(tree.freeze(), InArgType::kRawString, ObjectType::kTree, true);
//}