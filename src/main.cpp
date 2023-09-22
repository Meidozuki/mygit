#include <iostream>
#include <string>
#include <optional>
#include <string_view>
#include <memory>
#include <type_traits>
#include <set>
#include <array>
using namespace std::string_view_literals;

#include "FP_util.hpp"
#include "objects.hpp"
#include "GitHashObject.hpp"
#include "common.hpp"
#include "objects_proxy.hpp"
#include "index.hpp"

int main() {
    std::cout << hashObject("blob 12\0test content"sv) << '\n';

    std::string s("blob 12");
    s.resize(s.size()+1);
    s.append("test content");
    std::string_view view(s);

    std::cout << hashObject(view) << '\n';
    std::cout << hashObjectInterface("test content", InArgType::kRawString, ObjectType::kBlob, true) << '\n';

    GitObjectsProxy::getInstance().initFromDisk();
}