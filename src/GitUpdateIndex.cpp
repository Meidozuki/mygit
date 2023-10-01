#include "GitUpdateIndex.hpp"

#include "index.hpp"
#include "objects.hpp"
#include "GitHashObject.hpp"

static inline Index& index() {
    return Index::getInstance();
}

void updateIndex(const Path &filename, std::error_code &ec) noexcept {
    if (filesys::exists(filename, ec)) {
        if (filesys::is_regular_file(filename, ec)) {
            using filesys::perms;
            RegularFile file;
            file.filename = filename.string();
            file.file_size = filesys::file_size(filename, ec);
            file.file_mode = FileMode::kRegular;

            perms permission = filesys::status(filename).permissions();
            if ((permission & perms::owner_exec) != perms::none){
                file.file_mode = FileMode::kRegularExecutable;
            }

            SHAString sha1 = hashObjectInterface(filename.string(), InArgType::kFilename, ObjectType::kBlob, true);
            file.sha1 = sha1.data();

            index().addEntry(std::move(file));
        }
        else if (filesys::is_directory(filename, ec)) {
            DirectoryFile file;
            file.filename = filename.string();
            //TODO tree hash
        }
    }
}

void updateIndexCacheInfo(FileMode mode, const char *hash, std::filesystem::path path, bool add){
    if (!SHA1Proxy::find_s(hash)) {
        std::cerr << "invalid SHA1 value " << hash << '\n';
        return;
    }

    if (index().find(path)){
        auto &ptr = index().getDict().at(path);
        ptr->file_mode = mode;
        ptr->sha1 = hash;
    }
    else {
        if (add) {
            index().addEntry(RegularFile(mode, 0, SHA1Proxy::create_s(hash).data(), path.string()));
        }
        else {
            std::cerr << path << ": cannot add to the index - missing --add option?" << '\n';
            return;
        }
    }
}