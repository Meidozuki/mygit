//
// Created by user on 2023/9/16.
//

#include "index.hpp"

#include "objects.hpp"

std::ostream &operator<<(std::ostream &os, const Index &index) {
    os << "DIRC\n";
    for (auto &[_, entry]: index.dict_) {
        os << (int) entry->file_mode << ' '\
            << GitObject::typeName(entry->getObjectType())  << ' '\
            << entry->file_size << ' '\
            << entry->sha1 << '\n'\
            << entry->filename << '\n';
    }
    return os;
}
std::istream &operator>>(std::istream &is, Index &index) {
    {
        std::string header;
        is >> header;
        if (header != "DIRC") {
            is.setstate(std::ios::failbit);
            return is;
        }
    }
    // The eofbit needs to be set by peek()
    for (;!is.eof();is.peek()) {
        // Avoid trailing '\n'
        if (is.peek() == '\n') {is.ignore(); continue;}

        int mode_s = 0;
        is >> mode_s;
        // If mode is invalid, others are supposed to be invalid too.
        if (!modeFromInt(mode_s).has_value()) {
            is.setstate(std::ios::failbit);
            return is;
        }

        FileMode mode = modeFromInt(mode_s).value();
        std::string type, hash, filename;
        int file_size;
        is >> type >> file_size >> hash; is.ignore();
        std::getline(is, filename);
        if (filename.size() == 0) {
            std::cerr << "Index entry empty filename: " << type << ' ' << hash << ' ' << filename << '\n';
            is.setstate(std::ios::failbit);
            return is;
        }

        if (mode == FileMode::kDirectories) {
            if (type != "tree") {
                std::cerr << "Index entry wrong format: " << type << ' ' << hash << ' ' << filename << '\n';
                is.setstate(std::ios::failbit);
                return is;
            }
            SHAString sha1(SHA1Proxy::create_s(hash.c_str()));
            index.addEntry(DirectoryFile(mode, file_size, sha1.data(), std::move(filename)));
        }
        else if (mode == FileMode::kRegular || mode == FileMode::kRegularExecutable) {
            if (type != "blob") {
                std::cerr << "Index entry wrong format: " << type << ' ' << hash << ' ' << filename << '\n';
                is.setstate(std::ios::failbit);
                return is;
            }
            SHAString sha1(SHA1Proxy::create_s(hash.c_str()));
            index.addEntry(RegularFile(mode, file_size, sha1.data(), std::move(filename)));
        }
    }
    return is;
}
