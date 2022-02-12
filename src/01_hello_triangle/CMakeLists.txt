set(TARGET 01_hello_triangle)

add_executable(${TARGET} main.cpp)

target_link_libraries(${TARGET}
    OpenGL::OpenGL
    OpenGL::GLU
    GLEW::GLEW
    sfml-graphics
)
