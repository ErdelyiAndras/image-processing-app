add_subdirectory(cpu)
add_subdirectory(gpu)

add_library(canny OBJECT
    CannyEdgeDetectionComponent.cpp
)

target_include_directories(canny PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

target_link_libraries(canny PUBLIC
    core
    config
    canny_cpu
    canny_gpu
)
