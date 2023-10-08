message(STATUS "Host system: " ${CMAKE_HOST_SYSTEM})

if (${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Linux"
    AND NOT(${CMAKE_HOST_SYSTEM} MATCHES "WSL"))
    set(Linux_file_permission true)
elseif(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Windows")
    set(Windows_file_permission true)
elseif(${CMAKE_HOST_SYSTEM} MATCHES "WSL")
    set(WSL_file_permission true)
endif()

if (${WSL_file_permission})
    message(NOTICE "In the WSL filesystem, the behavior varies from Windows part to Linux part")
    if (${CMAKE_CURRENT_SOURCE_DIR} MATCHES "/mnt/[a-z]/")
        set(Windows_file_permission true)
    else()
        set(Linux_file_permission true)
    endif()
endif()

file(WRITE "config.h.in" 
"#cmakedefine Windows_file_permission
#cmakedefine Linux_file_permission
#cmakedefine WSL_file_permission")

configure_file(config.h.in config.h)