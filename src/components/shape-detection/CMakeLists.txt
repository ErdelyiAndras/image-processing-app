add_subdirectory(hough-line)
add_subdirectory(hough-circle)

add_library(shape_detection STATIC)

target_include_directories(shape_detection PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(shape_detection PUBLIC
    core
    config
    hough_line
    hough_circle
)

target_sources(shape_detection PRIVATE
    ShapeDetectionComponent.cpp
    $<TARGET_OBJECTS:hough_line>
    $<TARGET_OBJECTS:hough_line_cpu>
    $<TARGET_OBJECTS:hough_circle>
    $<TARGET_OBJECTS:hough_circle_cpu>
)
