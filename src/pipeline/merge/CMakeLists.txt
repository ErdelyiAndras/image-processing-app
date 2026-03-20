add_library(merge INTERFACE)

target_include_directories(merge INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(merge INTERFACE
    core
)
