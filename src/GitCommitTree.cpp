
#include "GitCommitTree.hpp"

#include <stdexcept>

#include "objects.hpp"
#include "GitHashObject.hpp"

Author getDefaultAuthor() {
    Author temp;
    temp.name="DEFAULT_USER";
    temp.email="DEFAULT_EMAIL";
    temp.timestamp=666666;
    temp.time_zone={8,0};
    return temp;
}

SHAString commitTree(std::string message, SHAString tree, Option<SHAString> parent) {
    Commit commit(tree);
    if (parent.has_value()) {
        commit.parent_ = parent.value();
    }

    commit.author_ = commit.committer_ = getDefaultAuthor();
    if (message.empty()) {
        throw std::invalid_argument("commit message cannot be empty.");
    }
    commit.message_ = std::move(message);

    return hashObjectInterface(commit.freeze(), InArgType::kRawString, ObjectType::kCommit, true);
}