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

        Commit current;
        std::cout << "commit " << current.sha1_\
            << "\nAuthor: " << current.author_.name
                  << "\nDate:   " << current.author_.timestamp
                  << "\n\n\t" << current.message_ << std::endl;
    }
}