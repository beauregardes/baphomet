cmake_minimum_required(VERSION 3.12...3.23)

project(Baphomet VERSION 1.0
        DESCRIPTION "TBD"
        LANGUAGES CXX)

if(MSVC)
    set(CMAKE_CXX_STANDARD 23)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
else()
    set(CMAKE_CXX_STANDARD 20)
endif()

add_library(goatlib STATIC
        extern/imgui/src/backends/imgui_impl_glfw.cpp
        extern/imgui/src/backends/imgui_impl_opengl3.cpp
        extern/imgui/src/imgui.cpp
        extern/imgui/src/imgui_demo.cpp
        extern/imgui/src/imgui_draw.cpp
        extern/imgui/src/imgui_tables.cpp
        extern/imgui/src/imgui_widgets.cpp
        extern/imgui/include/backends/imgui_impl_glfw.h
        extern/imgui/include/backends/imgui_impl_opengl3.h
        extern/imgui/include/backends/imgui_impl_opengl3_loader.h
        extern/imgui/include/imconfig.h
        extern/imgui/include/imgui.h
        extern/imgui/include/imgui_internal.h
        extern/imgui/include/imstb_rectpack.h
        extern/imgui/include/imstb_textedit.h
        extern/imgui/include/imstb_truetype.h
        extern/stb/stb_image.h
        extern/stb/stb_image_write.h

        src/goat/util/time.cpp
        src/goat/util/ticker.cpp
        src/goat/application.cpp
        src/goat/dear.cpp
        src/goat/engine.cpp
        src/goat/inputmgr.cpp
        src/goat/window.cpp
        include/goat/util/enum_bitops.hpp
        include/goat/util/platform.hpp
        include/goat/util/ticker.hpp
        include/goat/util/time.hpp
        include/goat/application.hpp
        include/goat/configs.hpp
        include/goat/dear.hpp
        include/goat/engine.hpp
        include/goat/goat.hpp
        include/goat/inputmgr.hpp
        include/goat/window.hpp)

find_package(Vulkan)
if(Vulkan)
    add_library(goatlib STATIC
        extern/imgui/src/backends/imgui_impl_vulkan.cpp
        extern/imgui/include/backends/imgui_impl_vulkan.h)
endif()

set(FREETYPE_LIBRARY ${CMAKE_CURRENT_SOURCE_DIR}/extern/msdf-atlas-gen/msdfgen/freetype/win64/freetype.lib)
set(FREETYPE_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/extern/msdf-atlas-gen/msdfgen/freetype/include)

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

set(MSDF_ATLAS_GEN_BUILD_STANDALONE OFF CACHE BOOL "" FORCE)
add_subdirectory(extern/msdf-atlas-gen)

set(SPDLOG_BUILD_EXAMPLE OFF CACHE BOOL "" FORCE)
set(SPDLOG_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(SPDLOG_FMT_EXTERNAL ON CACHE BOOL "" FORCE)
add_subdirectory(extern/spdlog)

target_include_directories(goatlib PUBLIC
        ${FREETYPE_INCLUDE_DIRS}
        extern/imgui/include/backends
        extern/imgui/include
        extern/stb
        include)

target_compile_features(goatlib PUBLIC cxx_std_20)

target_link_libraries(goatlib PUBLIC
        fmt::fmt
        ${FREETYPE_LIBS}
        glbinding::glbinding
        glfw
        glm::glm
        msdf-atlas-gen::msdf-atlas-gen
        spdlog::spdlog)

if(Vulkan)
    target_include_directories(goatlib PUBLIC ${Vulkan_INCLUDE_DIRS})
    target_link_libraries(goatlib PUBLIC ${Vulkan_LIBRARIES})
endif()

add_subdirectory(example)
