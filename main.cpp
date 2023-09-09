#include <iostream>
#include <string>
#include <optional>

#include "GitHashObject.hpp"

int main() {
    std::cout << std::string("AAF4C61DDCC5E8A2DABEDE0F3B482CD9AEA9434D").size() << '\n';

    std::cout << "AAF4C61DDCC5E8A2DABEDE0F3B482CD9AEA9434D" << '\n'\
        << HashObject("hello");

    return 0;
}
