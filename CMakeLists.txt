cmake_minimum_required(VERSION 3.20)
project(mygit)

set(CMAKE_CXX_STANDARD 17)
#set(CMAKE_CXX_FLAGS -O0)

set(SRC_FILES src/GitHashObject.cpp src/objects.cpp src/index.cpp
        src/GitUpdateIndex.cpp src/GitWriteTree.cpp src/GitCommitTree.cpp)
add_library(src_lib STATIC ${SRC_FILES})
target_precompile_headers(src_lib PUBLIC src/precompile.h)

add_executable(mygit src/main.cpp)
add_executable(debug_test src/one_file_test.cpp)

add_subdirectory(cryptopp)

target_link_libraries(src_lib PUBLIC Crypto_LIB)
target_include_directories(src_lib PUBLIC ${Crypto_INCLUDE_DIR}/include)
target_link_libraries(mygit PUBLIC src_lib)

add_subdirectory(unittest)