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

    "thirdparty/imgui/include/imconfig.h"
    "thirdparty/imgui/include/imgui.h"
    "thirdparty/imgui/include/imgui_internal.h"
    "thirdparty/imgui/include/imstb_rectpack.h"
    "thirdparty/imgui/include/imstb_textedit.h"
    "thirdparty/imgui/include/imstb_truetype.h"
    "thirdparty/imgui/include/backends/imgui_impl_glfw.h"
    "thirdparty/imgui/include/backends/imgui_impl_opengl3.h"
    "thirdparty/imgui/include/backends/imgui_impl_opengl3_loader.h"
    "thirdparty/imgui/sources/imgui.cpp"
    "thirdparty/imgui/sources/imgui_demo.cpp"
    "thirdparty/imgui/sources/imgui_draw.cpp"
    "thirdparty/imgui/sources/imgui_tables.cpp"
    "thirdparty/imgui/sources/imgui_widgets.cpp"
    "thirdparty/imgui/sources/backends/imgui_impl_glfw.cpp"
    "thirdparty/imgui/sources/backends/imgui_impl_opengl3.cpp"

    "thirdparty/implot/include/implot.h"
    "thirdparty/implot/include/implot_internal.h"
    "thirdparty/implot/sources/implot.cpp"
    "thirdparty/implot/sources/implot_demo.cpp"
    "thirdparty/implot/sources/implot_items.cpp"

    "thirdparty/pcg-cpp/include/pcg_extras.hpp"
    "thirdparty/pcg-cpp/include/pcg_random.hpp"

    "thirdparty/stb/include/stb_image.h"
    "thirdparty/stb/include/stb_image_resize.h"
    "thirdparty/stb/include/stb_image_write.h"

    "include/gl/buffer_base.hpp"
    "include/gl/context.hpp"
    "include/gl/context_enums.hpp"
    "include/gl/framebuffer.hpp"
    "include/gl/shader.hpp"
    "include/gl/static_buffer.hpp"
    "include/gl/vec_buffer.hpp"
    "include/gl/vertex_array.hpp"
    "include/gl/batching/batch.hpp"
    "include/gl/batching/line_batch.hpp"
    "include/gl/batching/oval_batch.hpp"
    "include/gl/batching/pixel_batch.hpp"
    "include/gl/batching/rect_batch.hpp"
    "include/gl/batching/tri_batch.hpp"
    "sources/gl/buffer_base.cpp"
    "sources/gl/context.cpp"
    "sources/gl/framebuffer.cpp"
    "sources/gl/shader.cpp"
    "sources/gl/vertex_array.cpp"
    "sources/gl/batching/batch.cpp"
    "sources/gl/batching/line_batch.cpp"
    "sources/gl/batching/oval_batch.cpp"
    "sources/gl/batching/pixel_batch.cpp"
    "sources/gl/batching/rect_batch.cpp"
    "sources/gl/batching/tri_batch.cpp"

    "include/hades/internal/bitmask_enum.hpp"
    "include/hades/internal/ts_queue.hpp"

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

    "thirdparty/imgui/include"
    "thirdparty/imgui/include/backends"
    "thirdparty/implot/include"
    "thirdparty/glad/include"
    "thirdparty/pcg-cpp/include"
    "thirdparty/stb/include"

    "include"
)

if (HADES_BUILD_EXAMPLES)
    add_subdirectory("example")
endif ()

