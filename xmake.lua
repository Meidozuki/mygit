-- add_requires("conan::cryptopp/8.8.0")
package("cryptopp")
    add_deps("cmake")
    set_sourcedir(path.join(os.scriptdir(), "cryptopp"))
    on_install(function (package)
        local configs = {}
        import("package.tools.cmake").install(package, configs)
    end)
    on_fetch(function (package)
        local result = {}
        result.links = "cryptopp"
        result.linkdirs = path.join(package:sourcedir(), "lib")
        result.includedirs = path.join(package:sourcedir(), "include")
        return result
    end)
package_end()

add_requires("cryptopp")
add_requires("gtest")

files = {"src/GitHashObject.cpp", "src/objects.cpp", "src/index.cpp"}

target("src_lib")
    set_kind("static")
    add_files(files)
    add_packages("cryptopp")

target("mygit")
    set_kind("binary")
    add_files("src/main.cpp", files)
    add_packages("cryptopp")

target("mytest")
    set_kind("binary")
    add_packages("gtest")
    add_deps("src_lib")
    add_includedirs("src")
    add_files("unittest/test_main.cpp")