cmake_minimum_required(VERSION 3.20)
project(mygit)

set(CMAKE_CXX_STANDARD 17)
#set(CMAKE_CXX_FLAGS -O0)

set(SRC_FILES src/GitHashObject.cpp src/objects.cpp src/index.cpp src/sha1_proxy.hpp)
add_executable(mygit src/main.cpp ${SRC_FILES})
add_library(src_lib STATIC ${SRC_FILES})
add_executable(debug_test src/one_file_test.cpp)

target_precompile_headers(mygit PUBLIC src/precompile.h)

add_subdirectory(cryptopp)

target_link_libraries(mygit PUBLIC Crypto_LIB)
target_include_directories(mygit PUBLIC ${Crypto_INCLUDE_DIR}/include)
target_link_libraries(src_lib PUBLIC Crypto_LIB)
target_include_directories(src_lib PUBLIC ${Crypto_INCLUDE_DIR}/include)

add_subdirectory(unittest)