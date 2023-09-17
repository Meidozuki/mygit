//
// Created by user on 2023/9/16.
//

#include "index.hpp"

#include "GitHashObject.hpp"

void Index::updateIndex(const Path &filename, std::error_code &ec) noexcept {
    namespace filesys = std::filesystem;
    if (filesys::exists(filename, ec)) {
        if (filesys::is_regular_file(filename, ec)) {
            RegularFile file;
            file.file_mode = FileMode::kRegular;
            file.file_size = filesys::file_size(filename, ec);
            file.filename = filename.string();

            using filesys::perms;
            perms permission = filesys::status(filename).permissions();
            if ((permission & perms::owner_exec) != perms::none){
                file.file_mode = FileMode::kRegularExecutable;
            }

            std::string content = readFile(filename);
            std::string sha1 = hashObject(content);
            //TODO
        }
        else if (filesys::is_directory(filename, ec)) {
            DirectoryFile file;
            file.filename = filename.string();
            //TODO tree hash
        }
    }
}
