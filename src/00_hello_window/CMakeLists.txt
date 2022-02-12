set(TARGET 00_hello_window)

add_executable(${TARGET} main.cpp)

target_link_libraries(${TARGET}
    OpenGL::OpenGL
    OpenGL::GLU
    GLEW::GLEW
    sfml-graphics
)
