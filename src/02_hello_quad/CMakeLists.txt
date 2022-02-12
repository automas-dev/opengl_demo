get_filename_component(PARENT_DIR ${CMAKE_CURRENT_LIST_DIR} NAME)
set(TARGET ${PARENT_DIR})

add_executable(${TARGET} main.cpp)

target_link_libraries(${TARGET}
    OpenGL::OpenGL
    OpenGL::GLU
    GLEW::GLEW
    sfml-graphics
    glm
)
