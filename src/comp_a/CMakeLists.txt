set(TARGET comp_a)

set(HEADER_LIST
    foo.hpp)
list(TRANSFORM HEADER_LIST PREPEND "include/${PROJECT_NAME}/${TARGET}/")

set(SOURCE_LIST
    foo.cpp)
list(TRANSFORM SOURCE_LIST PREPEND "src/")

add_library(${TARGET} ${HEADER_LIST} ${SOURCE_LIST})

# target_link_libraries(${TARGET} PRIVATE fmt::fmt)

library_component(${TARGET})