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
#include "index.hpp"
#include "GitUpdateIndex.hpp"


int main() {
//    index.addEntry<DryRunFile>(DryRunFile("70c379b63ffa0795fdbfbc128e5a2818397b7ef8","1.txt"));
//    index.addEntry<DryRunFile>(DryRunFile("sha222","f1/2.txt"));
//    index.addEntry<DryRunFile>(DryRunFile("sha333","f1/f2/3.txt"));

    Index &index = Index::getInstance();
    std::cout << hashObjectInterface("Hello world", InArgType::kRawString, ObjectType::kBlob, true)<< '\n';
    updateIndexCacheInfo(index,FileMode::kRegular,"70c379b63ffa0795fdbfbc128e5a2818397b7ef8","1.txt",true);
    SHAString sha= index.writeTree();
    std::cout << sha << '\n';

    Tree tree;
    tree.addItem({"1.txt", "70c379b63ffa0795fdbfbc128e5a2818397b7ef8", FileMode::kRegular, ObjectType::kBlob});
    std::cout << '\n';
    std::cout << hashObjectInterface(tree.freeze(),
                                     InArgType::kRawString, ObjectType::kTree) << '\n';

    std::cout << sha << '\n';
}