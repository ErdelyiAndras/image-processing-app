add_subdirectory(cpu)

add_library(hough_circle OBJECT
    HoughCircleShapeDetectionComponent.cpp
)

target_include_directories(hough_circle PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

target_link_libraries(hough_circle PUBLIC
    core
    config
    hough_circle_cpu
)
