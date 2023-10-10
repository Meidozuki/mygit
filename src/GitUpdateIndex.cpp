#include "GitUpdateIndex.hpp"

#include <stdexcept>
#include <functional>
#include <utility>

#include "objects.hpp"
#include "GitHashObject.hpp"

using FuncWithNoArg = std::function<void()>;

struct Throwing{
    std::string msg;
    Throwing() = default;
    Throwing(std::string s):msg(std::move(s)) {}

    [[noreturn]]
    void operator()() const {throw std::logic_error(msg);}
};

struct IndexAddStrategy{
    Index &index_;
    std::function<RegularFile()> entry_fn_;
    IndexAddStrategy(Index &index, std::function<RegularFile()> entry_fn): index_(index), entry_fn_(std::move(entry_fn)) {}

    void operator()() {
        index_.addEntry(entry_fn_());
    }
};

struct IndexRemoveStrategy{
    Index &index_;
    IndexRemoveStrategy(Index &index): index_(index) {}

    using RemoveOptionArg = const Path&;
    void operator()(const Path &path_) {
        index_.removeEntry(path_);
    }
};

class Context {
 private:
    using RemoveOptionArg = IndexRemoveStrategy::RemoveOptionArg;
    Index &index_;
 public:
    explicit Context(Index &index): index_(index) {}
    bool remove, add;

    auto getFileNotExistFn()->std::function<void(RemoveOptionArg)> {
        if (remove) {
            return IndexRemoveStrategy(index_);
        }
        else {
            return [](RemoveOptionArg path) {
              std::invoke(Throwing(path.string() + " file doesn't exist and --remove not set."));
            };
        }
    };

    auto getEntryNotExistLazyFn(std::function<RegularFile()> entry_fn, const Path &path)->std::function<void()> {
        if (add) {
            return IndexAddStrategy(index_, std::move(entry_fn));
        }
        else {
            return Throwing(path.string() + " entry is not in index and --add not set.");
        }
    }
};


void updateIndex(Index &index, const Path &filename, bool add, bool remove) {
    Context context(index);
    context.add = add;
    context.remove = remove;
    if (filesys::exists(filename)) {
        if (filesys::is_regular_file(filename)) {
            auto createEntry = [&filename](){
              using filesys::perms;
              RegularFile file;
              file.filename = filename.string();
              file.file_size = filesys::file_size(filename);
              file.file_mode = FileMode::kRegular;

              // Linux version only.
              perms permission = filesys::status(filename).permissions();
              if ((permission & perms::owner_exec) != perms::none){
                  file.file_mode = FileMode::kRegularExecutable;
              }

              file.sha1 = hashObjectInterface(filename.string(), InArgType::kFilename, ObjectType::kBlob, true).data();

              return file;
            };

            if (index.find(filename)){
                // [] only allowed in non-const std::map
                auto &ptr = index.getDict().at(filename);
                // expect both to be file entries
                RegularFile file = createEntry();
                ptr->sha1 = file.sha1;
                ptr->file_mode = file.file_mode;
                ptr->file_size = file.file_size;
            }
            else {
                context.getEntryNotExistLazyFn(createEntry, filename)();
            }
        }
        else if (filesys::is_directory(filename)) {
            throw std::logic_error("update-index receives a directory - add files inside instead");
        }
    }
    else {
        context.getFileNotExistFn()(filename);
    }
}



void updateIndexCacheInfo(Index &index, FileMode mode, const char *hash, Path filename, bool add, bool remove){
    Context context(index);
    context.add = add;
    context.remove = remove;

    if (!SHA1Proxy::find_s(hash)) {
        std::string msg = "invalid SHA1 value ";
        throw std::invalid_argument(msg + hash);
    }

    if (index.find(filename)){
        auto &ptr = index.getDict().at(filename);
        ptr->file_mode = mode;
        ptr->sha1 = hash;
    }
    else {
        auto createEntry = [&]() {
            return RegularFile(mode, 0, SHA1Proxy::create_s(hash).data(), filename.string());
        };
        context.getEntryNotExistLazyFn(createEntry, filename)();
    }
}