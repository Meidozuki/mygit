package("unittest-cmake-wrap")
    on_load(function(package)
        local configs = {}
        local options = {}
        -- options["cmake_generator"]="Unix Makefiles"
        import("package.tools.cmake").build(package, configs, options)
    end)
package_end()

target("unittest_cmake_wrapper")
    add_packages("gtest")
    add_files("main_body.cpp")
    add_includedirs("$(projectdir)/unittest/cmake", {public = true})
    before_build(function (target)
        os.run("cmake $(projectdir)/unittest/cmake")
    end)

target("unittest")
    set_kind("binary")
    add_packages("gtest")
    add_deps("src_lib", "unittest_cmake_wrapper")
    add_files("test_main.cpp", "*test.cpp")