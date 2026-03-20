add_subdirectory(merge)

add_library(pipeline STATIC
    Pipeline.cpp
)

target_include_directories(pipeline PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(pipeline PUBLIC
    core
    merge
)
