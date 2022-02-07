cmake_minimum_required(VERSION 3.21)
project(baphomet)

###########
# OPTIONS #
###########

option(BAPHOMET_BUILD_EXAMPLES "Build the baphomet example programs" ON)

################
# DEPENDENCIES #
################

include(cmake/CPM.cmake)

CPMAddPackage(
    NAME fmt
    GITHUB_REPOSITORY fmtlib/fmt
    GIT_TAG 8.1.1)

CPMAddPackage(
    NAME GLFW
    GITHUB_REPOSITORY glfw/glfw
    GIT_TAG 3.3.6
    OPTIONS
      "GLFW_BUILD_TESTS OFF"
      "GLFW_BUILD_EXAMPLES OFF"
      "GLFW_BUILD_DOCS OFF")

CPMAddPackage(
    NAME GLM
    GITHUB_REPOSITORY g-truc/glm
    GIT_TAG 0.9.9.8)

CPMAddPackage(
    NAME libnyquist
    GITHUB_REPOSITORY ddiakopoulos/libnyquist
    GIT_TAG 59a51e4
    OPTIONS
      "BUILD_EXAMPLE OFF")

CPMAddPackage(
    NAME OpenAL
    GITHUB_REPOSITORY kcat/openal-soft
    GIT_TAG 6192493
    OPTIONS
      "ALSOFT_UTILS OFF"
      "ALSOFT_NO_CONFIG_UTIL ON"
      "ALSOFT_EXAMPLES OFF"
      "ALSOFT_INSTALL OFF"
      "ALSOFT_INSTALL_CONFIG OFF"
      "ALSOFT_INSTALL_HRTF_DATA OFF"
      "ALSOFT_INSTALL_AMBDEC_PRESETS OFF"
      "ALSOFT_INSTALL_EXAMPLES OFF"
      "ALSOFT_INSTALL_UTILS OFF")

CPMAddPackage(
    NAME pcg
    GITHUB_REPOSITORY imneme/pcg-cpp
    GIT_TAG ffd522e
    DOWNLOAD_ONLY YES)

CPMAddPackage(
    NAME spdlog
    GITHUB_REPOSITORY gabime/spdlog
    VERSION 1.9.2
    OPTIONS
      "SPDLOG_FMT_EXTERNAL ON")

#################
# LIBRARY FILES #
#################

include(include/CMakeLists.txt)
include(src/CMakeLists.txt)
include(thirdparty/CMakeLists.txt)
add_library(baphomet STATIC ${headers} ${srcs})

#################
# CONFIGURATION #
#################

if (MSVC)
    target_compile_features(baphomet PUBLIC cxx_std_23)
else ()
    target_compile_features(baphomet PUBLIC cxx_std_20)
endif (MSVC)

target_link_libraries(baphomet PUBLIC
    fmt
    glfw
    glm
    libnyquist
    OpenAL
    spdlog)

target_include_directories(baphomet PUBLIC
    include
    ${pcg_SOURCE_DIR}/include
    thirdparty/glad/include
    thirdparty/imgui/include
    thirdparty/imgui/include/backends
    thirdparty/stb
)

if (HADES_BUILD_EXAMPLES)
    add_subdirectory("example")

    if (WIN32)
        # Copy the OpenAL dll over to the example directory
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${OpenAL_BINARY_DIR}/OpenAL32.dll"
                "${CMAKE_BINARY_DIR}/example"
                )
    endif (WIN32)
endif (BAPHOMET_BUILD_EXAMPLES)
