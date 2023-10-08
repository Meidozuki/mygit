#include <iostream>
#include <string>
#include <optional>
#include <string_view>
#include <memory>
#include <type_traits>
#include <set>
using namespace std::string_view_literals;

#include "FP_util.hpp"
#include "objects.hpp"
//#include "GitHashObject.hpp"


int main() {

    Blob t;
    t.type_=ObjectType::kBlob;
    t.sha1_="test sha";
    t.content_ ="test test";
    GitObjectsManager::getInstance().save(t);

    std::cout << sizeof(std::string) << ' ' << sizeof(std::array<char,40>);


}