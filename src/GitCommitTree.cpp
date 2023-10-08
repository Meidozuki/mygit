
#include "GitCommitTree.hpp"

#include <stdexcept>

#include "objects.hpp"
#include "GitHashObject.hpp"

Author getDefaultAuthor() {
    Author temp;
    temp.name="DEFAULT_USER";
    temp.email="DEFAULT_EMAIL";
    temp.timestamp=666666;
    temp.time_zone=8;
    return temp;
}

SHAString commitTree(SHAString tree, Option<SHAString> parent, std::string message) {
    Commit commit(tree);
    if (parent.has_value()) {
        commit.parent_ = parent.value();
    }

    commit.author = commit.committer = getDefaultAuthor();
    if (message.empty()) {
        throw std::invalid_argument("commit message cannot be empty.");
    }
    commit.message = std::move(message);

    return hashObjectInterface(commit.freeze(), InArgType::kRawString, ObjectType::kCommit, true);
}