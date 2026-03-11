add_library(hough_circle_gpu OBJECT
    HoughCircleShapeDetectionGPU.cpp
)

target_include_directories(hough_circle_gpu PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${CMAKE_CURRENT_SOURCE_DIR}/../..
)

target_link_libraries(hough_circle_gpu PUBLIC
    core
    config
)
