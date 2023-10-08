//
// Created by user on 2023/9/15.
//

#include "objects.hpp"

// #include "index.hpp"

std::string Tree::freeze() const {
    std::stringstream ss;
    for (auto &item: sub_items_) {
        ss << (unsigned)item.file_mode_ << ' '\
               << GitObject::typeName(item.object_type_) << ' '\
               << item.hash_ << ' ' << item.filename_ << '\n';
    }
    return ss.str();
}

void Tree::addItem(const TreeItem &item) {
    // if exists
    if (auto it = std::find(sub_items_.begin(), sub_items_.end(), item);
        it != sub_items_.end()) {
        // if modified
        if (it->hash_ != item.hash_) {
            sub_items_.erase(it);
            sub_items_.push_back(item);
        }
            // not modified
        else {
            return;
        }
    }
        // not exist, add to end
    else {
        sub_items_.push_back(item);
        num_entries++;
        if (item.object_type_ == ObjectType::kTree) {
            num_subtrees++;
        }
    }
}

void GitObjectsManager::save(const GitObject &object)  {
    auto sha1 = object.sha1_;
    Option<decltype(sha1)> hash(sha1);
    auto it = hash.map<IteratorType>([this]
                                         (HashArg x) {return this->insert(x);}
    );

    auto file_path = it.map<std::string>([this]
                                             (const IteratorType &iterator) {return this->getFilePath(iterator);}
    );

    auto file = file_path.flatMap<bool>([&object]
                                            // string can convert to path
                                            (const std::filesystem::path &path)->Option<bool> {
      std::filesystem::create_directories(path.parent_path());
      std::ofstream fs(path);
      if (fs.is_open()) {
          auto &&content = object.freeze();
          fs << object.type() << ' ' << content.size() << '\n' << content;
          return true;
      }
      else {
          return {};
      }
    });

    if (!file.has_value()) {
        std::cerr << "Save file " << std::quoted(sha1) << " failed\n";
    }
}
