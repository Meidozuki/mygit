cmake_minimum_required(VERSION 3.20)
project(mygit)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS -O0)
add_executable(mygit main.cpp GitHashObject.cpp objects.hpp GitUpdateIndex.cpp GitUpdateIndex.hpp)

add_library(crypto_lib STATIC IMPORTED)

set(lib_prefix ${PROJECT_SOURCE_DIR}/cryptopp)
set_target_properties(crypto_lib PROPERTIES
        IMPORTED_LOCATION ${lib_prefix}/lib/libcryptopp.a)

target_link_libraries(mygit PUBLIC crypto_lib)
target_include_directories(mygit PUBLIC ${lib_prefix}/include)