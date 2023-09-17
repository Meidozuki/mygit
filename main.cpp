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
#include "sha1_proxy.hpp"

namespace test{
class A{
    int x;
 public:
    A() =default;
};

}


int main() {

    std::cout << hashObject("blob 12") << '\n';
    std::cout << hashObject("test content") << '\n';
    std::cout << hashObject("blob 12\0test content"sv) << '\n';

    std::string s("blob 12");
    s.resize(s.size()+1);
    s.append("test content");
    std::string_view view(s);

    std::cout << hashObject(view) << '\n';
    std::cout << hashObjectInterface("test content") << '\n';


}