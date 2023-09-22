//
// Created by user on 2023/9/16.
//

#include "index.hpp"

#include "GitHashObject.hpp"
#include "objects.hpp"

using std::unique_ptr;

void Index::updateIndex(const Path &filename, std::error_code &ec) noexcept {
    if (filesys::exists(filename, ec)) {
        if (filesys::is_regular_file(filename, ec)) {
            auto file = std::make_unique<RegularFile>();
            file->file_mode = FileMode::kRegular;
            file->file_size = filesys::file_size(filename, ec);
            file->filename = filename.string();

            using filesys::perms;
            perms permission = filesys::status(filename).permissions();
            if ((permission & perms::owner_exec) != perms::none){
                file->file_mode = FileMode::kRegularExecutable;
            }

            SHAString sha1 = hashObjectInterface(filename.string(), InArgType::kFilename, ObjectType::kBlob, true);
            // Add an item to the dict
            dict_.emplace(sha1, std::move(file));
        }
        else if (filesys::is_directory(filename, ec)) {
            DirectoryFile file;
            file.filename = filename.string();
            //TODO tree hash
        }
    }
}
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
    while (!is.eof()) {
        int mode_s;
        // Avoid trailing '\n'
        if (!(is >> mode_s)) {
            break;
        }
        // If mode is invalid, others are supposed to be invalid too.
        if (!modeFromInt(mode_s).has_value()) {
            is.setstate(std::ios::failbit);
            return is;
        }

        FileMode mode = modeFromInt(mode_s).value();
        std::string type, hash, filename;
        int file_size;
        is >> type >> file_size >> hash; is.ignore();
        std::getline(is, filename); is.ignore();
        if (mode == FileMode::kDirectories) {
            if (type != "tree") {
                std::cerr << "Index entry wrong format: " << type << ' ' << hash << ' ' << filename << '\n';
                continue;
            }
            SHAString sha1(SHA1Proxy::create_s(hash.c_str()));
            auto &&entry = std::make_unique<DirectoryFile>(mode, file_size, sha1.data(), std::move(filename));
            index.insert(sha1, std::move(entry));
        }
        else if (mode == FileMode::kRegular || mode == FileMode::kRegularExecutable) {
            if (type != "blob") {
                std::cerr << "Index entry wrong format: " << type << ' ' << hash << ' ' << filename << '\n';
                continue;
            }
            SHAString sha1(SHA1Proxy::create_s(hash.c_str()));
            auto &&entry = std::make_unique<RegularFile>(mode, file_size, sha1.data(), std::move(filename));
            index.insert(sha1, std::move(entry));
        }
    }
    return is;
}
