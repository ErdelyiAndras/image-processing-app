add_subdirectory(sobel)

add_library(edge_detection STATIC)

target_include_directories(edge_detection PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(edge_detection PUBLIC
    core
    config
    sobel
)

target_sources(edge_detection PRIVATE
    EdgeDetectionComponent.cpp
    $<TARGET_OBJECTS:sobel>
    $<TARGET_OBJECTS:sobel_cpu>
    $<TARGET_OBJECTS:sobel_gpu>
)
