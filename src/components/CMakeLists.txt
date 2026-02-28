add_subdirectory(denoising)
add_subdirectory(edge-detection)
add_subdirectory(shape-detection)

add_library(components INTERFACE)
target_link_libraries(components INTERFACE
    denoising
    edge_detection
    shape_detection
)
