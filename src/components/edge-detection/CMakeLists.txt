add_subdirectory(sobel)
add_subdirectory(canny)

add_library(edge_detection STATIC)

target_include_directories(edge_detection PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(edge_detection PUBLIC
    core
    config
    sobel
    canny
)

target_sources(edge_detection PRIVATE
    $<TARGET_OBJECTS:sobel>
    $<TARGET_OBJECTS:sobel_cpu>
    $<TARGET_OBJECTS:sobel_gpu>
    $<TARGET_OBJECTS:canny>
    $<TARGET_OBJECTS:canny_cpu>
    $<TARGET_OBJECTS:canny_gpu>
)
