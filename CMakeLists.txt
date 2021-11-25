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

    "include/gl/buffer_base.hpp"
    "include/hades/gfxmgr.hpp"
    "include/gl/context_enums.hpp"
    "include/gl/framebuffer.hpp"
    "include/gl/shader.hpp"
    "include/gl/static_buffer.hpp"
    "include/gl/texture_unit.hpp"
    "include/gl/vec_buffer.hpp"
    "include/gl/vertex_array.hpp"
    "include/gl/batching/batch.hpp"
    "include/gl/batching/line_batch.hpp"
    "include/gl/batching/oval_batch.hpp"
    "include/gl/batching/pixel_batch.hpp"
    "include/gl/batching/rect_batch.hpp"
    "include/gl/batching/texture_batch.hpp"
    "include/gl/batching/tri_batch.hpp"
    "sources/gl/buffer_base.cpp"
    "sources/hades/gfxmgr.cpp"
    "sources/gl/framebuffer.cpp"
    "sources/gl/shader.cpp"
    "sources/gl/texture_unit.cpp"
    "sources/gl/vertex_array.cpp"
    "sources/gl/batching/batch.cpp"
    "sources/gl/batching/line_batch.cpp"
    "sources/gl/batching/oval_batch.cpp"
    "sources/gl/batching/pixel_batch.cpp"
    "sources/gl/batching/rect_batch.cpp"
    "sources/gl/batching/texture_batch.cpp"
    "sources/gl/batching/tri_batch.cpp"

    "include/hades/internal/batch_set.hpp"
    "include/hades/internal/bitmask_enum.hpp"
    "include/hades/internal/resource_loader.hpp"
    "include/hades/internal/ts_queue.hpp"
    "sources/hades/internal/batch_set.cpp"
    "sources/hades/internal/resource_loader.cpp"

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

    "include/hades/application.hpp"
    "include/hades/color.hpp"
    "include/hades/inputmgr.hpp"
    "include/hades/hades.hpp"
    "include/hades/runner.hpp"
    "include/hades/texture.hpp"
    "include/hades/window.hpp"
    "include/hades/font/cp437.hpp"
    "sources/hades/application.cpp"
    "sources/hades/color.cpp"
    "sources/hades/inputmgr.cpp"
    "sources/hades/runner.cpp"
    "sources/hades/texture.cpp"
    "sources/hades/window.cpp"
    "sources/hades/font/cp437.cpp"
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

    "thirdparty/glad/include"
    "thirdparty/pcg-cpp/include"
    "thirdparty/stb/include"

    "include"
)

if (HADES_BUILD_EXAMPLES)
    add_subdirectory("example")
endif ()

