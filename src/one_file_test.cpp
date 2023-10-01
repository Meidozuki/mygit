#include <type_traits>
#include <iostream>
#include <map>
#include <string>
#include "common.hpp"
using std::cout;


int main () {

    std::array<unsigned char,40> a={' ','a','b'};
    cout << a.data();


//    std::error_code ec;
//    cout << ec.value() << '\n';
//    std::cout << filesys::weakly_canonical(t,ec) << '\n';
//    cout << ec.value() << '\n';
//    std::cout << filesys::canonical(t,ec) << '\n';
//    cout << ec.value() << '\n';
//    cout << std::error_code().value();

}