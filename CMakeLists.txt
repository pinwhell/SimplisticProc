cmake_minimum_required(VERSION 3.5)
project(simplistic-proc)
find_package(CBuildKit REQUIRED)

option(USE_STATIC_LINKING "Use static linking for the runtime" ON)

if (USE_STATIC_LINKING)
    if (MSVC)
        # MSVC specific static linking flag
        add_compile_options(/MT$<$<CONFIG:Debug>:d>)
    else()
        # GCC/Clang specific static linking flag
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libgcc -static-libstdc++")
    endif()
endif()

string(TOUPPER ${CMAKE_SYSTEM_NAME} CMAKE_SYSTEM_NAME_UPPER)
add_definitions(-D${CMAKE_SYSTEM_NAME_UPPER})
add_subdirectory(src)
add_subdirectory(tests)