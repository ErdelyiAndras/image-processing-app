add_library(hough_line_gpu OBJECT
    HoughLineShapeDetectionGPU.cpp
)

target_include_directories(hough_line_gpu PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${CMAKE_CURRENT_SOURCE_DIR}/../..
)

target_link_libraries(hough_line_gpu PUBLIC
    core
    config
)
