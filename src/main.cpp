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
#include "GitUpdateRef.hpp"
#include "GitLog.hpp"

int main() {

    Index &index = Index::getInstance();
    hashObjectInterface("Hello world", InArgType::kRawString, ObjectType::kBlob, true);
    updateIndexCacheInfo(index, FileMode::kRegular, "70c379b63ffa0795fdbfbc128e5a2818397b7ef8", "1.txt", true);
    SHAString sha = index.writeTree(false);
    std::cout << sha << '\n';

    SHAString commit = commitTree("first commit",sha);
    std::cout << commit << '\n';

    updateRef(GitObjectsProxy::getHeadPath(), commit);
//    std::cout << catFile(commit) << '\n';

    printLog();

//    auto ss2 = GitObjectsProxy::getInstance().readObjectNoHeader("70c379b63ffa0795fdbfbc128e5a2818397b7ef8").value();
//    std::cout << ss2.str() << '\n';

}