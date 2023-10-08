#include <iostream>
#include <string>
#include <optional>
#include <string_view>
#include <sstream>
#include <fstream>
using namespace std::string_view_literals;

#include "GitHashObject.hpp"

int main() {
//    std::cout << hashObject("what is up, doc?") << std::endl;
//    std::cout << hashObject("what is up, doc?\0") << std::endl;
//


    std::ifstream fs("../test.txt");
    std::cout << readStream(fs,100) << "|\n";

    int n;
    fs=std::ifstream("../test.txt");
    fs >> n;
    std::cout << n << '\n';
    std::cout << readStream(fs,100) << "|\n";

    std::cout << readFile("../test.txt");

}