#include <iostream>
#include <string>
#include <optional>
#include <string_view>
#include <memory>
#include <type_traits>
#include <set>
#include <array>
#include <sstream>
using namespace std::string_view_literals;

#include "FP_util.hpp"
#include "objects.hpp"
#include "GitHashObject.hpp"
#include "common.hpp"
#include "objects_proxy.hpp"
#include "GitUpdateIndex.hpp"
#include "GitCommitTree.hpp"
#include "GitCatFile.hpp"

int main() {

    // Index &index = Index::getInstance();
    // hashObjectInterface("Hello world", InArgType::kRawString, ObjectType::kBlob, true);
    // updateIndexCacheInfo(index,FileMode::kRegular,"70c379b63ffa0795fdbfbc128e5a2818397b7ef8","1.txt",true);
    // SHAString sha= index.writeTree(false);
    // std::cout << sha << '\n';
    
    auto hash= commitTree("message", "tree");
    std::cout << hash << std::endl;
    std::cout << catFile(hash) << '\n';

    std::cout << std::is_trivial_v<ConditionCheck> << '\n';

}