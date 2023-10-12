#include <type_traits>
#include <iostream>
#include <map>
#include <string>
#include <fstream>
#include <sstream>
#include "common.hpp"
using std::cout;

struct A{
    void f1(){}

    template<class T>
    void f(const T& x) {
        std::cout << "base " << x << '\n';
    }

    template<class T>
    void f2(const T& x) {
        std::cout << "base f2" << x << '\n';
    }
};

struct B:A{
    void f1() =delete;

    template<class T>
    void f(T& x) {
        std::cout << "derived " << x << '\n';
    }
};


int main () {
    std::cout << std::is_trivially_copyable_v<std::optional<int>>;
}