cmake_policy(SET CMP0048 NEW)

project(
    hades
    VERSION 0.0.1
    LANGUAGES C CXX
)
cmake_minimum_required(VERSION 3.16)

option(HADES_BUILD_EXAMPLES "Build the hades example programs" ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

add_library(
    ${PROJECT_NAME}
    STATIC

    "thirdparty/glad/include/glad/gl.h"
    "thirdparty/glad/include/KHR/khrplatform.h"
    "thirdparty/glad/src/gl.c"

    "thirdparty/pcg-cpp/include/pcg_extras.hpp"
    "thirdparty/pcg-cpp/include/pcg_random.hpp"

    "thirdparty/stb/include/stb_image.h"
    "thirdparty/stb/include/stb_image_resize.h"
    "thirdparty/stb/include/stb_image_write.h"

    "include/hades/gl/buffer_base.hpp"
    "include/hades/gl/context.hpp"
    "include/hades/gl/context_enums.hpp"
    "include/hades/gl/framebuffer.hpp"
    "include/hades/gl/shader.hpp"
    "include/hades/gl/static_buffer.hpp"
    "include/hades/gl/vec_buffer.hpp"
    "include/hades/gl/vertex_array.hpp"
    "sources/hades/gl/buffer_base.cpp"
    "sources/hades/gl/context.cpp"
    "sources/hades/gl/framebuffer.cpp"
    "sources/hades/gl/shader.cpp"
    "sources/hades/gl/vertex_array.cpp"

    "include/hades/internal/bitmask_enum.hpp"

    "include/hades/util/averagers.hpp"
    "include/hades/util/framecounter.hpp"
    "include/hades/util/random.hpp"
    "include/hades/util/ticker.hpp"
    "include/hades/util/time.hpp"
    "include/hades/util/timermgr.hpp"
    "sources/hades/util/averagers.cpp"
    "sources/hades/util/framecounter.cpp"
    "sources/hades/util/random.cpp"
    "sources/hades/util/ticker.cpp"
    "sources/hades/util/timermgr.cpp"

    "include/hades/color.hpp"
    "include/hades/windowmgr.hpp"
    "include/hades/eventmgr.hpp"
    "include/hades/hades.hpp"
    "include/hades/window.hpp"
    "sources/hades/color.cpp"
    "sources/hades/windowmgr.cpp"
    "sources/hades/eventmgr.cpp"
    "sources/hades/window.cpp"
)

add_subdirectory("thirdparty/fmt")

set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
add_subdirectory("thirdparty/glfw")

add_subdirectory("thirdparty/glm")

set(SPDLOG_FMT_EXTERNAL ON CACHE BOOL "" FORCE)
add_subdirectory("thirdparty/spdlog")

target_link_libraries(
    ${PROJECT_NAME}
    PUBLIC

    glfw
    glm
    fmt
    spdlog
)

target_include_directories(
    ${PROJECT_NAME}
    PUBLIC

    "include"
    "thirdparty/glad/include"
    "thirdparty/pcg-cpp/include"
    "thirdparty/stb/include"
)

if (HADES_BUILD_EXAMPLES)
    add_subdirectory("example")
endif ()

