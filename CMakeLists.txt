cmake_minimum_required(VERSION 3.15...3.20)

project(OpenGL_Demo
    VERSION 0.1
    DESCRIPTION "An awesome opengl demo"
    LANGUAGES CXX)

include(GNUInstallDirs)

find_package(Threads REQUIRED)
find_package(SFML 2.5 REQUIRED CONFIG COMPONENTS graphics window system)
find_package(GLEW REQUIRED)
find_package(OpenGL REQUIRED)
find_package(glm REQUIRED CONFIG)

include_directories(stb)

add_subdirectory(examples)
