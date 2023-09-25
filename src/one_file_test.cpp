#include <type_traits>
#include <iostream>

struct A{
    A(){}
};

struct B:A{};

int main () {
    std::cout << std::is_base_of_v<A, B> << '\n';
    std::cout << std::is_base_of_v<A, const B> << '\n';
    std::cout << std::is_base_of_v<const A, const B> << '\n';
    std::cout << std::is_base_of_v<A, B&> << '\n';
}