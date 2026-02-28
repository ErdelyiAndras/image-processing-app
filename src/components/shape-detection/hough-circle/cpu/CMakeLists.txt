add_library(hough_circle_cpu OBJECT
    HoughCircleShapeDetectionCPU.cpp
)

target_include_directories(hough_circle_cpu PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${CMAKE_CURRENT_SOURCE_DIR}/../..
)

target_link_libraries(hough_circle_cpu PUBLIC
    core
    config
)
