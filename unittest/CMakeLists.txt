include(FetchContent)
cmake_policy(SET CMP0135 NEW)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

include(cmake/detect_host.cmake)

include(GoogleTest)

function(combine_test arg_test_name arg_src_files)
    add_executable(${arg_test_name} main_body.cpp ${arg_src_files})
    target_link_libraries(${arg_test_name}
            GTest::gtest_main
            src_lib)
    target_include_directories(${arg_test_name} PUBLIC
            ${CMAKE_CURRENT_BINARY_DIR}
            ${CMAKE_SOURCE_DIR}/src)
    gtest_discover_tests(${arg_test_name})
endfunction()

combine_test(hello_test hello_test.cpp)
combine_test(check_types check_type_property.cpp)
combine_test(hash_object_test hash_object_test.cpp)
combine_test(index_alone_test index_test.cpp)
combine_test(update_index_test update_index_test.cpp)
combine_test(objects_proxy_test objects_proxy_test.cpp)