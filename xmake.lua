
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

set_languages("cxx17")

files = {"src/GitHashObject.cpp", 
    "src/objects.cpp", 
    "src/index.cpp", 
    "src/GitUpdateIndex.cpp", 
    "src/GitWriteTree.cpp",
    "src/GitCommitTree.cpp",
    "src/GitCatFile.cpp",
    "src/objects_proxy.cpp"}

target("src_lib")
    set_kind("static")
    add_files(files)
    add_packages("cryptopp")
    add_includedirs("src", {public = true})

target("mygit")
    set_kind("binary")
    add_files("src/main.cpp")
    add_deps("src_lib")

includes("unittest")