add_library(hough_line_cpu OBJECT
    HoughLineShapeDetectionCPU.cpp
)

target_include_directories(hough_line_cpu PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${CMAKE_CURRENT_SOURCE_DIR}/../..
)

target_link_libraries(hough_line_cpu PUBLIC
    core
    config
)
