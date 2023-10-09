#include "GitUpdateIndex.hpp"

#include <stdexcept>
#include <functional>

#include "objects.hpp"
#include "GitHashObject.hpp"

using FuncWithNoArg = std::function<void()>;

void updateIndex(Index &index, const Path &filename, std::error_code &ec) {
    if (filesys::exists(filename, ec)) {
        if (filesys::is_regular_file(filename, ec)) {
            using filesys::perms;
            RegularFile file;
            file.filename = filename.string();
            file.file_size = filesys::file_size(filename, ec);
            file.file_mode = FileMode::kRegular;
            
            // Linux version only.
            perms permission = filesys::status(filename).permissions();
            if ((permission & perms::owner_exec) != perms::none){
                file.file_mode = FileMode::kRegularExecutable;
            }

            SHAString sha1 = hashObjectInterface(filename.string(), InArgType::kFilename, ObjectType::kBlob, true);
            file.sha1 = sha1.data();

            index.addEntry(std::move(file));
        }
        else if (filesys::is_directory(filename, ec)) {
            throw std::logic_error("update-index receives a directory - add files inside instead");
        }
    }
    else {
        std::string msg = filename.string() + " does not exist and --remove not passed";
        throw std::logic_error(msg);
    }
}

struct Throwing{
    std::string msg;
    Throwing() = default;
    Throwing(std::string s):msg(std::move(s)) {}

    [[noreturn]]
    void operator()() const {throw std::logic_error(msg);}
};

struct IndexAddStrategy{
    Index &index_;
    RegularFile entry_;
    IndexAddStrategy(Index &index,RegularFile entry):index_(index),entry_(std::move(entry)) {}
    void operator()() {
        index_.addEntry(std::move(entry_));
    }
};

struct IndexRemoveStrategy{
    Index &index_;
    Path &path_;
    IndexRemoveStrategy(Index &index, Path &path) : index_(index), path_(path) {}
    void operator()() {
        index_.erase(path_);
    }
};


void updateIndexCacheInfo(Index &index, FileMode mode, const char *hash, std::filesystem::path path, bool add){
    auto index_entry_not_found_fn = [&]() -> FuncWithNoArg{
        if (add) {
            return IndexAddStrategy(index, RegularFile(mode, 0, SHA1Proxy::create_s(hash).data(), path.string()));
        }
        else {
            return Throwing(path.string() + " entry is not in index and --add not set.");
        }
    }();

    bool remove;
    auto file_not_exist_fn = [&]() -> FuncWithNoArg{
        if (remove) {
            return IndexRemoveStrategy(index, path);
        }
        else {
            return Throwing(path.string() + " file doesn't exist and --remove not set.");
        }
    }();


    if (!SHA1Proxy::find_s(hash)) {
        std::string msg = "invalid SHA1 value ";
        throw std::invalid_argument(msg + hash);
    }

    if (index.find(path)){
        auto &ptr = index.getDict().at(path);
        ptr->file_mode = mode;
        ptr->sha1 = hash;
    }
    else {
        index_entry_not_found_fn();
    }
}