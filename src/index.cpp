//
// Created by user on 2023/9/16.
//

#include "index.hpp"

#include "GitHashObject.hpp"

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
