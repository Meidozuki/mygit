#pragma once

#include <string>
#include <fstream>

#include "sha1_proxy.hpp"
#include "objects_proxy.hpp"

inline void printLog() {
    std::string s;
    std::ifstream(GitObjectsProxy::getHeadPath()) >> s;
    if (!s.empty() && GitObjectsProxy::getInstance().find(s)) {
        SHAString hash = SHA1Proxy::invokeCreate(s.c_str());

        auto current = GitObjectsProxy::getInstance().readCommitObject(hash);
        while (current.nonEmpty()) {
            auto &commit = current.value();
            std::cout << "commit " << commit.sha1_\
            << "\nAuthor: " << commit.author_.name
                      << "\nDate:   " << commit.author_.timestamp
                      << "\n\n\t" << commit.message_ << std::endl;

            current = GitObjectsProxy::getInstance().readCommitObject(commit.parent_);
        }
    }
}