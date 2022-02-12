
set(TARGET support)

set(HEADER_LIST
    debug.hpp
    Shader.hpp
)
list(TRANSFORM HEADER_LIST PREPEND "${${PROJECT_NAME}_SOURCE_DIR}/include/")

set(SOURCE_LIST
    Shader.cpp
)
list(TRANSFORM SOURCE_LIST PREPEND "${${PROJECT_NAME}_SOURCE_DIR}/src/")

add_library(${TARGET} ${SOURCE_LIST} ${HEADER_LIST} ${${PROJECT_NAME}_SOURCE_DIR}/stb/stb_image.h)

target_include_directories(${TARGET} PUBLIC
    $<BUILD_INTERFACE:${${PROJECT_NAME}_SOURCE_DIR}/include>
    # $<BUILD_INTERFACE:${${PROJECT_NAME}_SOURCE_DIR}/stb>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

target_compile_features(${TARGET} PUBLIC cxx_std_17)

target_link_libraries(${TARGET}
    OpenGL::OpenGL
    GLEW::GLEW
    glm)
