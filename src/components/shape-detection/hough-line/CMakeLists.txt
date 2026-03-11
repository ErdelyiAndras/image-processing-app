add_subdirectory(cpu)
add_subdirectory(gpu)

add_library(hough_line OBJECT
    HoughLineShapeDetectionComponent.cpp
)

target_include_directories(hough_line PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

target_link_libraries(hough_line PUBLIC
    core
    config
    hough_line_cpu
    hough_line_gpu
)
