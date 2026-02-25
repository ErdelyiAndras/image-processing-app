add_subdirectory(cpu)
add_subdirectory(gpu)

add_library(sobel OBJECT
    SobelEdgeDetectionComponent.cpp
)

target_include_directories(sobel PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

target_link_libraries(sobel PUBLIC
    core
    config
    sobel_cpu
    sobel_gpu
)
