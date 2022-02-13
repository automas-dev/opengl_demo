cmake_minimum_required(VERSION 3.15...3.20)

project(OpenGL_Demo
    VERSION 0.1
    DESCRIPTION "An awesome opengl demo"
    LANGUAGES CXX)

# Set the default build type if non is specified
set(default_build_type "Release")
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message(STATUS "Setting build type to '${default_build_type}' since none was specified.")
    set(CMAKE_BUILD_TYPE "${default_build_type}" CACHE
        STRING "Choose the type of build." FORCE)

    # Set possible values of build type for cmake-gui
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
        "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

include(GNUInstallDirs)

find_package(Threads REQUIRED)
find_package(SFML 2.5 REQUIRED CONFIG COMPONENTS graphics window system)
find_package(GLEW REQUIRED)
find_package(OpenGL REQUIRED)
find_package(glm REQUIRED CONFIG)

include_directories(stb)

add_subdirectory(examples)
