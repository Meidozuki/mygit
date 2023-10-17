
target("unittest_cmake_wrapper")
    add_packages("gtest")
    add_files("main_body.cpp")
    add_includedirs("$(projectdir)/unittest/cmake", {public = true})
    before_build(function (target)
        local dir = "$(projectdir)/unittest/cmake"
        os.run("cmake -S "..dir .." -B "..dir)
    end)

target("unittest")
    set_kind("binary")
    add_packages("gtest")
    add_deps("src_lib", "unittest_cmake_wrapper")
    add_files("check_type_property.cpp","*test.cpp")