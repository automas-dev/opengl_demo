
# file(GLOB sources_list LIST_DIRECTORIES true *)
# foreach(dir ${sources_list})
#     IF(IS_DIRECTORY ${dir})
#         add_subdirectory(${dir})
#     ELSE()
#         CONTINUE()
#     ENDIF()
# endforeach()

include_directories(include)

add_subdirectory(00_hello_window)
add_subdirectory(01_hello_triangle)
add_subdirectory(02_hello_quad)
add_subdirectory(03_shader)
add_subdirectory(04_texture)
add_subdirectory(05_buffer)
add_subdirectory(06_frame_buffer)
