#include <type_traits>
#include <iostream>
#include <map>
#include <string>
#include <fstream>
#include "common.hpp"
using std::cout;


int main () {


    std::cout << std::is_trivially_copyable_v<std::string_view>;

}