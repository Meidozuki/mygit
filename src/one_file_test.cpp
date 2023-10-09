#include <type_traits>
#include <iostream>
#include <map>
#include <string>
#include <fstream>
#include "common.hpp"
using std::cout;


int main () {

    TimeZone t{12,30};
    cout << t<< '\n';
    cout << (-5%3) << '\n';
    cout << std::is_trivially_copyable_v<TimeZone>;
}