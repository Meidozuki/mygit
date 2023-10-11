
#include "objects.hpp"

#include <iomanip>
#include <sstream>
#include <algorithm>

#include "sha1_proxy.hpp"

std::string Tree::freeze() const {
    std::stringstream ss;
    for (auto &item: sub_items_) {
        ss << (unsigned)item.file_mode_ << ' '\
            << item.filename_.generic_string() << '\0'\
            << SHA1Proxy::compressSHA(item.hash_).data();
    }
    return ss.str();
}

std::string Commit::freeze() const {
    std::stringstream ss;
    ss << "tree " << tree_ << '\n';
    if (!parent_.empty()) {
        ss << parent_ << '\n';
    }
    ss << "author " << author_.toString() << "\ncommitter " << committer_.toString() \
        << "\n\n" << message_;
    return ss.str();
}

void Tree::addItem(TreeItem item) {
    // if exists
    if (auto it = std::find(sub_items_.begin(), sub_items_.end(), item);
        it != sub_items_.end()) {
        // if modified
        if (it->hash_ != item.hash_) {
            sub_items_.erase(it);
            sub_items_.emplace_back(std::move(item));
        }
        // not modified
        else {
            return;
        }
    }
    // not exist, add to end
    else {
        sub_items_.emplace_back(std::move(item));
        num_entries_++;
        if (item.object_type_ == ObjectType::kTree) {
            num_subtrees_++;
        }
    }
}

std::string Author::toString() const {
    std::stringstream ss;
    ss << name << " <" << email << "> " << timestamp << ' '\
        << std::setfill('0') << std::setw(4) << time_zone;
    return ss.str();
}