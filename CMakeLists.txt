cmake_minimum_required(VERSION 3.12...3.23)

project(Baphomet VERSION 1.0
        DESCRIPTION "TBD"
        LANGUAGES CXX)

add_library(goatlib STATIC
        src/goat/util/time.cpp
        src/goat/util/ticker.cpp
        src/goat/application.cpp
        src/goat/engine.cpp
        src/goat/inputmgr.cpp
        src/goat/window.cpp
        include/goat/util/enum_bitops.hpp
        include/goat/util/platform.hpp
        include/goat/util/ticker.hpp
        include/goat/util/time.hpp
        include/goat/application.hpp
        include/goat/configs.hpp
        include/goat/engine.hpp
        include/goat/goat.hpp
        include/goat/inputmgr.hpp
        include/goat/window.hpp)

set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
add_subdirectory(extern/fmt)

set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(OPTION_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(OPTION_BUILD_TOOLS OFF CACHE BOOL "" FORCE)
set(OPTION_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(extern/glbinding)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(extern/glfw)

add_subdirectory(extern/glm)

set(SPDLOG_BUILD_EXAMPLE OFF CACHE BOOL "" FORCE)
set(SPDLOG_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(SPDLOG_FMT_EXTERNAL ON CACHE BOOL "" FORCE)
add_subdirectory(extern/spdlog)

target_include_directories(goatlib PUBLIC include)

target_compile_features(goatlib PUBLIC cxx_std_20)

target_link_libraries(goatlib PUBLIC
        fmt::fmt
        glbinding::glbinding
        glfw
        glm::glm
        spdlog::spdlog)

add_subdirectory(example)
