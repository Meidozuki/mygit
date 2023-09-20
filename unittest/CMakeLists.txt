include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(
        hello_test
        test_main.cpp
)
target_link_libraries(
        hello_test
        GTest::gtest_main
        src_lib
)
target_include_directories(
        hello_test PUBLIC
        ${CMAKE_SOURCE_DIR}/src
)

include(GoogleTest)
gtest_discover_tests(hello_test)